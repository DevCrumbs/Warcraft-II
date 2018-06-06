# Óscar Hernández
## Individual Contribution

- **Read map logic from xml:**
We have used Tiled to create the maps, the logic of the maps are also saved in the xml files. 
I have developed functions and methods to read the map logic, ally and enemy entities spawns, buildings and rooms.

The enemy dynamic entities are loaded in groups.

- **Map / fow  drawing optimization:**
One of the biggest problems we encountered was the performance. I have worked on the optimization of the map and the FoW. Using the optimization process the map draw function consumes Xms, Yms less.
The FoW uses Xms with the optimization and Yms more without.

- **Minimap:**
I have implemented my minimap project to the game.  Various extra features like a zoom toggle, moving the camera from the minimap and sending the selected units to a point selected on the minimap.


- **Enemy wave:**
I have worked on the Enemy Wave module, creating the functions to decide how many enemies will appear in the wave.

- **Smooth camera movement:**
I have improved the camera movement, making the speed of the camera inversely proportional to the distance of the mouse to the border of the screen.

-**FoW:**

I have included the fog of war to the game. The FoW has 3 levels, unit sight, explored, unexplored.

- **Save and load:**
I have included a Save/Load functionality to the game with the help of Aleix. We save the current state of the game so the player can resume the game later at the point he saved the game.

- **Keys redefinition**
With the help of Aleix, I have developed a system so the player can change the controls of the game and restore the default controls.
