
# pcisph
Predictive-Corrective Incompressible SPH paper implementation rendered using Screen Space Fluid Rendering with Curvature Flow



//==========================================
// Build -- 2 choices
//==========================================

You will need to change some paths though to be your computers paths.
These are in util/def.h (change base_directory), and you also need to update the file paths of the objs from the scene files (scenes/test#.json).

1. Use cmake:

a: 'cd' in to the root directory
b: mkdir build
c: cd build
d: cmake ..
e: make -j4 (or -j8 if you have 8 cores)
f: ./fluid_simulator


2. Use Qt (actually still using cmake)

a. open cmakelist.txt in Qt.
b. specify the cmake on your computer. (type 'which cmake' in temrinal and you can see the path of your cmake).
c. click build



//==========================================
// Controls:
//==========================================

wasd and mouse to move and look around.
0 to switch to rendering just particles (but make sure you are not in prerender mode)
p to toggle prerender mode. It starts on, and it will run the simulation but only render every 1/60 seconds. Then when you press ENTER it will play back what was rendered in real time. if it is off, then it will just renderevery frame and you can't play it back

123456 change what is rendered
1:toggles noise
2:toggles refraction
3:reflection
4:specular
5:render normals
6:do curvature flow

tab cycles through some intermediate textures
full rendering -> depth -> thickness -> noise -> smoothed thickness -> smoothed depthmap -> back to the very beginning

space pauses and starts

zxcvb load in the 5 different scenes



//==========================================
// Code breakdown.
//==========================================

Sam:
Mostly in shading, specifically SSFRenderer and most of the things that uses (shaders folder, and a bunch of supporting material). The other members did most of the initial particle rendering while they were doing the simulation to get an initial visual and I extended it using the screen space fluid rendering paper.

Zihao:
Mostly in the algorithm folder.

Dixuan:
Mostly in the visualization folder and scenes: creates visualization data structures like voxelgrid, mesh and scene. Generates particles from boxes, spheres and meshes.
Do initial rendering of particles with Zihao.

All:
There was a lot of other code that we had to use together to get everything to work together (engine, utils, gui).

