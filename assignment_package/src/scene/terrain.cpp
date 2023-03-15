#include "terrain.h"
#include "cube.h"
#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <stdlib.h>

Terrain::Terrain(OpenGLContext *context)
    : m_chunks(), m_generatedTerrain(), mp_context(context), lsystem_water_block(), lsystem_empty_block(), //water_block(), empty_block(),
      m_geomSnow(context), m_geomRain(context),
      generateSnow(false), snowSmallScale(false), snowMidScale(false),
      xStart(-10000), xEnd(10000), zStart(-10000), zEnd(10000)
{
    lsystem = new LSystem(QString("FFFX"), 138, 10000, 316, 10000);
    // lsystem1 = new LSystem(QString("FFFFFY"), 0, 0, 0, 0);
}


Terrain::~Terrain() {
    m_geomRain.destroyVBOdata();
    m_geomSnow.destroyVBOdata();
    //m_geomCube.destroyVBOdata();
}

// Combine two 32-bit ints into one 64-bit int
// where the upper 32 bits are X and the lower 32 bits are Z
int64_t toKey(int x, int z) {
    int64_t xz = 0xffffffffffffffff;
    int64_t x64 = x;
    int64_t z64 = z;

    // Set all lower 32 bits to 1 so we can & with Z later
    xz = (xz & (x64 << 32)) | 0x00000000ffffffff;

    // Set all upper 32 bits to 1 so we can & with XZ
    z64 = z64 | 0xffffffff00000000;

    // Combine
    xz = xz & z64;
    return xz;
}

int64_t toKey(int x, int y, int z) {
    int64_t xyz = 0xffffffffffffffff;
    int64_t x64 = x;
    int64_t y64 = y;
    int64_t z64 = z;

    // Set all lower 32 bits to 1 so we can & with Z later
    xyz = (xyz & (x64 << 32)) | 0x00000000ffffffff;

    // Set all upper 32 bits to 1 so we can & with XZ
    z64 = z64 | 0xffffffff00000000;
    y64 = y64 | 0xffffffff00000000;

    // Combine
    xyz = xyz & z64;
    xyz = xyz & y64;
    return xyz;
}

glm::ivec2 toCoords(int64_t k) {
    // Z is lower 32 bits
    int64_t z = k & 0x00000000ffffffff;
    // If the most significant bit of Z is 1, then it's a negative number
    // so we have to set all the upper 32 bits to 1.
    // Note the 8    V
    if(z & 0x0000000080000000) {
        z = z | 0xffffffff00000000;
    }
    int64_t x = (k >> 32);

    return glm::ivec2(x, z);
}

// Surround calls to this with try-catch if you don't know whether
// the coordinates at x, y, z have a corresponding Chunk
BlockType Terrain::getBlockAt(int x, int y, int z) const
{
    if(hasChunkAt(x, z)) {
        // Just disallow action below or above min/max height,
        // but don't crash the game over it.
        if(y < 0 || y >= 256) {
            return EMPTY;
        }
        const uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        return c->getBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                             static_cast<unsigned int>(y),
                             static_cast<unsigned int>(z - chunkOrigin.y));
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

BlockType Terrain::getBlockAt(glm::vec3 p) const {
    return getBlockAt(p.x, p.y, p.z);
}

bool Terrain::hasChunkAt(int x, int z) const {
    // Map x and z to their nearest Chunk corner
    // By flooring x and z, then multiplying by 16,
    // we clamp (x, z) to its nearest Chunk-space corner,
    // then scale back to a world-space location.
    // Note that floor() lets us handle negative numbers
    // correctly, as floor(-1 / 16.f) gives us -1, as
    // opposed to (int)(-1 / 16.f) giving us 0 (incorrect!).
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    auto it = m_chunks.find(toKey(16 * xFloor, 16 * zFloor));
    return it != m_chunks.end() && it->second != nullptr;
}


uPtr<Chunk>& Terrain::getChunkAt(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks[toKey(16 * xFloor, 16 * zFloor)];
}

const uPtr<Chunk>& Terrain::getChunkAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.at(toKey(16 * xFloor, 16 * zFloor));
}

void Terrain::setBlockAt(int x, int y, int z, BlockType t)
{
    if(hasChunkAt(x, z)) {
        uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        c->setBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                      static_cast<unsigned int>(y),
                      static_cast<unsigned int>(z - chunkOrigin.y),
                      t);
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

Chunk* Terrain::instantiateChunkAt(int x, int z) {
    newChunks[toKey(x, z)] = mkU<Chunk>(mp_context, glm::ivec2(x, z));
    Chunk* chunk = newChunks[toKey(x, z)].get();
    // Set the neighbor pointers of itself and its neighbors
    if(hasChunkAt(x, z + 16)) {
        auto &chunkNorth = getChunkAt(x, z + 16);
        chunk->linkNeighbor(chunkNorth, ZPOS);
    }
    if(hasChunkAt(x, z - 16)) {
        auto &chunkSouth = getChunkAt(x, z - 16);
        chunk->linkNeighbor(chunkSouth, ZNEG);
    }
    if(hasChunkAt(x + 16, z)) {
        auto &chunkEast = getChunkAt(x + 16, z);
        chunk->linkNeighbor(chunkEast, XPOS);
    }
    if(hasChunkAt(x - 16, z)) {
        auto &chunkWest = getChunkAt(x - 16, z);
        chunk->linkNeighbor(chunkWest, XNEG);
    }

    if (hasNewChunkAt(x, z + 16)) {
        auto &chunkNorth = getNewChunkAt(x, z + 16);
        chunk->linkNeighbor(chunkNorth, ZPOS);
    }
    if (hasNewChunkAt(x, z - 16)) {
        auto &chunkSouth = getNewChunkAt(x, z - 16);
        chunk->linkNeighbor(chunkSouth, ZNEG);
    }
    if (hasNewChunkAt(x + 16, z)) {
        auto &chunkEast = getNewChunkAt(x + 16, z);
        chunk->linkNeighbor(chunkEast, XPOS);
    }
    if (hasNewChunkAt(x - 16, z)) {
        auto &chunkWest = getNewChunkAt(x - 16, z);
        chunk->linkNeighbor(chunkWest, XNEG);
    }

    return chunk;
}

// TODO: When you make Chunk inherit from Drawable, change this code so
// it draws each Chunk with the given ShaderProgram, remembering to set the
// model matrix to the proper X and Z translation!
void Terrain::draw(int minX, int maxX, int minZ, int maxZ, ShaderProgram *shaderProgram) {
    for(int x = minX; x < maxX; x += 16)
    {
        for(int z = minZ; z < maxZ; z += 16)
        {
            if (hasChunkAt(x, z))
            {
                const uPtr<Chunk> &chunk = getChunkAt(x, z);
                //if(chunk->opaqueElemCount() > 0)
                //{
                if(!chunk->hasBindVBOData)
                {
                    chunk->loadVBOdata();
                }
                    shaderProgram->setModelMatrix(glm::translate(glm::mat4(1.f), glm::vec3(x, 0.f, z)));
                    shaderProgram->drawOpaque(*chunk);
                //}

            }
        }
    }

    for(int x = minX; x < maxX; x += 16)
    {
        for(int z = minZ; z < maxZ; z += 16)
        {
            if (hasChunkAt(x, z))
            {
                const uPtr<Chunk> &chunk = getChunkAt(x, z);
                //if(chunk->translElemCount() > 0)
                //{
                if(!chunk->hasBindVBOData)
                {
                    chunk->loadVBOdata();
                }
                    shaderProgram->setModelMatrix(glm::translate(glm::mat4(1.f), glm::vec3(x, 0.f, z)));
                    shaderProgram->drawTranslucent(*chunk);
                //}
            }
        }
    }
}


void Terrain::CreatChunkBlocks(Chunk* chunk)
{
    int chunkPosX = chunk->m_pos[0];
    int chunkPosZ = chunk->m_pos[1];
    for(int x = 0; x < 16; x++) {
        for(int z = 0; z < 16; z++) {            

            // Bedrock
            chunk->setBlockAt(x, 0, z, BEDROCK);

            // Rock and Cave
            for (int y = 1; y <= 128; y++)
            {
                drawCave(chunkPosX + x, y, chunkPosZ + z, x, y, z, chunk);
            }

            float thresHold = Noise::perlinNoise2D(glm::vec2(chunkPosX + x, chunkPosZ + z) / 1000.f);
            thresHold = (thresHold + 1) * 0.5;
            thresHold = glm::smoothstep(0.25, 0.75, (double) thresHold);
            int height = Noise::calculateHeight(chunkPosX + x, chunkPosZ + z, thresHold);
            if (thresHold < 0.5)
            {
                // GrassLand
                // int height = Noise::grasslands(x, z) + 5;
                for (int y = 128; y < height; y++)
                {
                    chunk->setBlockAt(x, y, z, DIRT);
                }
                chunk->setBlockAt(x, height, z, GRASS);

//                if (x > 3 && x < 12  && z > 3 && z < 12 && chunk->getBlockAt(x, height+1, z) == EMPTY)
//                {
//                    int mashRoomRandom = rand() % 2000;
//                    int treesRandom = rand() % 500;
//                    if(treesRandom < 25 && height > 140)
//                    {
//                        chunk->drawBirch(x, z, height + 1);
//                    }
//                    else if(mashRoomRandom > 1960 && height < 145)
//                    {
//                        chunk->drawMashRoom(x, z, height + 1);
//                    }
//                }
            }
            else if (thresHold > 0.5)
            {
                // Mountain
                // int height = Noise::mountains(x, z);
                if (height >= 200)
                {
                    for (int y = 128; y < height; y++)
                    {
                        chunk->setBlockAt(x, y, z, STONE);
                    }
                    chunk->setBlockAt(x, height, z, SNOW);
                }
                else
                {
                    for (int y = 128; y <= height; y++)
                    {
                        chunk->setBlockAt(x, y, z, STONE);

                        if(y == height)
                        {
                            int cactusRandom = rand() % 200;
                            if(cactusRandom > 191)
                            {
                                if (x >= 6 && x <= 9  && z >= 6 && z <= 9)
                                {
                                    chunk->drawCactus(x, z, y + 1, cactusRandom % 3 + 6);
                                }
                            }
                        }
                    }
                }
            }

            // Water
            for (int y = 128; y <= 138; y++)
            {
                if (chunk->getBlockAt(x, y, z) == EMPTY)
                {
                    chunk->setBlockAt(x, y, z, WATER);
                }
            }

            // L-system (Water)
            for (int y = 137; y <= 138; y++)
            {
                if (chunk->getBlockAt(x, y, z) != WATER)
                {
                    //if (lsystem_water_block.find(toKey(chunkPosX + x, y, chunkPosZ + z)) != lsystem_water_block.end())
                    if (lsystem_water_block.find(toKey(chunkPosX + x, chunkPosZ + z)) != lsystem_water_block.end())
                    //if (water_block.contains(glm::vec2(x, z)))
                    {
                        chunk->setBlockAt(x, y, z, WATER);
                    }
                }
            }

            // Empty Check (L-system)
            for (int y = 139; y < 256; y++)
            {
                // if the glock is already EMPTY, there's no reason we shoud set it as EMPTY.
                if (chunk->getBlockAt(x, y, z) != EMPTY)
                {
                    if (lsystem_empty_block.find(toKey(chunkPosX + x, chunkPosZ + z)) != lsystem_empty_block.end())
                    {
                        chunk->setBlockAt(x, y, z, EMPTY);
                    }
                }
            }

            if (thresHold < 0.4)
            {
                if (x > 3 && x < 12  && z > 3 && z < 12 && height > 138 && chunk->getBlockAt(x, height, z) == GRASS)
                {
                    int mashRoomRandom = rand() % 2000;
                    int treesRandom = rand() % 500;
                    if(treesRandom < 25 && height > 142)
                    {
                        chunk->drawTree(x, z, height + 1);
                    }
                    else if(mashRoomRandom > 1960 && height < 143)
                    {
                        chunk->drawMashRoom(x, z, height + 1);
                    }
                }
            }
        }
    }
    
    chunk->hasSetBlocks = true;
}

void Terrain::CreateTestScene()
{
    // TODO: DELETE THIS LINE WHEN YOU DELETE m_geomCube!
    //m_geomCube.createVBOdata();

    // Create the Chunks that will
    // store the blocks for our
    // initial world space
    for(int x = xStart; x < xEnd; x += 16)
    {
        for(int z = zStart; z < zEnd; z += 16)
        {
            instantiateChunkAt(x, z);
        }
    }
    // Tell our existing terrain set that
    // the "generated terrain zone" at (0,0)
    // now exists.
    m_generatedTerrain.insert(toKey(-512, -512));

    // Create the basic terrain floor
    for(int x = xStart; x < xEnd; ++x)
    {
        for(int z = zStart; z < zEnd; ++z)
        {
            // Bedrock
            setBlockAt(x, 0, z, BEDROCK);

            // Rock and Cave
            for (int y = 1; y <= 128; y++)
            {
                drawCave1(x, y, z);
            }

            float thresHold = Noise::perlinNoise2D(glm::vec2(x, z) / 1000.f);
            thresHold = (thresHold + 1) * 0.5;
            thresHold = glm::smoothstep(0.25, 0.75, (double) thresHold);
            int height = Noise::calculateHeight(x, z, thresHold);
            if (thresHold < 0.5)
            {
                // GrassLand
                // int height = Noise::grasslands(x, z) + 5;
                for (int y = 128; y < height; y++)
                {
                    setBlockAt(x, y, z, DIRT);
                }
                setBlockAt(x, height, z, GRASS);
            }
            else if (thresHold > 0.5)
            {
                // Mountain
                // int height = Noise::mountains(x, z);
                if (height >= 200)
                {
                    for (int y = 128; y < height; y++)
                    {
                        setBlockAt(x, y, z, WATER);
                    }
                    setBlockAt(x, height, z, SNOW);
                }
                else
                {
                    for (int y = 128; y <= height; y++)
                    {
                        setBlockAt(x, y, z, WATER);
                    }
                }
            }

            // Water
            for (int y = 128; y <= 138; y++)
            {
                if (getBlockAt(x, y, z) == EMPTY)
                {
                    setBlockAt(x, y, z, WATER);
                }
            }
        }
    }
       /*for(int x = -512; x < 64; x += 16) {
           for(int z = -512; z < 64; z += 16) {
               if (hasChunkAt(x, z))
               {
                   const uPtr<Chunk> &chunk = getChunkAt(x, z);
                   chunk->createVBOdata();
                   chunk->loadVBOdata();
               }
           }
       }*/
}


void Terrain::drawCave1(int x, int y, int z)
{
    float thresHold = Noise::perlinNoise3D(glm::vec3(x, y, z) / 256.f);
    if (thresHold < 0)
    {
        if (y < 25)
        {
            setBlockAt(x, y, z, LAVA);
        } else {
            setBlockAt(x, y, z, EMPTY);
        }
    } else {
        setBlockAt(x, y, z, STONE);
    }
}

void Terrain::drawCave(int worldX, int worldY, int worldZ, int localX, int localY, int localZ, Chunk* chunk)
{
    float thresHold = Noise::perlinNoise3D(glm::vec3(worldX, worldY, worldZ) / 256.f);
    if (thresHold < 0)
    {
        if (localY < 25)
        {
            chunk->setBlockAt(localX, localY, localZ, LAVA);
        } else {
            chunk->setBlockAt(localX, localY, localZ, EMPTY);
        }
    } else {
        chunk->setBlockAt(localX, localY, localZ, STONE);
    }
}

void Terrain::initializeLSystem(LSystem* l_system)
{
    //Expanding the axiom for n iterations
    for (int i = 0; i < 5; i++)
    {
        l_system->axiom = l_system->expandGrammar(l_system->axiom);
    }

    //Match rules to each character in the axiom (defined in lsystems.cpp)
    int count = 0;
    while (count < l_system->axiom.length())
    {
        l_system->executeRule(l_system->axiom.at(count), count);
        count = count + 1;
    }

    //Trace the turtle's route and update blocks correspondingly from the states stored in the turtleHistory stack
    Turtle start = l_system->turtleHistory.first();
    l_system->turtleHistory.pop_front();

    while(l_system->turtleHistory.size() > 1)
    {
        Turtle nextTurtle = l_system->turtleHistory.first();
        l_system->turtleHistory.pop_front();
        //Only drawRoute if depth of next turtle is 1 more than start turtle
        //This facilitates branching logic and prevents rotations from being drawn
        if(nextTurtle.depth == start.depth + 1)
        {
            drawRoute(start, nextTurtle);
        }
        start = nextTurtle;
    }
}

void Terrain::drawRoute(Turtle startTurtle, Turtle nextTurtle)
{
    int start_x = startTurtle.pos.x;
    int end_x = nextTurtle.pos.x;
    int start_z = startTurtle.pos.y;
    int end_z = nextTurtle.pos.y;
    float distance = sqrt(pow(end_x - start_x, 2) + pow(end_z - start_z, 2));
    float x_incr = (end_x - start_x) / distance;
    float z_incr = (end_z - start_z) / distance;
    int width = std::max(4 - nextTurtle.depth/3, 2);
    //int width = 0;

    int drawX, drawZ;

    for(int i = 1; i <= distance; i++) {
        //Check within boundary that has been rendered
        if(start_x + (i * x_incr) < xEnd && start_x + (i * x_incr) > xStart && start_z + (i * z_incr) < zEnd && start_z + (i * z_incr) > zStart)
        {
            //Increment x and z values by the width and setBlockAt those positions as well to give the river some thickness
            for(int d = -width; d <= width; d++)
            {
                drawX = start_x + (i * x_incr) + d;
                drawZ = start_z + (i * z_incr) + d;
                if(start_x + (i * x_incr) + d < xEnd && start_x + (i * x_incr) + d > xStart && start_z + (i * z_incr) + d < zEnd && start_z + (i * z_incr) + d > zStart)
                {
//                    water_block.append(glm::vec2(int(start_x + (i * x_incr) + d), int(start_z + (i * z_incr))));
//                    water_block.append(glm::vec2(int(start_x + (i * x_incr)), int(start_z + (i * z_incr) + d)));
//                    water_block.append(glm::vec2(int(start_x + (i * x_incr) + d), int(start_z + (i * z_incr) + d)));

                    lsystem_water_block[toKey(int(start_x + (i * x_incr) + d), int(start_z + (i * z_incr)))] = glm::vec3(int(start_x + (i * x_incr) + d), 140, int(start_z + (i * z_incr)));
                    lsystem_water_block[toKey(int(start_x + (i * x_incr)), int(start_z + (i * z_incr) + d))] = glm::vec3(int(start_x + (i * x_incr)), 140, int(start_z + (i * z_incr) + d));
                    lsystem_water_block[toKey(int(start_x + (i * x_incr) + d), int(start_z + (i * z_incr) + d))] = glm::vec3(int(start_x + (i * x_incr) + d), 140, int(start_z + (i * z_incr) + d));

//                    setBlockAt(start_x + (i * x_incr) + d, 138, start_z + (i * z_incr), WATER);
//                    setBlockAt(start_x + (i * x_incr), 138, start_z + (i * z_incr) + d, WATER);
//                    setBlockAt(start_x + (i * x_incr) + d, 138, start_z + (i * z_incr) + d, WATER);

//                    setBlockAt(start_x + (i * x_incr) + d, 139, start_z + (i * z_incr), WATER);
//                    setBlockAt(start_x + (i * x_incr), 139, start_z + (i * z_incr) + d, WATER);
//                    setBlockAt(start_x + (i * x_incr) + d, 139, start_z + (i * z_incr) + d, WATER);

                    //Set all blocks above the river to be empty
                    for(int y = 139; y < 256; y++)
                    {
//                        empty_block.append(glm::vec3(int(start_x + (i * x_incr) + d), y, int(start_z + (i * z_incr))));
//                        empty_block.append(glm::vec3(int(start_x + (i * x_incr)), y, int(start_z + (i * z_incr) + d)));
//                        empty_block.append(glm::vec3(int(start_x + (i * x_incr) + d), y, int(start_z + (i * z_incr) + d)));

                        lsystem_empty_block[toKey(int(start_x + (i * x_incr) + d), int(start_z + (i * z_incr)))] = glm::vec3(int(start_x + (i * x_incr) + d), y, int(start_z + (i * z_incr)));
                        lsystem_empty_block[toKey(int(start_x + (i * x_incr)), int(start_z + (i * z_incr) + d))] = glm::vec3(int(start_x + (i * x_incr)), y, int(start_z + (i * z_incr) + d));
                        lsystem_empty_block[toKey(int(start_x + (i * x_incr) + d), int(start_z + (i * z_incr) + d))] = glm::vec3(int(start_x + (i * x_incr) + d), y, int(start_z + (i * z_incr) + d));

//                        setBlockAt(start_x + (i * x_incr) + d, y, start_z + (i * z_incr), EMPTY);
//                        setBlockAt(start_x + (i * x_incr), y, start_z + (i * z_incr) + d, EMPTY);
//                        setBlockAt(start_x + (i * x_incr) + d, y, start_z + (i * z_incr) + d, EMPTY);
                    }
                }
            }

            //To smooth edges of the river
            for(int d = -width * 4; d <= width * 4; d++)
            {
                if(d < -width || d > width)
                {
                    if(start_x + (i * x_incr) + d < xEnd && start_x + (i * x_incr) + d > xStart && start_z + (i * z_incr) + d < zEnd && start_z + (i * z_incr) + d > zStart)
                    {
                        for(int y = 138 + fabs(d) - width; y < 256; y++)
                        {
//                            empty_block.append(glm::vec3(int(start_x + (i * x_incr) + d), y, int(start_z + (i * z_incr))));
//                            empty_block.append(glm::vec3(int(start_x + (i * x_incr)), y, int(start_z + (i * z_incr) + d)));
//                            empty_block.append(glm::vec3(int(start_x + (i * x_incr)) + d, y, int(start_z + (i * z_incr) + d)));

//                            lsystem_empty_block[toKey(int(start_x + (i * x_incr) + d), y, int(start_z + (i * z_incr)))] = glm::vec3(int(start_x + (i * x_incr) + d), y, int(start_z + (i * z_incr)));
//                            lsystem_empty_block[toKey(int(start_x + (i * x_incr)), y, int(start_z + (i * z_incr) + d))] = glm::vec3(start_x + (i * x_incr), y, int(start_z + (i * z_incr) + d));
//                            lsystem_empty_block[toKey(int(start_x + (i * x_incr)) + d, y, int(start_z + (i * z_incr) + d))] = glm::vec3(start_x + (i * x_incr) + d, y, int(start_z + (i * z_incr) + d));

//                            setBlockAt(start_x + (i * x_incr) + d, y, start_z + (i * z_incr), EMPTY);
//                            setBlockAt(start_x + (i * x_incr), y, start_z + (i * z_incr) + d, EMPTY);
//                            setBlockAt(start_x + (i * x_incr) + d, y, start_z + (i * z_incr) + d, EMPTY);
                        }
                    }
                }
            }
        }
    }//for each increment of distance from start to next turtle

}


bool Terrain::hasZoneAt(int x, int z) const
{
    int groundX = static_cast<int>(glm::floor(x / 64.f));
    int groundZ = static_cast<int>(glm::floor(z / 64.f));
    return m_generatedTerrain.find(toKey(64 * groundX, 64 * groundZ)) != m_generatedTerrain.end();
}

std::vector<glm::ivec2> Terrain::getNeighborAreas(int x, int z, int r)
{
    int radius = r * 64;
    int groundX = static_cast<int>(glm::floor(x / 64.f));
    int groundZ = static_cast<int>(glm::floor(z / 64.f));
    int currX = 64 * groundX;
    int currZ = 64 * groundZ;

    std::vector<glm::ivec2> neighborAreas = {};
    for (int i = currX - radius; i <= currX + radius; i += 64)
    {
        for (int j = currZ - radius; j <= currZ + radius; j+= 64)
        {
            neighborAreas.push_back(glm::ivec2(i, j));
        }
    }
    return neighborAreas;
}

void Terrain::terrainCheck(glm::vec3 playerPos, glm::vec3 prev_playerPos)
{
    int r = 2;
//    glm::vec2 pPos(playerPos.x, playerPos.z);
//    glm::ivec2 chunk(16 * glm::ivec2(glm::floor(pPos / 16.f)));
//    int posX = chunk.x;
//    int posZ = chunk.y;
//    for(int x = posX - 96; x < posX + 96; x += 16)
//    {
//       for(int z = posZ - 96; z < posZ + 96; z += 16)
//       {
//           if (!hasChunkAt(x, z))
//           {
//               uPtr<Chunk> &_chunk = getChunkAt(x,z);
//               if(_chunk->elemCount() <= 0)
//               {
//                   _chunk->createVBOdata();
//                   _chunk->loadVBOdata();
//               }
//               instantiateChunkAt(x,z);
//           }
//        }
//     }

    newZones = getNeighborAreas(playerPos.x, playerPos.z, r);

    std::unordered_set<int64_t> newZonesMap;

    for (glm::ivec2 newZone : newZones)
    {
        newZonesMap.insert(toKey(newZone.x, newZone.y));
        if (!hasZoneAt(newZone[0], newZone[1]))
        {
            m_generatedTerrain.insert(toKey(newZone.x, newZone.y));
            for (int x = 0; x < 64; x += 16)
            {
                for (int z = 0; z < 64; z += 16)
                {
                    instantiateChunkAt(newZone.x + x, newZone.y + z);
                }
            }
        }
    }

    for(glm::ivec2 prevZone : prevZones)
    {
        if(newZonesMap.find(toKey(prevZone[0], prevZone[1])) == newZonesMap.end())
        {
            glm::vec2 prePos = glm::vec2(prevZone[0], prevZone[1]);
            for(int x = prePos.x; x < prePos.x + 64; x+=16){
                for(int z = prePos.y; z < prePos.y + 64; z+=16)
                {
                    if(hasChunkAt(x,z))
                    {
                        getChunkAt(x, z)->destroyVBOdata();
                    }
                }
            }
        }
    }


    for (auto & [key, chunk]: newChunks)
    {
        BlockTypeWorkers.push_back(std::thread(&Terrain::BlockTypeWorker, this, move(chunk)));
    }

    newChunks.clear();

    BlockTypeMutex.lock();
    for (auto & [key, chunk] : BlockTypesMap)
    {
        VBOWorkers.push_back(std::thread(&Terrain::VBOWorker, this, move(chunk)));
    }
    BlockTypesMap.clear();
    BlockTypeMutex.unlock();

    VBOdataMutex.lock();
    for (auto & [key, chunk] : VBOdataMap)
    {
        chunk->loadVBOdata();
        m_chunks[key] = move(chunk);
    }
    VBOdataMap.clear();
    VBOdataMutex.unlock();

    prevZones.assign(newZones.begin(), newZones.end());
    newZones.clear();
}

void Terrain::BlockTypeWorker(uPtr<Chunk> chunk)
{
    CreatChunkBlocks(chunk.get());
    BlockTypeMutex.lock();
    BlockTypesMap[toKey(chunk->m_pos[0], chunk->m_pos[1])] = move(chunk);
    BlockTypeMutex.unlock();
}


void Terrain::VBOWorker(uPtr<Chunk> chunk)
{
    chunk->createVBOdata();

    VBOdataMutex.lock();
    VBOdataMap[toKey(chunk->m_pos[0], chunk->m_pos[1])] = move(chunk);
    VBOdataMutex.unlock();
}

bool Terrain::hasNewChunkAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));

    return newChunks.find(toKey(16 * xFloor, 16 * zFloor)) != newChunks.end();
}

uPtr<Chunk>& Terrain::getNewChunkAt(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));

    return newChunks[toKey(16 * xFloor, 16 * zFloor)];
}

void Terrain::endThreads() {
    for (auto &thread: BlockTypeWorkers) {
        thread.join();
    }
    for (auto &thread: VBOWorkers) {
        thread.join();
    }
}

bool Terrain::rain(int x, int y, int z) const
{
    if(y < 0 || y >= 256)
    {
        return false;
    }
    const uPtr<Chunk> &c = getChunkAt(x, z);
    glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
    return c->rain;
}
bool Terrain::snow(int x, int y, int z) const
{
    const uPtr<Chunk> &c = getChunkAt(x, z);
    glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
    return c->snow;
}

void Terrain::draw_snow( ShaderProgram *shaderProgram)
{
    if(m_geomSnow.elemCount() >= 0)
    {
       shaderProgram->drawInstanced(m_geomSnow);
    }
}

void Terrain::create_snow(int minX, int maxX, int minZ, int maxZ, int playerPosY, int scale){
    m_geomSnow.clearOffsetBuf();
    m_geomSnow.clearColorBuf();
    std::vector<glm::vec3>  colors, offsets_snow;
    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            if(hasChunkAt(x, z))
            {
                const uPtr<Chunk> &chunk = getChunkAt(x, z);
                chunk->snow = true;
                for(float i = 0; i < 16; i += rand()%scale)
                {
                    for(float j = playerPosY; j <= playerPosY + 100; j = j+ rand()%(10*scale))
                    {
                        for(float k = 0; k < 16; k += rand()%scale)
                        {
                            float p = rand()%100;

                            if(p > 90)
                            {
                                offsets_snow.push_back(glm::vec3(i+x+rand()%10/25.f, j+rand()%10/25.f, k+z+rand()%10/25.f));
                                colors.push_back(glm::vec3(255.f, 250.f, 250.f) / 255.f);
                            }
                        }
                    }
                }
            }
        }
    }

     m_geomSnow.createInstancedVBOdata(offsets_snow, colors);
}

void Terrain::draw_rain( ShaderProgram *shaderProgram) {
    if(m_geomRain.elemCount() >= 0)
    {
       shaderProgram->drawInstanced(m_geomRain);
    }
}

void Terrain::create_rain(int minX, int maxX, int minZ, int maxZ, int playerPosY){
    m_geomRain.clearOffsetBuf();
    m_geomRain.clearColorBuf();
    std::vector<glm::vec3>  colors, offsets_rain;
    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            if(hasChunkAt(x, z))
            {
                const uPtr<Chunk> &chunk = getChunkAt(x, z);
                 chunk->rain = true;

                for(float i = 0; i < 16; i += rand()%5)
                {
                    for(float j = playerPosY + 60; j <= playerPosY + 660; j = j+ rand()%30)
                    {
                        for(float k = 0; k < 16; k += rand()%5)
                        {
                            if(rand()%200 > 125 + (playerPosY + 800 - j)/10.f)
                            {
                                offsets_rain.push_back(glm::vec3(i+x+rand()%10/25.f, j+rand()%10/25.f, k+z+rand()%10/25.f));
                                colors.push_back(glm::vec3(80.f, 150.f, 255.f) / 255.f);
                            }
                        }
                    }
                }
            }
        }
    }

     m_geomRain.createInstancedVBOdata(offsets_rain, colors);
}
