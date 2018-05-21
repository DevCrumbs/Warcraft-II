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

# Version 0.9

The goal of the player in this alpha version is to rescue two prisoners - Khadgar and Alleria. The prisoners are hidden somewhere in a dungeon full of enemies. 
In order to complete this objective, the player will have to create troops and command them towards the victory.

# Controls

Space: Go to player base
Up arrow or W: Move the camera up
Down arrow or S: Move the camera down
Left arrow or A: Move the camera left
Right arrow or D: Move the camera right

Z: Select all Footman on screen (max 8 units)
X: Select all Elven Archer on screen (max 8 units)
C: Select all Gryphon Rider on screen (max 8 units)
V: Select all units on screen (max 8 units)

Q: Move the camera to the units selected

Left/Right Ctrl + 1/2/3: Save a group of units to a slot
Left/Right Shift + 1/2/3: Select a saved group of units from a slot

## For the minimap:

TAB: Change minimap zoom
Mouse Left click: Move camera to the pressed location
Mouse Right click: Send a group of units to a destination

## For the units:

Mouse Left click: Select a group of units
Mouse Right click: Send a group of units to a destination
Mouse Right click On an enemy or a critter: attack the enemy/critter
Mouse Right click On a Gold Mine or Runestone: (if there is a unit nearby) collect the gold / heal the units

N: Set command patrol to the units selected (click a tile to patrol there)
M: Stop the units selected

## For the buildings:

B: Open the building dropdown
After opening the building dropdown in the top right of the screen:
Mouse Left click: Place a building somwhere
Mouse Right click: Delete the preview building from the view of the player

# Debug controls

F8: debug mode

If debug mode is on:

	H: -20 HP to last Chicken Farm created
	G: Get + 500 gold
	F: Get +3 food
	K: Show/hide Fog of War

	1: Spawn a Footman at mouse position
	2: Spawn an Elven Archer at mouse position
	3: Spawn a Gryphon Rider at mouse position
	4: Spawn a Grunt at mouse position
	5: Spawn a Troll Axethrower at mouse position
	6: Spawn a Dragon at mouse position
	7: Spawn a sheep at mouse position
	8: Spawn a boar at mouse position
	9: Spawn Alleria
	0: Spawn Turalyon

	F1: Direct win
	F2: Direct lose
	F3: Spawn a new enemy wave (in player base)
	F4: Activate or stop the spawn of waves
	F5: Show/hide game's colliders

# Game system

To create buildings or units the player must get gold and food. Every unit costs a unit of food and every unit and
building has a set number of gold cost.

- To get gold the player can get it in a gold mine or by killing enemies.

- To get more food, the player can build chicken farms. Every chicken farm gives the player one unit of food.

- The player can also heal themselves if they go to a runestone and click it.

## Win/lose condition

- The player will win the game if they are able to rescue the two prisioners in the map and the enemies haven't destroyed
  they Town Hall.

- The player will lose the game if they are out of units and they have no gold to produce more, or if their Town Hall in the
  base is destroyed.

# Levels

There are four levels for the player to choose in bettween. Two of them are easy, two of them have a medium difficulty
and the other one is hard. 

- The easy levels give the player more amout of gold in mines and have less intense attacks to the base.

- The normal levels have a normal amount of gold given to the player, and have more intense attacks to base.

- The hard level gives the player the same amount of gold as with the normal levels, but doesn't give them
  gold when killing enemys. Also, the attacks to base are the more intense out of the three difficulties.

# About the units

If the player clicks the barracks, and they have enough gold or food, the player creates units.
The units that they can create are:

Footman: 500 gold
The footman is a melee attack unit.

Elven Archer: 400 gold
The elven archer is a long range atack unit.

Gryphon Rider: 750 gold
The Gryphon rider is an aerial unit.
Gryphon Riders cannot rescue prisioners.

The units can move towards a tile or a target to attack it or patrol the area.

Unit specific buttons:
- Patrol: To patrol the area.
- Stop: to stop the unit on its tracks.

The enemy units are Grunts, Troll Axethrowers and Dragons, which are the counterpart to the three main enemy units.

# About the buildings

When the player selects a building in the buildings dropdown, a preview of the building will be shown where
the player's mouse is. If the building can be placed where the mouse is, it will be indicated with green tiles, if not
it will be indicated with red tiles. The player can only place a building in the player's base
and where there are walkable tiles in the map, and where there's not another unit or building in the map

The player can build:

Chicken farms: 500 gold
They give the player 4 units of food.

Barracks: 1000 gold
The barracks can spawn Footmans and Elven Archers if the player has enough gold.
(They can only be built if there aren't any Barracks on the map)

Gryphon Aviary: 400 gold
This allows the player to spawn Gryphon Riders if they have enough gold (it needs the Townhall upgrade to be built).
(They can only be built if there aren't any Gryphon Aviarys on the map)

Scout Tower: 400 gold
Guard Tower: 600 gold
Cannon Tower: 800 gold
They attack enemy units that are in base.

TOWN HALL:
The Town Hall is a special building that the player cannot build. It can be upgraded to a Keep that allows
the player to build a Gryphon Aviary. If the player

There are many enemy buildings scattered throughout the map. Be careful because there are several orcish towers that 
can attack you!

## Building buttons

Every building has a set of buttons when they are selected. 
Normal buildings like Chicken Farms and Towers will only have a destroy building button, that destroys the building,
in case the player has built the building in an inconvinient place.

The production buildings, being the Barracks and the Gryphon Aviary have the pertinent creation units buttons. They create the pertinent
unit that the player selects, if they have enough gold.

The Town Hall has an upgrade button, that allows the building to upgrade to a Keep.

## Room clear

If the player clears a room of enemies and enemy buildings, they will get a message conforming they
have cleared the room, and thay will be recompesned with a generous amount of gold.

# Mini map

There's a mini map in the top left side of the screen that shows:
- Enemy troops (red)
- Enemy buildings (dark blue)
- Allied troops (liight blue)
- Allied buildings (dark blue)
- Gold mines and Runestones (yellow)

# Enemy waves to base

Every 4-5 minutes, enemy troops will arrive to the player's base in a ship to destroy their buildings,
but mostly the townhall. Remember that if your townhall is destroyed YOU WILL LOSE THE GAME.
The number of ships and enemies that arrive to the base in every wave depends on the difficulty level that the player has
chosen, but there will never be more than three boats per wave and more than six enemies per boat.
When the wave is finished the player will be recompensed with gold.

# The artifacts

There are four artifacts present in the game. The player will get one artifact or another depending on how much time they
spend beating the level.
The four artifacts are:

- Scepter of Sagreras: Less than 18 minutes to complete the level.

- Eye of Dalaran: Less than 20 minutes to complete the level.

- Skull of Gul'dan: Less than 25 minutes to complete the level

- Book of Medivh: More than 25 minutes to complete the level

Good luck on getting the Scepter of Sagreras!

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
