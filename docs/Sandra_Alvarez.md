# Sandra Alvarez
## Individual Contribution

- **Adapt the Pathfinding module to work with time-sliced Pathfinding:** implement a “cycle-once” method for the A* and Dijkstra algorithms.
- **PathManager module and PathPlanner class:** while the PathManager module calls the cycle-once methods of all the searches registered, the PathPlanner class contains the search itself and other information about it.
- **Improve my Group Movement module:** Group Movement is a research of mine in moving troops through complex terrains (without using the Boids algorithm). For more information, check it here: [RTS Group Movement](https://sandruski.github.io/RTS-Group-Movement/)
	- Draw formations at destination to organize the units.
- **Improve the Collision module:**
	- Allow groups of colliders (colliders made of more than one collider).
	- Hierarchical collision detection between two levels of collisions (high-level collision detection, which is for colliders, and low-level collision detection, which is for complex collider groups).
	- Triggers.
- **Goal-Driven Agent Behavior:** implement a Goal module, which is divided into the Atomic Goal class and the Composite Goal class. With this module, I implemented most of the behavior of the units and their interaction with the elements of the map. It allowed the units to remember the actions that they were doing before started doing something else and, later, be able to decide whether or not to resume their previous actions.
- **Establish the EntityFactory hierarchy:** separate the entities in Dynamic and Static (both of them inherit from a base Entity class) and prepare the base classes for all the entities that would be in the game.
- **Implement all units** (player units: Footman, Elven Archer and Gryphon Rider; enemy units: Grunt, Troll Axethrower and Dragon; prisoners (kind of player units)**:** Alleria and Turalyon; neutral units (critters): sheep and boar).
	- **Implement the distinctive features of the units:** for example, Gryphon Rider and Dragon units ignore the walkability of the map.
	- **Implement the particles left by some of the units:** Sheep and Boar units leave their paws drawn on the ground as they move, Elven Archer and Troll Axethrower throw arrows and axes, respectively, and Gryphon Rider and Dragon throw fire.
	- Implement the healing particles.
	- Adjust all the animations and make sure they are drawn correctly.
	- Implement a pleasant units’ selection and offset when clicking on a unit.
- **Implement a command system for the player units:** the command system allows the player to order their units to perform certain actions, which are:
	- Rescue of prisoners (with Goals).
	- Gathering of gold in gold mines (with Goals).
	- Healing a group of units with runestones (with Goals).
	- Patrol and Stop commands.
- **Attack:**
	- **Multiple attack between units** (player troops versus enemy troops)**:** units are always recalculating the best opposing target to attack. This avoids all units focus on the same opposing target, thus improves the strategy of the combat.
	- **Units attack buildings:** units find the best spot at the surrounds of a building and distribute themselves in order to attack it, which improves the strategy of the combat.
	- **Units attack critters:** critters restore units’ health when killed.
- **Units AI:**
	- **Player units AI:** 
		- Default state: goal Look Around. If an enemy unit is seen by a player unit, the player unit will automatically attack it (without the need of the player to give the order for them to do it).
		- Defend state: if a player unit is being attacked, the player unit will automatically attack back.
		- Attack state: if the current target of a player unit is dead, the player unit will automatically attack the next enemy unit.
	- **Enemy units AI:** 
		- Different AI for enemy units from any room of the map and enemy units from waves (which attack the player base).
		- Default state: for an enemy unit from a room, mix of the goals Wander and Look Around. For an enemy unit from a wave, goal Attack the Town Hall of the player.
		- Defend state: if an enemy unit is being attacked, the enemy unit will automatically attack back.
		- Runaway state: if an enemy unit has low life, they will stop attacking (if they were attacking) and suddenly enter the Heal state.
		- Heal state: if an enemy unit has low life, they will search for a critter in order to kill it and restore some life.
		- Attack state: if the current target of an enemy unit is dead, the enemy unit will automatically attack the next player unit. For an enemy unit from a wave, if no player unit is seen by the enemy unit, the enemy unit will proceed to attack the buildings of the player.
	- **Neutral units (critters) AI:**
		- Default state: mix of the goals Wander and Look Around.
- **Units distribution when created:** make sure that units never appear on top of each other by using a BFS search algorithm every time a new unit is spawned.
- **Update the walkability of the map appropriately when a building is built/destroyed.**
	- Recalculate the affected paths of the units if necessary.
- **Implement the flow of the Enemy Waves at player base throughout the game.**
- **Allow the player to save groups and select saved groups:** the player can have a maximum of three groups of units saved.
- **Beautify the Fog of War:** prepare the sprites of the tiles with round corners and decide which have to be drawn in every case.

### Other tasks:
- Provide base code adapted to STL.
- Improve the Printer module (this module orders objects by layer and draws them accordingly) of a classmate: add circles and lines (for debug purposes) and priority to quads, circles and lines.
- Create simple methods to allow the game designer/s modify the parameters of the game in an easy and fast way.
- Fixe the majority of the most critical bugs of the game (game crush bugs) alongst its development.
- Write some of the messages said by King Terenas.
- Add feedback to specific features of the game: when a room is cleared or the player base is successfully defended (brighten the floor and print the information in the middle of the screen), when a player building or unit is selected (draw a quad around it), when an enemy building or unit is attacked (draw a red quad around it for just a few seconds), when a new group of units is saved or a saved group of units is selected, etc.
- Select all units of a certain type which are on screen, center the camera to a specific group of units and allow the player to easily add or remove units from the selected units.

### Wiki tasks:
- Home page.
- _The original game: Warcraft II_ page.
- _From Warcraft II to our game_ page, the sections _What are players asking for?_ and _The recipe_.
- Plot of our game.
- Half of the GDD (in terms of content): expand on each concept and describe it with more precision. Plus, the sections _Plot_, _Game Flow_, _Objectives_, _Fog of War_, _Learn to Play_ and _Controls_.
- UML (from the Tech Design Document).


NOTE: the PathManager module, PathPlanner class and Goal module were learned from the book _Programming Game AI by Example_, by Mat Buckland.

Finally,
As a programmer, I helped as much as I could other members with their code issues.
As a lead, I tried to maintain the group united and motivated until the end of the project:)