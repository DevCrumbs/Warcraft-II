#ifndef __j1MAP_H__
#define __j1MAP_H__

#include <list>

#include "PugiXml/src/pugixml.hpp"
#include "SDL\include\SDL.h"

#include "j1Module.h"

#include "p2Point.h"

#include "j1App.h"
#include "j1Textures.h"

using namespace std;

struct Object {
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
			objects.erase(item);
			item++;
		}
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

// TODO 1: Create a struct for the map layer
// ----------------------------------------------------

enum LAYER_TYPE {
	LAYER_TYPE_NONE,
	LAYER_TYPE_COLLISION,
	LAYER_TYPE_ABOVE,
	LAYER_TYPE_PARALLAX
};

struct MapLayer {

	string name;
	LAYER_TYPE index = LAYER_TYPE_NONE;

	uint width = 0; //number of tiles in the x axis
	uint height = 0; //number of tiles in the y axis

	uint* data = nullptr;
	uint sizeData = 0;

	float speed = 1.0f; //parallax (speed of the layer)

	Properties	properties;

	~MapLayer() {
		RELEASE_ARRAY(data);
	}

	// TODO 6: Short function to get the value of x,y
	inline uint Get(int x, int y) const;
};

// ----------------------------------------------------
struct TileSet
{
	// TODO 7: Create a method that receives a tile id and returns it's Rectfind the Rect associated with a specific tile id
	SDL_Rect GetTileRect(int id) const;

	string				name;
	int					firstgid = 0;
	int					margin = 0;
	int					spacing = 0;
	int					tileWidth = 0;
	int					tileHeight = 0;
	SDL_Texture*		texture = nullptr;
	int					texWidth = 0;
	int					texHeight = 0;
	int					numTilesWidth = 0;
	int					numTilesHeight = 0;
	int					offsetX = 0;
	int					offsetY = 0;

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
	SDL_Color			backgroundColor;
	MapTypes			type = MAPTYPE_UNKNOWN;
	list<TileSet*>	tilesets;

	// TODO 2: Add a list/array of layers to the map!
	list<MapLayer*> layers;

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
	void DrawAboveLayer();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	// Unload map
	bool UnLoad();

	// TODO 8: Create a method that translates x,y coordinates from map positions to world positions
	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;

	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tilesetNode, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tilesetNode, TileSet* set);

	// TODO 3: Create a method that loads a single layer
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);

	bool LoadObjectGroupDetails(pugi::xml_node& objectGroupNode, ObjectGroup* objectGroup);
	bool LoadObject(pugi::xml_node& objectNode, Object* object);

	TileSet* GetTilesetFromTileId(int id) const;

public:

	MapData				data;
	MapLayer*			collisionLayer = nullptr;

private:

	pugi::xml_document	mapFile;
	string				folder;
	bool				isMapLoaded = false;

	MapLayer*			aboveLayer = nullptr;

public:

	int					culingOffset = 0;
	int					blitOffset = 0;
	bool				cameraBlit = false;
};

#endif // __j1MAP_H__
