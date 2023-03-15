#include "chunk.h"


Chunk::Chunk(OpenGLContext* context, glm::ivec2 pos) :
    Drawable(context),
    m_blocks(),
    m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}},
    m_pos(pos),
    hasSetBlocks(false), rain(false), snow(false)
{
    std::fill_n(m_blocks.begin(), 65536, EMPTY);
}

Chunk::~Chunk(){}

void Chunk::createVBOdata() {
   std::vector<glm::vec4> opaque_VBOdata;
   std::vector<GLuint> opaque_idx;

   std::vector<glm::vec4> translucent_VBOdata;
   std::vector<GLuint> translucent_idx;
   int curr_opaque_idx = 0;
   int curr_translucent_idx = 0;

   for (int x = 0; x < 16; x++)
   {
        for (int y = 0; y < 256; y++)
        {
            for (int z = 0; z < 16; z++)
            {

                BlockType currentType = getBlockAt(x, y, z);
                glm::vec4 currentPos = glm::vec4(x, y, z, 0);
                if (currentType != EMPTY)
                {
                    for (BlockNeighbor neighborFace : neighborFaces)
                    {
                        if((currentType == WATER || currentType == LAVA) && neighborFace.dir != YPOS)
                        {
                            continue;
                        }
                        glm::vec3 neighborPos = neighborFace.offset + glm::vec3(x, y, z);
                        BlockType neighborType;
                        if ((x == 0 || z == 0 || x == 15 || z == 15) and neighborFace.dir != YPOS and neighborFace.dir != YNEG)
                        {
                            Chunk* neighborChunk = m_neighbors[neighborFace.dir];
                            if (neighborChunk == nullptr)
                            {
                                neighborType = EMPTY;
//                                continue;
                            }
                            else
                            {
                                if (neighborFace.dir == XPOS && x == 15)
                                {
                                    neighborType = neighborChunk->getBlockAt(int(0), int(neighborPos.y), int(neighborPos.z));

                                }
                                else if (neighborFace.dir == XNEG && x == 0)
                                {
                                    neighborType = neighborChunk->getBlockAt(int(15), int(neighborPos.y), int(neighborPos.z));
                                }
                                else if (neighborFace.dir == ZPOS && z == 15)
                                {
                                    neighborType = neighborChunk->getBlockAt(int(neighborPos.x), int(neighborPos.y), int(0));
                                }
                                else if (neighborFace.dir == ZNEG && z == 0)
                                {
                                    neighborType = neighborChunk->getBlockAt(int(neighborPos.x), int(neighborPos.y), int(15));
                                }
                                else
                                {
                                    neighborType = getBlockAt(int(neighborPos.x), int(neighborPos.y), int(neighborPos.z));
                                }
                            }

                        }
                        else if(y == 0 || y==255)
                        {
                            neighborType = EMPTY;
                        }
                        else
                        {
                            neighborType = getBlockAt(int(neighborPos.x), int(neighborPos.y), int(neighborPos.z));
                        }

                        if (neighborType == EMPTY || (neighborType == WATER && currentType != WATER))
                        {
                            Direction d = neighborFace.dir;
                            glm::vec4 uv = blockFaceUVs[currentType][d];
                            if(currentType == LAVA)
                                uv.z = 1;
                            else if(currentType == WATER)
                                uv.z = 2;

                            if(currentType == LAVA || currentType == WATER
                                    || currentType == BIRCHLEAF || currentType == CACTUS)
                            {
                                for (int i = 0; i < 4; i++)
                                {
                                    translucent_VBOdata.push_back(neighborFace.vertices[i].m_pos + currentPos);
                                    translucent_VBOdata.push_back(glm::vec4(neighborFace.offset, 0));
                                    auto temp = uv + glm::vec4(neighborFace.vertices[i].m_uv, 0, 0);
                                    translucent_VBOdata.push_back(uv + glm::vec4(neighborFace.vertices[i].m_uv, 0, 0));
                                }

                                // update translucent idx
                                translucent_idx.push_back(curr_translucent_idx);
                                translucent_idx.push_back(curr_translucent_idx + 1);
                                translucent_idx.push_back(curr_translucent_idx + 2);
                                translucent_idx.push_back(curr_translucent_idx);
                                translucent_idx.push_back(curr_translucent_idx + 2);
                                translucent_idx.push_back(curr_translucent_idx + 3);
                                curr_translucent_idx += 4;
                            }
                            else
                            {
                                for (int i = 0; i < 4; i++)
                                {
                                    opaque_VBOdata.push_back(neighborFace.vertices[i].m_pos + currentPos);
                                    opaque_VBOdata.push_back(glm::vec4(neighborFace.offset, 0));
                                    opaque_VBOdata.push_back(uv + glm::vec4(neighborFace.vertices[i].m_uv, 0, 0));
                                }

                                // update opaque idx
                                opaque_idx.push_back(curr_opaque_idx);
                                opaque_idx.push_back(curr_opaque_idx + 1);
                                opaque_idx.push_back(curr_opaque_idx + 2);
                                opaque_idx.push_back(curr_opaque_idx);
                                opaque_idx.push_back(curr_opaque_idx + 2);
                                opaque_idx.push_back(curr_opaque_idx + 3);
                                curr_opaque_idx += 4;
                            }
                        }
                    }
                }
            }
        }
    }

   chunkVBOData.opaque_idxData = opaque_idx;
   chunkVBOData.opaque_vboData = opaque_VBOdata;
   chunkVBOData.trans_idxData = translucent_idx;
   chunkVBOData.trans_vboData = translucent_VBOdata;

   hasCreatVBOData = true;
}

void Chunk::loadVBOdata()
{
    this->m_opaqueCount = chunkVBOData.opaque_idxData.size();
    generateOpaqueIdx();
    bindOpaqueIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             chunkVBOData.opaque_idxData.size() * sizeof(GLuint),
                             chunkVBOData.opaque_idxData.data(),
                             GL_STATIC_DRAW);

    generateOpaque();
    bindOpaque();
    mp_context->glBufferData(GL_ARRAY_BUFFER,
                             chunkVBOData.opaque_vboData.size() * sizeof(glm::vec4),
                             chunkVBOData.opaque_vboData.data(),
                             GL_STATIC_DRAW);

    this->m_translucentCount = chunkVBOData.trans_idxData.size();
    generateTranslucentIdx();
    bindTranslucentIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             chunkVBOData.trans_idxData.size() * sizeof(GLuint),
                             chunkVBOData.trans_idxData.data(),
                             GL_STATIC_DRAW);

    generateTranslucent();
    bindTranslucent();
    mp_context->glBufferData(GL_ARRAY_BUFFER,
                             chunkVBOData.trans_vboData.size() * sizeof(glm::vec4),
                             chunkVBOData.trans_vboData.data(),
                             GL_STATIC_DRAW);

    m_count = m_opaqueCount + m_translucentCount;
    hasBindVBOData = true;
}

GLenum Chunk::drawMode()
{
   return GL_TRIANGLES;
}

// Does bounds checking with at()
BlockType Chunk::getBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
    return m_blocks.at(x + 16 * y + 16 * 256 * z);
}

// Exists to get rid of compiler warnings about int -> unsigned int implicit conversion
BlockType Chunk::getBlockAt(int x, int y, int z) const {
    return getBlockAt(static_cast<unsigned int>(x), static_cast<unsigned int>(y), static_cast<unsigned int>(z));
}

// Does bounds checking with at()
void Chunk::setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t) {
    long int index = x + 16 * y + 16 * 256 * z;
    if(index >= 65536)
    {
        std::cout << " x: " << x << ", y: " << y << ", z: " << z << std::endl;
        return;
    }
    m_blocks.at(index) = t;
}


const static std::unordered_map<Direction, Direction, EnumHash> oppositeDirection
{
    {XPOS, XNEG},
    {XNEG, XPOS},
    {YPOS, YNEG},
    {YNEG, YPOS},
    {ZPOS, ZNEG},
    {ZNEG, ZPOS}
};

void Chunk::linkNeighbor(uPtr<Chunk> &neighbor, Direction dir) {
    if(neighbor != nullptr) {
        this->m_neighbors[dir] = neighbor.get();
        neighbor->m_neighbors[oppositeDirection.at(dir)] = this;
    }
}

void Chunk::drawCactus(int x, int z, int height, int tallness) {
    for (int y = height; y < height + tallness; y++) {
        setBlockAt(x, y, z, CACTUS);
    }

    setBlockAt(x+1, height + 3, z, CACTUS);
    for(int j = height + 2; j < height + tallness - 2; j++) {
        setBlockAt(x-2, j, z, CACTUS);
    }
    for(int j = height + 3; j < height + tallness - 1; j++) {
        setBlockAt(x+2, j, z, CACTUS);
    }
    setBlockAt(x-1, height + 2, z, CACTUS);
}

void Chunk::drawTree(int x, int z, int height) {
    // tree trunk height 2
    for (int y = height; y < height + 4; y++) {
        setBlockAt(x, y, z, BIRCH);
    }
    //bottom layer
    int bottom = height + 4;
    for (int i = x - 2; i <= x + 2; i++) {
        for (int j = z - 2; j <= z + 2; j++) {
            if (getBlockAt(i, bottom, j) == EMPTY) {
                setBlockAt(i, bottom, j, BIRCHLEAF);
            }
        }
     }
    //middle
    int middle = height + 5;
    for (int i = x - 1; i <= x + 1; i++) {
        for (int k = z - 1; k <= z + 1; k++) {
            for (int j = middle; j <= middle + 2; j+=2) {
                 setBlockAt(i, middle, k, BIRCHLEAF);
            }
        }
    }
    //top cross
    int top = height + 6;
    for (int i = x - 1; i <= x + 1; i++) {
       setBlockAt(i, top, z, BIRCHLEAF);
    }
    for (int j = z - 1; j <= z + 1; j++) {
       setBlockAt(x, top, j, BIRCHLEAF);
    }
    //post trunk
    for (int y = height + 4; y < height + 6; y++) {
        setBlockAt(x, y, z, BIRCH);
    }

}

void Chunk::drawMashRoom(int x, int z, int height) {
    for (int y = height; y < height + 2; y++) {
        setBlockAt(x, y, z, MASHROOMSTEM);
    }

    int bottom = height + 2;
    for (int i = x - 2; i <= x + 2; i++) {
        for (int k = z - 1; k <= z + 1; k++) {
            for (int j = bottom; j < bottom + 2; j++) {
                setBlockAt(i, j, k, MASHROOMLEAF);
            }
        }
    }

    for (int i = x - 1; i <= x + 1; i++) {
        for (int k = z - 2; k <= z + 2; k+=4) {
            for (int j = bottom; j < bottom + 2; j++) {
                setBlockAt(i, j, k, MASHROOMLEAF);
            }
        }
    }

    for (int i = x - 1; i <= x + 1; i++) {
        for (int k = z - 1; k <= z + 1; k++) {
            setBlockAt(i, bottom + 2, k, MASHROOMLEAF);
        }
    }
}
