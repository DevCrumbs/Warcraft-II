# Óscar Hernández
## Individual Contribution

- **Random map:** <br>
I have developed a module to generate a semi-randomly generated maps. This module can generate this maps using xml files. It uses one file as a general map structure, to decide the shape of the map. Then the module decides which room to load from the given pool in the structure file. 

<img src="Readme_Files/ÓscarHernández/mapCode.png"><br>
	_Random map code_<br><br>	

<img src="Readme_Files/ÓscarHernández/randomMap.jpg"><br>
	_Random map example_<br><br>	
  
- **Pathfinding:**<br>
In order to adapt the A* algorithms to the rooms-based map I improved the pathmanager module. The module launch a high-level pathfinding (using the general map structure from the random map generator), deciding which rooms the unit will need to go through. Once the next room the unit has to go is decided, a low-level path is launched to from the unit’s current location to the next room entrance. This process is repeated until the unit gets to the desired location.

<img src="Readme_Files/ÓscarHernández/path.gif"><br>
	_Pathfinding_<br><br>	

- **Read map logic from xml:**<br>
We have used Tiled to create the maps, the logic of the maps are also saved in the xml files. 
I have developed functions and methods to read the map logic, ally and enemy entities spawns, buildings and rooms.
The enemy dynamic entities are loaded in groups.

<img src="Readme_Files/ÓscarHernández/mapLogic.png"><br>
	_Map logic code_<br><br>	

- **Fog of War:**<br>
I included the fog of war to the game. The FoW has 3 levels, unit sight, explored, unexplored.

<img src="Readme_Files/ÓscarHernández/fow.gif"><br>
	_Fog of War_<br><br>	
	
- **Map / fow  drawing optimization:**<br>
One of the biggest problems we encountered was the performance. I have worked on the optimization of the map and the FoW. Using the optimization process the map draw function consumes 0.4ms, 25ms less.
The FoW uses arround 0.5ms with the optimization and 168.3ms without.

<img src="Readme_Files/ÓscarHernández/noOptimizationFowMap.png"><br>
	_Frame time without optimization_<br><br>	
	
<img src="Readme_Files/ÓscarHernández/optimizationFowMap.png"><br>
	_Frame time with optimization_<br><br>	
	
- **Minimap:**<br>
I have implemented my minimap project to the game.  Various extra features like a zoom toggle, moving the camera from the minimap and sending the selected units to a point selected on the minimap.

<img src="Readme_Files/ÓscarHernández/minimap.gif"><br>
	_Minimap_<br><br>	
	
- **Enemy wave:**<br>
I have worked on the Enemy Wave module, creating the functions to decide how many enemies will appear in the wave.

- **Smooth camera movement:**<br>
I have improved the camera movement, making the speed of the camera inversely proportional to the distance of the mouse to the border of the screen.

<img src="Readme_Files/ÓscarHernández/camera.gif"><br>
	_Camera movement_<br><br>	
	
- **Save and load:**<br>
I have included a Save/Load functionality to the game with the help of Aleix. We save the current state of the game so the player can resume the game later at the point he saved the game.

<img src="Readme_Files/ÓscarHernández/save-load.gif"><br>
	_Save and load_<br><br>	
	
- **Keys redefinition:**<br>
With the help of Aleix, I have developed a system so the player can change the controls of the game and restore the default controls.

<img src="Readme_Files/ÓscarHernández/keys.gif"><br>
	_Keys redefinition_<br><br>	
### Wiki tasks:
- TDD (with Sandra Alvarez).
