#pragma once
#include "smartpointerhelp.h"
#include "glm_includes.h"
#include "chunk.h"
#include <array>
#include <unordered_map>
#include <unordered_set>
#include "shaderprogram.h"
#include "cube.h"
#include <cmath>
#include <thread>
#include <mutex>
#include <math.h>
#include <QList>
#include <set>

#include "noise.h"
#include "lsystem.h"
#include "snow.h"
#include "rain.h"

//using namespace std;

// Helper functions to convert (x, z) to and from hash map key
int64_t toKey(int x, int z);
glm::ivec2 toCoords(int64_t k);
int64_t toKey(int x, int y, int z);

// The container class for all of the Chunks in the game.
// Ultimately, while Terrain will always store all Chunks,
// not all Chunks will be drawn at any given time as the world
// expands.
class Terrain {
private:
    // Stores every Chunk according to the location of its lower-left corner
    // in world space.
    // We combine the X and Z coordinates of the Chunk's corner into one 64-bit int
    // so that we can use them as a key for the map, as objects like std::pairs or
    // glm::ivec2s are not hashable by default, so they cannot be used as keys.
    std::unordered_map<int64_t, uPtr<Chunk>> m_chunks;

    // We will designate every 64 x 64 area of the world's x-z plane
    // as one "terrain generation zone". Every time the player moves
    // near a portion of the world that has not yet been generated
    // (i.e. its lower-left coordinates are not in this set), a new
    // 4 x 4 collection of Chunks is created to represent that area
    // of the world.
    // The world that exists when the base code is run consists of exactly
    // one 64 x 64 area with its lower-left corner at (0, 0).
    // When milestone 1 has been implemented, the Player can move around the
    // world to add more "terrain generation zone" IDs to this set.
    // While only the 3 x 3 collection of terrain generation zones
    // surrounding the Player should be rendered, the Chunks
    // in the Terrain will never be deleted until the program is terminated.
    std::unordered_set<int64_t> m_generatedTerrain;

    // TODO: DELETE ALL REFERENCES TO m_geomCube AS YOU WILL NOT USE
    // IT IN YOUR FINAL PROGRAM!
    // The instance of a unit cube we can use to render any cube.
    // Presently, Terrain::draw renders one instance of this cube
    // for every non-EMPTY block within its Chunks. This is horribly
    // inefficient, and will cause your game to run very slowly until
    // milestone 1's Chunk VBO setup is completed.
    //Cube m_geomCube;

    OpenGLContext* mp_context;

    //std::unordered_set<glm::vec2> water_block;
    //std::unordered_set<glm::vec3> empty_block;
    std::map<int64_t, glm::vec3> lsystem_water_block;
    std::map<int64_t, glm::vec3> lsystem_empty_block;

public:

    int  xStart, xEnd, zStart, zEnd;

    LSystem* lsystem;
    LSystem* lsystem1;

    Terrain(OpenGLContext *context);
    ~Terrain();

    // Instantiates a new Chunk and stores it in
    // our chunk map at the given coordinates.
    // Returns a pointer to the created Chunk.
    Chunk* instantiateChunkAt(int x, int z);
    // Do these world-space coordinates lie within
    // a Chunk that exists?
    bool hasChunkAt(int x, int z) const;
    // Assuming a Chunk exists at these coords,
    // return a mutable reference to it
    uPtr<Chunk>& getChunkAt(int x, int z);
    // Assuming a Chunk exists at these coords,
    // return a const reference to it
    const uPtr<Chunk>& getChunkAt(int x, int z) const;
    // Given a world-space coordinate (which may have negative
    // values) return the block stored at that point in space.
    BlockType getBlockAt(int x, int y, int z) const;
    BlockType getBlockAt(glm::vec3 p) const;
    // Given a world-space coordinate (which may have negative
    // values) set the block at that point in space to the
    // given type.
    void setBlockAt(int x, int y, int z, BlockType t);

    // Draws every Chunk that falls within the bounding box
    // described by the min and max coords, using the provided
    // ShaderProgram
    void draw(int minX, int maxX, int minZ, int maxZ, ShaderProgram *shaderProgram);

    // Initializes the Chunks that store the 64 x 256 x 64 block scene you
    // see when the base code is run.
    void CreateTestScene();

    // Cave System
    void drawCave1(int x, int y, int z);
    void drawCave(int worldX, int worldY, int worldZ, int localX, int localY, int localZ, Chunk* chunk);

    // L-System to draw river
    void initializeLSystem(LSystem* l_system);
    void drawRoute(Turtle startTurtle, Turtle nextTurtle);

    void CreatChunkBlocks(Chunk* chunk);
    void endThreads();
    std::vector<uPtr<Chunk>> getNeigbhorChunks(uPtr<Chunk> chunk);
    // Generate Terrain Stuff (Noise function)
    void terrainCheck(glm::vec3 playerPos, glm::vec3 prev_playerPos);
    bool hasZoneAt(int x, int z) const;
    std::vector<glm::ivec2> getNeighborAreas(int x, int z, int r);

    std::vector<std::thread> BlockTypeWorkers;
    std::vector<std::thread> VBOWorkers;

    void BlockTypeWorker(uPtr<Chunk> chunk);
    void VBOWorker(uPtr<Chunk> chunk);

    std::unordered_map<int64_t, uPtr<Chunk>> newChunks;
    std::unordered_map<int64_t, uPtr<Chunk>> BlockTypesMap;
    std::mutex BlockTypeMutex;
    std::unordered_map<int64_t, uPtr<Chunk>> VBOdataMap;
    std::mutex VBOdataMutex;
    std::vector<glm::ivec2> newZones;
    std::vector<glm::ivec2> prevZones;
    Snow m_geomSnow;
    Rain m_geomRain;

    bool generateSnow;
    bool snowSmallScale;
    bool snowMidScale;

    bool hasNewChunkAt(int x, int z) const;

    uPtr<Chunk>& getNewChunkAt(int x, int z);

    void draw_snow( ShaderProgram *shaderProgram);

    void create_snow(int minX, int maxX, int minZ, int maxZ, int playerPosY, int scale);
    void draw_rain( ShaderProgram *shaderProgram);

    void create_rain(int minX, int maxX, int minZ, int maxZ, int playerPosY);

    bool snow(int x, int y, int z) const;
    bool rain(int x, int y, int z) const;
};
