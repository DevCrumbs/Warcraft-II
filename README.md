### Welcome, travellers!

# Warcraft II: The Stolen Artifacts

Warcraft II: The Stolen Artifacts is an expansion that derives from the videogame Warcraft II: Tides of Darkness. 
The game is set after the events of Tides of Darkness, in which a small group of the Azeroth's army must expedition to a now destroyed Draenor 
to find the legendary alliance expeditioners that became trapped in Draenor, and defeat the evil Ner’zhul, 
by retrieving one of the four artifacts that he stole from Azeroth.

The player must command this expedition in an RTS dungeon crawler game in which they must recover the remaints of that legendary expedition in order to defeat Ner’zhul. 
Every time the expedition enters Draenor the construction of the land, and the path they will have to take will be different, 
so the game will be different every run the player makes.

* **GitHub repository**: 
[DevCrumbs](https://github.com/DevCrumbs/Warcraft-II)
* **Wiki**: 
[DevCrumbs wiki homepage](https://github.com/DevCrumbs/Warcraft-II/wiki)

# The team: DevCrumbs

DevCrumbs is an independent video game company based in Terrassa, Catalonia. Born from a university project, it is formed by a 
group of students from the second year of the Bachelor's degree in Video Game Design and Development (UPC). 
Although each member of the team has been involved in similar projects before, we are currently working on our first game together. 
The overall aim of the company is to create enjoyable gaming experiences, a unique kind of work that we are passionate about. 
If we are able to feed your imagination, our mission will be accomplished!

# Version 0.5

The goal of the player in this demo version is to rescue two prisoners - Khadgar and Alleria. The prisoners are hidden somewhere in a dungeon full of enemies. 
In order to complete this objective, the player will have to create troops and command them towards the victory.

# Controls

Space: go to player base

P: set command patrol to the units selected (click a tile to patrol there)
S: stop the units selected

For the units:
Mouse Left click: select a group of units
Mouse Right click: send a group of units to a destination
Mouse Right click on an enemy or a critter: attack the enemy/critter
Mouse Right click on a Gold Mine or Runestone: (if there is a unit nearby) collect the gold / heal the units.

For the buildings:
After opening the building dropdown in the top right of the screen:
Mouse Left click: Place a building somwhere.
Mouse Right click: Delete the preview building from the view of the player.

# Game system

To create buildings or units the player must get gold and food. Every unit costs a unit of food and every unit and
building has a set number of gold cost.

- To get gold the player can get it in a gold mine or by killing enemies.

- To get more food, the player can build chicken farms. Every chicken farm gives the player one unit of food.

- The player can also heal themselves if they go to a runestone and click it.

# About the units

If the player clicks the barracks, and they have enough gold or food, the player creates units.
The units that they can create are:

Footman: 500 gold
The footman is a melee attack unit.

Elven Archer: 400 gold
The elven archer is a long range attack unit.

The units can move towards a tile or a target to attack it or patrol the area.

Unit specific buttons:
- Patrol: To patrol the area.
- Stop: to stop the unit on its tracks.

The enemy units are Grunts and Troll Axethrowers.

# About the buildings

When the player selects a building in the buildings dropdown, a preview of the building will be shown where
the player's mouse is. If the building can be placed where the mouse is, it will be indicated with green tiles, if not
it will be indicated with red tiles. The player can only place a building in the player's base
and where there are walkable tiles in the map, and where there's not another unit or building in the map

The player can build:

Chicken farms: 500 gold
They give the player 4 units of food.

Scout Tower: 400 gold
Guard Tower: 600 gold
Cannon Tower: 800 gold
They attack enemy entities and enemy buildings.

There is a Town Hall and Barracks which the player cannot build.
The barracks can spawn Footmans and Elven Archers if the player has enough gold.

There are many enemy buildings scattered throughout the map. Be careful because there are several orcish towers that 
can attack you!

# Mini map

There's a mini map in the top left side of the screen that shows:
- Enemy troops (red)
- Enemy buildings (dark blue)
- Allied troops (liight blue)
- Allied buildings (dark blue)
- Gold mines and Runestones (yellow)

# Debug controls

F8: debug mode

If debug mode is on:

	C: -20 HP to last Chicken Farm created
	G: get + 500 gold

	1: Spawn a Footman at mouse position
	2: Spawn an Elven Archer at mouse position
	3: Spawn a Gryphon Rider at mouse position

	4: Spawn a Grunt at mouse position
	5: Spawn a Troll Axethrower at mouse position
	6: Spawn a Dragon at mouse position

	7: Spawn a sheep at mouse position
	8: Spawn a boar at mouse position

	W: direct win
	L: direct lose

# Tools used

- IDE: Microsoft Visual Studio 2017 (C++)
- SDL 2.0, pugixml 1.8
- Level edition: Tiled
- Profiler: Brofiler
- Graphics edition: Adobe Photoshop, Microsoft Paint

# Art used

Some of the art used is from Warcraft II™, that is a Blizzard property. We don't own all the art of the game.
We only own the main menu, king Terenas, Kadghar and Alleria art, that are made by Manav Lakhwani and David Varela.

## Sources

All the art of the original Warcraft II that we used is all found in this web page:

https://www.spriters-resource.com/pc_computer/warcraft2/

All the music and sounds of the original Warcraft II we used os aññ found in this web page:

https://www.sounds-resource.com/pc_computer/warcraft2/sound/352/

## About us

In order to cover all the areas needed to fulfill the project, each of us has a different responsability in the team. 
Despite that, we all will be working in those areas which request more dedication and helping each other to achieve the best results on time.

**Lead**: Sandra Alvarez
* **GitHub account**: [Sandruski](https://github.com/Sandruski)

**Management**: Manav Lakhwani
* **GitHub account**: [manavld](https://github.com/manavld)

**Art/Audio**: David Varela
* **GitHub account**: [lFreecss](https://github.com/lFreecss)

**Code**: Óscar Hernández
* **GitHub account**: [OscarHernandezG](https://github.com/OscarHernandezG)

**Design**: Joan Valiente
* **GitHub account**: [JoanValiente](https://github.com/JoanValiente)

**QA**: David Valdivia
* **GitHub account**: [ValdiviaDev](https://github.com/ValdiviaDev)

**UI**: Aleix Gabarró
* **GitHub account**: [aleixgab](https://github.com/aleixgab)
