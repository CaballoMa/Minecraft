# Minecraft
Developed by cpp &amp; OpenGL

### Player Physics

In this part, I implemented the player's tick function, which include ```processInputs``` and ```computePhysics``` functions. For ```processInputs```, I associate each input keys with  accelerations on each direction, which is fairly easy using Qt's input system. Then for ```computePhysics```, I have fly version and non-fly version. Player's velocity is computed using 

```
m_velocity += glm::normalize(m_forward)    * m_acceleration.z * dT
               + glm::normalize(m_right)   * m_acceleration.x * dT
               + glm::normalize(m_up)      * m_acceleration.y * dT;
``` 
When player isnt flying, we ignore the up velocity, and then also add gravity to it. For collision detection, I used the ```gridMarch``` function given on the slide, and check whether any one from the 12 vertices intersect with a block, and then stop that direction movement. One difficulty here is to correctly detect the min of all vertices, and another is to correctly implement the jump function so that the player can't do infinite jumping.

I also implemented delete and create block function, which is done in the moveclick event.Everytime the mouse click, I do a raycast from the camera and check whether it hit something within 3 units. If it does, set that block's type to Empty.

### Efficient Terrain Rendering and Chunking
#### CHUNK:
1. A structure is created that stores vertex positions, and another that stores directions, offsets, and vertices that form a chunk neighbor's face.
2. Make Chunk inherits from Drawable, implements the create() function which iterates over the x, y, z coordinates of the chunk and iterates over the coordinates of the 6 neighboring blocks to check if they are empty. The VBO data is added for each face of each empty neighboring block if the neighboring block is empty.
3. The loadVBO() function is implemented to receive the interleaved vertex data vector as well as the index data vector , and buffer them into VBOs.

#### TERRAIN:
1. Terrain is now generated from a 4x4 Chunk to generate a zone unit. The terrain expands as the player approaches 16 blocks from the edge of a Chunk that is not connected to an existing Chunk.
2. Changed the draw() function when the Chunk inherits from Drawable, so it draws each Chunk with the given ShaderProgram; sets the model matrix to the appropriate X and Z transformations.

#### SHADERPROGRAM:
1. Implemented a second drawing function drawInterleave() for the ShaderProgram class, to render a Drawable with an interleaved VBO set.

#### DIFFICULTY:
After I changed the rendering of each block to rendering each Chunk, the position of the scene object was always wrong, and I checked the various data in Chunk.cpp for a long time without finding any problem. Finally I found out that the matrix parameter was not passed in the instance's shader, and the position was correct after using Lambert's shader instead lol.

### Texture and Texture Animation

I finished the texture pipeline, and also some simple texture animation on water and lava. I also implemented opaque and transparent rendering so that after enabling GL_BLEND all textures are rendered correctly.

### Muti-Thread

I implemented a multi-threaded chunked rendering. First, I locate the player's zone by the current player position, then I compare the player's zone in the previous frame with the zone in this frame to find the chunks that do not need to be rendered, and destroy the vbo data of these chunks. After that, I decide if I need to create chunks for this zone by determining if the zone was created before, and then store these chunks in the newChunkMap if required. By iterating through the newChunkMap, I create a blockTypeWorker for each new chunk and create an additional thread to initialize the block data in the chunk. After initializing the chunk, store it in the blockTypeMap. Iterate through the blockTypeMap, creating a VBOWorker for each chunk and creating an extra thread to initialize the chunk's VBO data. Finally, after loading the VBO data, the chunk binds the VBO data to the GPU uniformly. In addition, a thread mutex is needed for each Map operation.

The implementation of this part can be optimized and improved: In blocktypeworker, each time before initializing the block data of a chunk, you can determine whether the chunk needs to be rendered in the current frame.If the chunk does not need to be rendered, the chunk should be skipped. In VBOworker, it is also necessary to determine whether the chunk needs to be rendered in the current frame and do the same. The premise is to determine directly which chunks need to be rendered based on the player's location and store those chunks in the container. (my approach is to render chunks by determining the zone)


### Shadow Mapping

This is a very classic method to generating shadows in games. In this project I only implemented a very basic SM, but planning to add PCSS in the future.

Difficulty:
I found it a little difficult to find a correct bias and near/far plan number, so it takes me a long time to adjust in order to create better shadows.

![](screenshot1.gif)

### Weahter, Procedurally placed assets, Water waves, Post-process Camera Overlay, Distance fog

#### Weahter:

I implemented two different weather systems: rain and snow (triggered by the K and L keys in the demo): A certain number of particles are initialized depending on the player's position when the weather is created at the beginning. The speed of the snowflakes is assigned by the noise function and the time. The rain is moving vertically downward, and when it falls to the ground it will return to the initialized position and fall in a cycle. This allows the rain particles to be reused to reduce the performance consumption.

![](screenshot6.gif)

#### Procedurally placed assets:

I implemented functions to draw different plants in chunk and called these functions when initializing blocks: Different plants are created by determining the current block type and the altitude of the block. The noise function is used to make the generation of plants more random and reasonable.

![](screenshot2.gif)

#### Water waves:

I change the position of the water wave vertex Y by the sin function. To prevent the water waves from looking neatly waved, I pass the UV value of the vertex into the noise function as an argument to get a messy undulation of the water waves. In addition, I recalculate the normals based on the Y values of the water wave vertices. Then, I implemented the water reflection model of blinn phong.

![](screenshot3.gif)

#### Post-process Camera Overlay:

1. For Underwater, I change the color of original vertex to blue and applied the sine function to implement the visual effect. (The same as ShaderFun project)
2. For UnderLava, I change the color of orignial vertex to red and increase the color depth by time through. Additionally, I make the visual effect more realistic by Berlin noise function.

##### **Water:**

![](screenshot4.gif)

##### **Lava:**

![](screenshot5.gif)

#### Distance fog:

Pass in the current position of the player through the shaderprogram. Linear interpolation is performed between the original color of each pixel and the color of the fog, depending on the distance of the object from the player.

![](screenshot7.png)