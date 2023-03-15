#pragma once
#include "smartpointerhelp.h"
#include "glm_includes.h"
#include <array>
#include <unordered_map>
#include <cstddef>
#include "drawable.h"

#include <iostream>

#define BLK_UVX * 0.0625
#define BLK_UVY * 0.0625
#define BLK_UV 0.0625

//using namespace std;

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.
enum BlockType : unsigned char
{
    EMPTY, GRASS, DIRT, STONE, WATER, SNOW, LAVA, SAND, WOOD, ICE, BEDROCK,
    ACACIA,ACACIALEAF,MASHROOMSTEM,MASHROOMLEAF,SNOWLEAF,DARKWOOD,BIRCH,BIRCHLEAF,CACTUS
};

// The six cardinal directions in 3D space
enum Direction : unsigned char
{
    XPOS, XNEG, YPOS, YNEG, ZPOS, ZNEG
};

// Lets us use any enum class as the key of a
// std::unordered_map
struct EnumHash {
    template <typename T>
    size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};

struct VertexData
{
    glm::vec4 m_pos;
    glm::vec2 m_uv;

    VertexData(glm::vec4 pos, glm::vec2 uv)
        : m_pos(pos), m_uv(uv)
    {}
};

struct BlockNeighbor
{
    Direction dir;
    glm::vec3 offset;
    std::array<VertexData, 4> vertices;
    BlockNeighbor(Direction dir, glm::vec3 off, const VertexData &a,
              const VertexData &b, const VertexData &c,
              const VertexData &d)
        : dir(dir), offset(off), vertices{a, b, c, d}
    {}
};

struct ChunkVBOData
{
    // opaque
    //std::vector<glm::vec4> vboData;
    std::vector<glm::vec4> opaque_vboData;
    //std::vector<GLuint> idxData;
    std::vector<GLuint> opaque_idxData;
    // transparent
    std::vector<glm::vec4> trans_vboData;
    std::vector<GLuint> trans_idxData;
};

const static std::array<BlockNeighbor, 6> neighborFaces{
    //+X
    BlockNeighbor(XPOS, glm::vec3(1, 0, 0),
              VertexData(glm::vec4(1, 0, 1, 1), glm::vec2(0, 0)),
              VertexData(glm::vec4(1, 0, 0, 1), glm::vec2(BLK_UV, 0)),
              VertexData(glm::vec4(1, 1, 0, 1), glm::vec2(BLK_UV, BLK_UV)),
              VertexData(glm::vec4(1, 1, 1, 1), glm::vec2(0, BLK_UV))),

    //-X
    BlockNeighbor(XNEG, glm::vec3(-1, 0, 0),
              VertexData(glm::vec4(0, 0, 0, 1), glm::vec2(0, 0)),
              VertexData(glm::vec4(0, 0, 1, 1), glm::vec2(BLK_UV, 0)),
              VertexData(glm::vec4(0, 1, 1, 1), glm::vec2(BLK_UV, BLK_UV)),
              VertexData(glm::vec4(0, 1, 0, 1), glm::vec2(0, BLK_UV))),

    //+Y
    BlockNeighbor(YPOS, glm::vec3(0, 1, 0),
              VertexData(glm::vec4(0, 1, 1, 1), glm::vec2(0, 0)),
              VertexData(glm::vec4(1, 1, 1, 1), glm::vec2(BLK_UV, 0)),
              VertexData(glm::vec4(1, 1, 0, 1), glm::vec2(BLK_UV, BLK_UV)),
              VertexData(glm::vec4(0, 1, 0, 1), glm::vec2(0, BLK_UV))),
    //-Y
    BlockNeighbor(YNEG, glm::vec3(0, -1, 0),
              VertexData(glm::vec4(0, 0, 0, 1), glm::vec2(0, 0)),
              VertexData(glm::vec4(1, 0, 0, 1), glm::vec2(BLK_UV, 0)),
              VertexData(glm::vec4(1, 0, 1, 1), glm::vec2(BLK_UV, BLK_UV)),
              VertexData(glm::vec4(0, 0, 1, 1), glm::vec2(0, BLK_UV))),
    //+Z
    BlockNeighbor(ZPOS, glm::vec3(0, 0, 1),
              VertexData(glm::vec4(0, 0, 1, 1), glm::vec2(0, 0)),
              VertexData(glm::vec4(1, 0, 1, 1), glm::vec2(BLK_UV, 0)),
              VertexData(glm::vec4(1, 1, 1, 1), glm::vec2(BLK_UV, BLK_UV)),
              VertexData(glm::vec4(0, 1, 1, 1), glm::vec2(0, BLK_UV))),

    //-Z
    BlockNeighbor(ZNEG, glm::vec3(0, 0, -1),
              VertexData(glm::vec4(1, 0, 0, 1), glm::vec2(0, 0)),
              VertexData(glm::vec4(0, 0, 0, 1), glm::vec2(BLK_UV, 0)),
              VertexData(glm::vec4(0, 1, 0, 1), glm::vec2(BLK_UV, BLK_UV)),
              VertexData(glm::vec4(1, 1, 0, 1), glm::vec2(0, BLK_UV)))
};

static std::unordered_map<BlockType,
std::unordered_map<Direction, glm::vec4, EnumHash>, EnumHash> blockFaceUVs{
    {GRASS, std::unordered_map<Direction, glm::vec4, EnumHash>{
            {XPOS, glm::vec4(3.f BLK_UVX, 15.f BLK_UVY, 0, 0)},
            {XNEG, glm::vec4(3.f BLK_UVX, 15.f BLK_UVY, 0, 0)},
            {YPOS, glm::vec4(8.f BLK_UVX, 13.f BLK_UVY, 0, 0)},
            {YNEG, glm::vec4(2.f BLK_UVX, 15.f BLK_UVY, 0, 0)},
            {ZPOS, glm::vec4(3.f BLK_UVX, 15.f BLK_UVY, 0, 0)},
            {ZNEG, glm::vec4(3.f BLK_UVX, 15.f BLK_UVY, 0, 0)}
        }},
    {DIRT, std::unordered_map<Direction, glm::vec4, EnumHash>{
            {XPOS, glm::vec4(2.f BLK_UVX, 15.f BLK_UVY, 0, 0)},
            {XNEG, glm::vec4(2.f BLK_UVX, 15.f BLK_UVY, 0, 0)},
            {YPOS, glm::vec4(2.f BLK_UVX, 15.f BLK_UVY, 0, 0)},
            {YNEG, glm::vec4(2.f BLK_UVX, 15.f BLK_UVY, 0, 0)},
            {ZPOS, glm::vec4(2.f BLK_UVX, 15.f BLK_UVY, 0, 0)},
            {ZNEG, glm::vec4(2.f BLK_UVX, 15.f BLK_UVY, 0, 0)}
        }},
    {STONE, std::unordered_map<Direction, glm::vec4, EnumHash>{
            {XPOS, glm::vec4(1.f BLK_UVX, 15.f BLK_UVY, 0, 0)},
            {XNEG, glm::vec4(1.f BLK_UVX, 15.f BLK_UVY, 0, 0)},
            {YPOS, glm::vec4(1.f BLK_UVX, 15.f BLK_UVY, 0, 0)},
            {YNEG, glm::vec4(1.f BLK_UVX, 15.f BLK_UVY, 0, 0)},
            {ZPOS, glm::vec4(1.f BLK_UVX, 15.f BLK_UVY, 0, 0)},
            {ZNEG, glm::vec4(1.f BLK_UVX, 15.f BLK_UVY, 0, 0)}
        }},
    {WATER, std::unordered_map<Direction, glm::vec4, EnumHash>{
            {XPOS, glm::vec4(14.f BLK_UVX, 2.f BLK_UVY, 0, 0)},
            {XNEG, glm::vec4(14.f BLK_UVX, 2.f BLK_UVY, 0, 0)},
            {YPOS, glm::vec4(14.f BLK_UVX, 2.f BLK_UVY, 0, 0)},
            {YNEG, glm::vec4(14.f BLK_UVX, 2.f BLK_UVY, 0, 0)},
            {ZPOS, glm::vec4(14.f BLK_UVX, 2.f BLK_UVY, 0, 0)},
            {ZNEG, glm::vec4(14.f BLK_UVX, 2.f BLK_UVY, 0, 0)}
        }},
    {LAVA, std::unordered_map<Direction, glm::vec4, EnumHash>{
            {XPOS, glm::vec4(14.f BLK_UVX, 0.f BLK_UVY, 0, 0)},
            {XNEG, glm::vec4(14.f BLK_UVX, 0.f BLK_UVY, 0, 0)},
            {YPOS, glm::vec4(14.f BLK_UVX, 0.f BLK_UVY, 0, 0)},
            {YNEG, glm::vec4(14.f BLK_UVX, 0.f BLK_UVY, 0, 0)},
            {ZPOS, glm::vec4(14.f BLK_UVX, 0.f BLK_UVY, 0, 0)},
            {ZNEG, glm::vec4(14.f BLK_UVX, 0.f BLK_UVY, 0, 0)}
        }},
    {SNOW, std::unordered_map<Direction, glm::vec4, EnumHash>{
            {XPOS, glm::vec4(2.f BLK_UVX, 11.f BLK_UVY, 0, 0)},
            {XNEG, glm::vec4(2.f BLK_UVX, 11.f BLK_UVY, 0, 0)},
            {YPOS, glm::vec4(2.f BLK_UVX, 11.f BLK_UVY, 0, 0)},
            {YNEG, glm::vec4(2.f BLK_UVX, 11.f BLK_UVY, 0, 0)},
            {ZPOS, glm::vec4(2.f BLK_UVX, 11.f BLK_UVY, 0, 0)},
            {ZNEG, glm::vec4(2.f BLK_UVX, 11.f BLK_UVY, 0, 0)}
        }},
    {SAND, std::unordered_map<Direction, glm::vec4, EnumHash>{
            {XPOS, glm::vec4(2.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {XNEG, glm::vec4(2.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {YPOS, glm::vec4(2.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {YNEG, glm::vec4(2.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {ZPOS, glm::vec4(2.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {ZNEG, glm::vec4(2.f BLK_UVX, 14.f BLK_UVY, 0, 0)}
        }},
    {WOOD, std::unordered_map<Direction, glm::vec4, EnumHash>{
            {XPOS, glm::vec4(4.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {XNEG, glm::vec4(4.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {YPOS, glm::vec4(5.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {YNEG, glm::vec4(5.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {ZPOS, glm::vec4(4.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {ZNEG, glm::vec4(4.f BLK_UVX, 14.f BLK_UVY, 0, 0)}
        }},
    {BEDROCK, std::unordered_map<Direction, glm::vec4, EnumHash>{
            {XPOS, glm::vec4(1.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {XNEG, glm::vec4(1.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {YPOS, glm::vec4(1.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {YNEG, glm::vec4(1.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {ZPOS, glm::vec4(1.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {ZNEG, glm::vec4(1.f BLK_UVX, 14.f BLK_UVY, 0, 0)}
        }},

    {MASHROOMSTEM, std::unordered_map<Direction, glm::vec4, EnumHash>{
            {XPOS, glm::vec4(13.f BLK_UVX, 7.f BLK_UVY, 0, 0)},
            {XNEG, glm::vec4(13.f BLK_UVX, 7.f BLK_UVY, 0, 0)},
            {YPOS, glm::vec4(6.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {YNEG, glm::vec4(6.f BLK_UVX, 14.f BLK_UVY, 0, 0)},
            {ZPOS, glm::vec4(13.f BLK_UVX, 7.f BLK_UVY, 0, 0)},
            {ZNEG, glm::vec4(13.f BLK_UVX, 7.f BLK_UVY, 0, 0)}}},

    {MASHROOMLEAF, std::unordered_map<Direction, glm::vec4, EnumHash>{
            {XPOS, glm::vec4(13.f BLK_UVX, 8.f BLK_UVY, 0, 0)},
            {XNEG, glm::vec4(13.f BLK_UVX, 8.f BLK_UVY, 0, 0)},
            {YPOS, glm::vec4(13.f BLK_UVX, 8.f BLK_UVY, 0, 0)},
            {YNEG, glm::vec4(13.f BLK_UVX, 8.f BLK_UVY, 0, 0)},
            {ZPOS, glm::vec4(13.f BLK_UVX, 8.f BLK_UVY, 0, 0)},
            {ZNEG, glm::vec4(13.f BLK_UVX, 8.f BLK_UVY, 0, 0)}}},

    {BIRCH, std::unordered_map<Direction, glm::vec4, EnumHash>{
            {XPOS, glm::vec4(5.f BLK_UVX, 8.f BLK_UVY, 0, 0)},
            {XNEG, glm::vec4(5.f BLK_UVX, 8.f BLK_UVY, 0, 0)},
            {YPOS, glm::vec4(5.f BLK_UVX, 13.f BLK_UVY, 0, 0)},
            {YNEG, glm::vec4(5.f BLK_UVX, 13.f BLK_UVY, 0, 0)},
            {ZPOS, glm::vec4(5.f BLK_UVX, 8.f BLK_UVY, 0, 0)},
            {ZNEG, glm::vec4(5.f BLK_UVX, 8.f BLK_UVY, 0, 0)}}},

    {BIRCHLEAF, std::unordered_map<Direction, glm::vec4, EnumHash>{
            {XPOS, glm::vec4(4.f BLK_UVX, 12.f BLK_UVY, 0, 0)},
            {XNEG, glm::vec4(4.f BLK_UVX, 12.f BLK_UVY, 0, 0)},
            {YPOS, glm::vec4(4.f BLK_UVX, 12.f BLK_UVY, 0, 0)},
            {YNEG, glm::vec4(4.f BLK_UVX, 12.f BLK_UVY, 0, 0)},
            {ZPOS, glm::vec4(4.f BLK_UVX, 12.f BLK_UVY, 0, 0)},
            {ZNEG, glm::vec4(4.f BLK_UVX, 12.f BLK_UVY, 0, 0)}}},

    {DARKWOOD, std::unordered_map<Direction, glm::vec4, EnumHash>{
            {XPOS, glm::vec4(4.f BLK_UVX, 8.f BLK_UVY, 0, 0)},
            {XNEG, glm::vec4(4.f BLK_UVX, 8.f BLK_UVY, 0, 0)},
            {YPOS, glm::vec4(4.f BLK_UVX, 7.f BLK_UVY, 0, 0)},
            {YNEG, glm::vec4(4.f BLK_UVX, 7.f BLK_UVY, 0, 0)},
            {ZPOS, glm::vec4(4.f BLK_UVX, 8.f BLK_UVY, 0, 0)},
            {ZNEG, glm::vec4(4.f BLK_UVX, 8.f BLK_UVY, 0, 0)}}},

    {CACTUS, std::unordered_map<Direction, glm::vec4, EnumHash>{
            {XPOS, glm::vec4(6.f BLK_UVX, 11.f BLK_UVY, 0, 0)},
            {XNEG, glm::vec4(6.f BLK_UVX, 11.f BLK_UVY, 0, 0)},
            {YPOS, glm::vec4(5.f BLK_UVX, 11.f BLK_UVY, 0, 0)},
            {YNEG, glm::vec4(5.f BLK_UVX, 11.f BLK_UVY, 0, 0)},
            {ZPOS, glm::vec4(6.f BLK_UVX, 11.f BLK_UVY, 0, 0)},
            {ZNEG, glm::vec4(6.f BLK_UVX, 11.f BLK_UVY, 0, 0)}}},

    {SNOWLEAF, std::unordered_map<Direction, glm::vec4, EnumHash>{
            {XPOS, glm::vec4(5.f BLK_UVX, 12.f BLK_UVY, 0, 0)},
            {XNEG, glm::vec4(5.f BLK_UVX, 12.f BLK_UVY, 0, 0)},
            {YPOS, glm::vec4(2.f BLK_UVX, 11.f BLK_UVY, 0, 0)},
            {YNEG, glm::vec4(5.f BLK_UVX, 12.f BLK_UVY, 0, 0)},
            {ZPOS, glm::vec4(5.f BLK_UVX, 12.f BLK_UVY, 0, 0)},
            {ZNEG, glm::vec4(5.f BLK_UVX, 12.f BLK_UVY, 0, 0)}}},


};

// One Chunk is a 16 x 256 x 16 section of the world,
// containing all the Minecraft blocks in that area.
// We divide the world into Chunks in order to make
// recomputing its VBO data faster by not having to
// render all the world at once, while also not having
// to render the world block by block.

// TODO have Chunk inherit from Drawable
class Chunk : public Drawable{
private:
    // All of the blocks contained within this Chunk
    std::array<BlockType, 65536> m_blocks;
    // This Chunk's four neighbors to the north, south, east, and west
    // The third input to this map just lets us use a Direction as
    // a key for this map.
    // These allow us to properly determine

public:
    std::unordered_map<Direction, Chunk*, EnumHash> m_neighbors;
    ChunkVBOData chunkVBOData;
    glm::ivec2 m_pos;
    bool hasSetBlocks;
    bool snow;
    bool rain;
    Chunk(OpenGLContext* context, glm::ivec2 pos);
    ~Chunk();
    void createVBOdata() override;
    void loadVBOdata();
    GLenum drawMode() override;
    BlockType getBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getBlockAt(int x, int y, int z) const;
    void setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t);
    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);

    void drawCactus(int x, int z, int height, int tallness);
    void drawTree(int x, int z, int height);
    void drawMashRoom(int x, int z, int height);
};
