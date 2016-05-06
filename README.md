# pcisph
Predictive-Corrective Incompressible SPH paper implementation rendered using Screen Space Fluid Rendering with Curvature Flow

We used CMake to build the project, if you open CMakeList.txt in Qt it should work. You will need to change some paths though to be your computers paths. These are in util/def.h (change base_directory), and you also need to update the file paths of the objs from the scene files (scenes/test#.json).


Controls:
wasd and mouse to move and look around
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






Code breakdown.

Sam:
Mostly in shading, specifically SSFRenderer and most of the things that uses (shaders folder, and a bunch of supporting material). The other members did most of the initial particle rendering while they were doing the simulation to get an initial visual and I extended it using the screen space fluid rendering paper.


Zihao:
Mostly in the algorithm folder.

Dixuan:
Mostly in the visualization folder and scenes.

All:
There was a lot of other code that we had to use together to get everything to work together (engine, utils, gui).