#ifndef __j1MAP_H__
#define __j1MAP_H__

#include <list>

#include "j1Module.h"

#include "p2Point.h"

#include "j1App.h"
#include "j1Textures.h"
#include "Entity.h"

#include "PugiXml/src/pugixml.hpp"
#include "SDL\include\SDL.h"

#include "SDL\include\SDL_rect.h"

using namespace std;

class Entity;
enum ENTITY_TYPE;

enum ROOM_TYPE
{
	roomType_NONE = 0,

	roomType_LARGE = 411,
	roomType_LITTLE = 412,
	roomType_BASE = 413

};

struct Room
{
public:
	Room(SDL_Rect rect = { 0,0,0,0 }, bool cleared = false)
	{
		roomRect = rect;
		isCleared = cleared;
	}
	Room()
	{
		roomRect = { 0,0,0,0 };
		isCleared = false;
	}




	bool operator==(Room room)
	{
		return roomRect == room.roomRect && isCleared == room.isCleared;
	}
	bool operator==(SDL_Rect room)
	{
		return roomRect == roomRect;
	}

public:
	SDL_Rect roomRect{ 0,0,0,0 };
	bool isCleared = false;
};


struct Object
{
	string name;
	uint id = 0;
	uint x = 0;
	uint y = 0;
	uint width = 0;
	uint height = 0;
	uint type = 0;
};

struct ObjectGroup {
	string name;

	list<Object*> objects;

	~ObjectGroup() {
		list<Object*>::const_iterator item;
		item = objects.begin();

		while (item != objects.end())
		{
			delete *item;
			item++;
		}
		objects.clear();
	}
};

// ----------------------------------------------------

struct Properties
{
	struct Property
	{
		string name;
		bool value = false;
	};

	~Properties()
	{
		list<Property*>::const_iterator item;
		item = properties.begin();

		while (item != properties.end())
		{
			delete *item;
			item++;
		}
		properties.clear();
	}

	bool GetProperty(const char* name, bool default_value = false) const;

	list<Property*>	properties;
};

// ----------------------------------------------------

struct MapLayer {

	string name;

	uint width = 0; //number of tiles in the x axis
	uint height = 0; //number of tiles in the y axis

	uint* data = nullptr;
	uint sizeData = 0;

	float speed = 1.0f; //parallax (speed of the layer)

	Properties properties;

	~MapLayer() {
		RELEASE_ARRAY(data);
	}

	// Short function to get the value of x,y
	inline uint Get(int x, int y) const;
};

// ----------------------------------------------------
struct TileSet
{
	// Method that receives a tile id and returns it's Rectfind the Rect associated with a specific tile id
	SDL_Rect GetTileRect(int id) const;

	string				name;
	int					firstgid = 0;
	int					margin = 0;
	int					spacing = 0;
	int					tile_width = 0;
	int					tile_height = 0;
	SDL_Texture*		texture = nullptr;
	int					tex_width = 0;
	int					tex_height = 0;
	int					num_tiles_width = 0;
	int					num_tiles_height = 0;
	int					offset_x = 0;
	int					offset_y = 0;

	~TileSet() {
		App->tex->UnLoad(texture);
	}

};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

// ----------------------------------------------------

struct MapData
{
	int					width = 0;
	int					height = 0;
	int					tileWidth = 0;
	int					tileHeight = 0;
	SDL_Color			background_color;
	MapTypes			type = MAPTYPE_UNKNOWN;
	list<TileSet*>		tilesets;
	list<MapLayer*>		layers;

	list<ObjectGroup*> objectGroups;

	fPoint GetObjectPosition(string groupObject, string object);
	fPoint GetObjectSize(string groupObject, string object);
	Object* GetObjectByName(string groupObject, string object);

	bool CheckIfEnter(string groupObject, string object, fPoint position);
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called each loop iteration
	void Draw();

	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	bool LoadLogic();

	list<Entity*> LoadLayerEntities(MapLayer* layer);

	bool LoadRoomRect(MapLayer * layer);

	// This function distributes a list of entities arranged by level to a list of entities arranged by level and room
	bool CreateEntityGroup(list<list<Entity*>> entityGroupLevel);

	bool IsGoalOnRoom(SDL_Rect origin, SDL_Rect goal);
	bool IsGoalOnRoom(iPoint origin, iPoint goal);

	bool IsOnBase(iPoint pos);

	bool IsOnRoom(iPoint pos, Room room);
	bool IsOnRoom(fPoint pos, Room room);

	Room GetEntityRoom(Entity* entity);

	list<Entity*> GetEntitiesOnRoom(Room room, ENTITY_CATEGORY entityType = EntityCategory_NONE, EntitySide entitySide = EntitySide_NoSide);

	bool IsRoomCleared(Room room);

	// Unload map
	bool UnLoad();

	// Method that translates x,y coordinates from map positions to world positions
	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;

	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

	bool LoadWalkabilityMap(int & width, int & height, uchar ** buffer) const;

	TileSet* GetTilesetFromTileId(int id) const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);

	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);

	bool LoadObjectGroupDetails(pugi::xml_node& objectGroup_node, ObjectGroup* objectGroup);
	bool LoadObject(pugi::xml_node& object_node, Object* object);

public:

	MapData				data;
	MapLayer*			collisionLayer = nullptr;

	int					culing_offset = 0;
	int					blit_offset = 0;
	bool				camera_blit = false;

	//Base pos
	SDL_Rect			playerBase{ 0,0,0,0 };

	//Texture path
	string				tilesetPath;

	// Walkability map
	mutable	uchar*		walkMap = nullptr;
	mutable	int         walkWidth = 0;
	mutable	int			walkHeight = 0;

	// Rooms rects
	list<Room>		roomRectList;

	// Default rooms sizes
	int					defaultRoomSize = 0;
	int					defaultBaseSize = 0;
	int					defaultLittleSize = 0;
	int					defaultTileSize = 0;
	int					defaultHallSize = 0;

	list<list<Entity*>>	entityGroups;

private:

	pugi::xml_document	mapFile;
	string				folder;
	bool				map_loaded = false;

	MapLayer*			aboveLayer = nullptr;
};

#endif // __j1MAP_H__

//#ifndef __j1MAP_H__
//#define __j1MAP_H__
//
//#include <list>
//#include <vector>
//
//#include "PugiXml/src/pugixml.hpp"
//#include "SDL\include\SDL.h"
//
//#include "j1Module.h"
//
//#include "p2Point.h"
//
//#include "j1App.h"
//#include "j1Textures.h"
//
//using namespace std;
//
//struct Object {
//	string name;
//	uint id = 0;
//	uint x = 0;
//	uint y = 0;
//	uint width = 0;
//	uint height = 0;
//	uint type = 0;
//};
//
//struct ObjectGroup {
//	string name;
//
//	list<Object*> objects;
//
//	~ObjectGroup() {
//		list<Object*>::const_iterator item;
//		item = objects.begin();
//
//		while (item != objects.end())
//		{
//			objects.erase(item);
//			item++;
//		}
//	}
//};
//
//// ----------------------------------------------------
//
//struct Properties 
//{
//	struct Property
//	{
//		string name;
//		bool value = false;
//	};
//
//	~Properties()
//	{
//		list<Property*>::const_iterator item;
//		item = properties.begin();
//
//		while (item != properties.end())
//		{
//			delete *item;
//			item++;
//		}
//
//		properties.clear();
//	}
//
//	bool GetProperty(const char* name, bool default_value = false) const;
//
//	list<Property*>	properties;
//};
//
//// TODO 1: Create a struct for the map layer
//// ----------------------------------------------------
//
//enum LAYER_TYPE {
//	LAYER_TYPE_NONE,
//	LAYER_TYPE_COLLISION,
//	LAYER_TYPE_ABOVE,
//	LAYER_TYPE_PARALLAX
//};
//
//struct MapLayer {
//
//	string name;
//	LAYER_TYPE index = LAYER_TYPE_NONE;
//
//	uint width = 0; // number of tiles in the x axis
//	uint height = 0; // number of tiles in the y axis
//
//	uint* data = nullptr; // tile gid
//	uint sizeData = 0;
//
//	float speed = 1.0f; //parallax (speed of the layer)
//
//	Properties	properties;
//
//	~MapLayer() {
//		RELEASE_ARRAY(data);
//	}
//
//	// TODO 6: Short function to get the value of x,y
//	inline uint Get(int x, int y) const;
//};
//
//// ----------------------------------------------------
//struct TileSet
//{
//	// TODO 7: Create a method that receives a tile id and returns it's Rectfind the Rect associated with a specific tile id
//	SDL_Rect GetTileRect(int id) const;
//
//	string				name;
//	int					firstgid = 0;
//	int					margin = 0;
//	int					spacing = 0;
//	int					tileWidth = 0;
//	int					tileHeight = 0;
//	SDL_Texture*		texture = nullptr;
//	int					texWidth = 0;
//	int					texHeight = 0;
//	int					numTilesWidth = 0;
//	int					numTilesHeight = 0;
//	int					offsetX = 0;
//	int					offsetY = 0;
//
//	~TileSet() {
//		App->tex->UnLoad(texture);
//	}
//
//};
//
//enum ROOM_TYPE
//{
//	ROOMTYPE_UNKNOWN = 0,
//	ROOMTYPE_ORTHOGONAL,
//	ROOMTYPE_ISOMETRIC,
//	ROOMTYPE_STAGGERED
//};
//
//// ----------------------------------------------------
//
//struct Room
//{
//	int					width = 0;
//	int					height = 0;
//	int					x = 0;
//	int					y = 0;
//	int					tileWidth = 0;
//	int					tileHeight = 0;
//
//	fPoint				roomPos{ 0, 0 };
//	SDL_Color			backgroundColor;
//	ROOM_TYPE			type = ROOMTYPE_UNKNOWN;
//	list<TileSet*>		tilesets;
//
//	// TODO 2: Add a list/array of layers to the map!
//	list<MapLayer*>		layers;
//
//	list<ObjectGroup*>	objectGroups;
//
//	fPoint GetObjectPosition(string groupObject, string object);
//	fPoint GetObjectSize(string groupObject, string object);
//	Object* GetObjectByName(string groupObject, string object);
//
//	bool CheckIfEnter(string groupObject, string object, fPoint position);
//};
//
//// ----------------------------------------------------
//enum MAP_TYPE
//{
//	MAPTYPE_NONE
//};
//struct RoomMap
//{
//	list<Room>		rooms;
//
//	MAP_TYPE			mType = MAPTYPE_NONE;
//
//};
//
//enum DIRECTION
//{
//	DIRECTION_NONE = -1,
//	DIRECTION_NORTH = 0,
//	DIRECTION_EAST,
//	DIRECTION_SOUTH,
//	DIRECTION_WEST
//};
//
//struct RoomInfo
//{
//	int type = -1;
//	int x = 0, y = 0;
//
//	int pullRoomNo = -1;
//
//	list<DIRECTION> doors;
//
//
//};
//	
////-----------------------------------------------------
//
//class j1Map : public j1Module
//{
//public:
//
//	j1Map();
//
//	// Destructor
//	virtual ~j1Map();
//
//	// Called before render is available
//	bool Awake(pugi::xml_node&);
//
//	// Called each loop iteration
//	void Draw();
//	void DrawAboveLayer();
//
//	// Called before quitting
//	bool CleanUp();
//
//	// Load new map
//	bool Load(const char* path, int x = 0, int y = 0);
//
//	// Unload map
//	bool UnLoad();
//
//	// TODO 8: Create a method that translates x,y coordinates from map positions to world positions
//	iPoint MapToWorld(int x, int y) const;
//	iPoint WorldToMap(int x, int y) const;
//
//	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;
//
//	bool CreateNewMap();
//	bool LoadMapInfo(pugi::xml_node& mapInfoDocument);
//	bool SelectRooms();
//	bool LoadRooms();
//	bool LoadCorridors();
//	bool CreateCorridor(Room room, DIRECTION direction = DIRECTION_NONE);
//	bool LoadLogic();
//
//private:
//
//	bool LoadRoom();
//	bool LoadTilesetDetails(pugi::xml_node& tilesetNode, TileSet* set);
//	bool LoadTilesetImage(pugi::xml_node imageInfo);
//
//	// TODO 3: Create a method that loads a single layer
//	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
//	bool LoadProperties(pugi::xml_node& node, Properties& properties);
//
//	bool LoadObjectGroupDetails(pugi::xml_node& objectGroupNode, ObjectGroup* objectGroup);
//	bool LoadObject(pugi::xml_node& objectNode, Object* object);
//
//	TileSet* GetTilesetFromTileId(int id) const;
//
//public:
//
//	Room				data;
//	MapLayer*			collisionLayer = nullptr;
//
//	int					culingOffset = 0;
//	int					blitOffset = 0;
//	bool				cameraBlit = false;
//
//	int					mapTypesNo = 0;
//	int					defaultRoomSize = 0;
//	int					playerBaseSize = 0;
//	int					defaultLittleRoomSize = 0;
//	int					defaultTileSize = 0;
//	int					defaultHallSize = 0;
//
//	mutable uint				width = 0; 
//	mutable uint				height = 0;
//	mutable uchar*				walkabilityMap = nullptr;
//
//private:
//
//	pugi::xml_document	mapFile;
//	pugi::xml_document  mapInfoDocument;
//
//	string				folder;
//	bool				isMapLoaded = false;
//
//	MapLayer*			aboveLayer = nullptr;
//
//	RoomMap				playableMap;
//	list<RoomInfo>		roomsInfo;
//	vector<int>			noPullRoom;
//
//};
//
//#endif // __j1MAP_H__
