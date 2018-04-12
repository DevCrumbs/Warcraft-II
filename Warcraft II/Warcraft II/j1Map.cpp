#include <math.h>
#include <time.h>

#include "Brofiler\Brofiler.h"

#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"

#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "j1Scene.h"
#include "j1Audio.h"
#include "j1Map.h"
#include "j1Window.h"
#include "j1EntityFactory.h"
#include "Entity.h"
#include "StaticEntity.h"
#include "j1Player.h"
#include "j1Pathfinding.h"


j1Map::j1Map() : j1Module(), isMapLoaded(false)
{
	name.assign("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.assign(config.child("folder").childValue());

	blitOffset = config.child("general").child("blit").attribute("offset").as_int();
	cameraBlit = config.child("general").child("cameraBlit").attribute("value").as_bool();
	culingOffset = config.child("general").child("culing").attribute("value").as_int();
	///
	defaultRoomSize = 50;
	playerBaseSize = 40;
	defaultLittleRoomSize = 30;
	defaultTileSize = 32;
	defaultHallHeight = 20;
	defaultHallWidth = 8;
	mapTypesNo = config.child("general").child("mapTypesNo").attribute("number").as_int();
	for (pugi::xml_node iterator = config.child("general").child("roomPullNo"); iterator; iterator = iterator.next_sibling("roomPullNo"))
	{
		noPullRoom.push_back(iterator.attribute("number").as_int());
	}

	srand(time(NULL));

	return ret;
}

void j1Map::Draw()
{
	BROFILER_CATEGORY("Draw(notAbove)", Profiler::Color::Azure);

	if (!isMapLoaded)
		return;

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		App->render->camera.x = 0;
		App->render->camera.y = 0;
	}
	// TODO 5: Prepare the loop to draw all tilesets + Blit

	list<Room>::iterator roomIterator = playableMap.rooms.begin();
	while (roomIterator != playableMap.rooms.end())
	{
		SDL_Texture* tex = nullptr;
		for (list<MapLayer*>::const_iterator layer = (*roomIterator).layers.begin(); layer != (*roomIterator).layers.end(); ++layer)
		{
			//			if (!(*layer)->properties.GetProperty("Draw", false))
			//				continue;

			if ((*layer)->index != LAYER_TYPE_ABOVE) {
				iPoint startTile = WorldToMap(-App->render->camera.x  / App->win->GetScale() - (*roomIterator).x,
										 -App->render->camera.y / App->win->GetScale() - (*roomIterator).y);
				iPoint endTile = WorldToMap(-App->render->camera.x / App->win->GetScale() - (*roomIterator).x + App->render->camera.w,
										 -App->render->camera.y / App->win->GetScale() - (*roomIterator).y + App->render->camera.h);
				int i = startTile.x + 1;
				if (i < 0 && endTile.x >= 0)
					i = 0;

				for (; i < (*layer)->width && i < endTile.x; ++i) {
					int j = startTile.y + 1;
					if (j < 0 && endTile.y >= 0)
						j = 0;

					for (; j < (*layer)->height && j < endTile.y; ++j) {

						int tileId = (*layer)->Get(i, j);
						if (tileId > 0) {

							TileSet* tileset = GetTilesetFromTileId(tileId);
							if(tex == nullptr)
							tex = tileset->texture;
							SDL_Rect rect = tileset->GetTileRect(tileId);

							SDL_Rect* section = &rect;
							iPoint world = MapToWorld(i, j);


							App->render->Blit(tileset->texture, world.x + (*roomIterator).x, world.y + (*roomIterator).y, section, (*layer)->speed);
						}
					}//for
				}//for

				SDL_Rect section = { 32,32,32,32 };

				App->render->Blit(tex, (*roomIterator).exitPointN.x, (*roomIterator).exitPointN.y, &section);
				App->render->Blit(tex, (*roomIterator).exitPointE.x, (*roomIterator).exitPointE.y, &section);
				App->render->Blit(tex, (*roomIterator).exitPointS.x, (*roomIterator).exitPointS.y, &section);
				App->render->Blit(tex, (*roomIterator).exitPointW.x, (*roomIterator).exitPointW.y, &section);
			}
		}
		roomIterator++;

	}


	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		App->render->camera.x = -2400;
		App->render->camera.y = -6720;
	}
}

void j1Map::DrawAboveLayer()
{
	/*
	BROFILER_CATEGORY("DrawAboveLayer", Profiler::Color::Azure);

	if (!isMapLoaded)
		return;

	if (aboveLayer != nullptr) {
		
		list<TileSet*>::const_iterator drawTilesets = data.tilesets.begin();
		while (drawTilesets != data.tilesets.end())
		{
			for (int i = 0; i < aboveLayer->width; i++) {
				for (int j = 0; j < aboveLayer->height; j++) {

					if (aboveLayer->Get(i, j) != 0) {

						SDL_Rect rect = (*drawTilesets)->GetTileRect(aboveLayer->Get(i, j));
						SDL_Rect* section = &rect;

						iPoint world = MapToWorld(i, j);

						App->render->Blit((*drawTilesets)->texture, world.x, world.y, section, aboveLayer->speed);
					}
				}
			}
			drawTilesets++;
		}
	}
	*/
}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	list<TileSet*>::const_iterator item = data.tilesets.begin();
	TileSet* set = *item;

	while (item != data.tilesets.end())
	{
		if (id < (*item)->firstgid)
		{
			set = *(item--);
			break;
		}
		set = *item;
		item++;
	}

	return set;
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if (data.type == ROOMTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tileWidth;
		ret.y = y * data.tileHeight;
	}
	else if (data.type == ROOMTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tileWidth * 0.5f);
		ret.y = (x + y) * (data.tileHeight * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);

	if (data.type == ROOMTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tileWidth;
		ret.y = y / data.tileHeight;
	}
	else if (data.type == ROOMTYPE_ISOMETRIC)
	{

		float half_width = data.tileWidth * 0.5f;
		float half_height = data.tileHeight * 0.5f;
		ret.x = int((x / half_width + y / half_height) / 2) - 1;
		ret.y = int((y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	/// Heeeelp
	int relativeId = id - 1 /*firstgid*/;
	SDL_Rect rect;
	rect.w = tileWidth;
	rect.h = tileHeight;
	rect.x = margin + ((rect.w + 1/*spacing*/) * (relativeId % numTilesWidth));
	rect.y = margin + ((rect.h + 1/*spacing*/) * (relativeId / numTilesWidth));
	
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	bool ret = true;

	LOG("Unloading map");

	// Remove all objectGroups
	list<ObjectGroup*>::const_iterator objectGroup;
	objectGroup = data.objectGroups.begin();

	while (objectGroup != data.objectGroups.end())
	{
		// Remove all objects inside the objectGroup
		list<Object*>::const_iterator object;
		object = (*objectGroup)->objects.begin();

		while (object != (*objectGroup)->objects.end())
		{
			delete (*object);
			object++;
		}
		(*objectGroup)->objects.clear();

		// Remove the objectGroup
		delete (*objectGroup);
		objectGroup++;
	}
	data.objectGroups.clear();

	// Remove all tilesets
	list<TileSet*>::const_iterator item;
	item = data.tilesets.begin();

	while (item != data.tilesets.end())
	{
		delete *item;
		item++;
	}
	data.tilesets.clear();

	// Remove all layers
	list<MapLayer*>::const_iterator item1;
	item1 = data.layers.begin();

	while (item1 != data.layers.end())
	{
		delete *item1;
		item1++;
	}
	data.layers.clear();

	delete collisionLayer;
	delete aboveLayer;

	collisionLayer = nullptr;
	aboveLayer = nullptr;

	// Clean up the pugui tree
	mapFile.reset();


	return ret;
}

// Unload map
bool j1Map::UnLoad()
{
	bool ret = true;


	roomsInfo.clear();
//	noPullRoom.clear();
	
	list<Room>::iterator iterator = playableMap.rooms.begin();
	while (iterator != playableMap.rooms.end())
	{
		list<TileSet*>::iterator tileIterator = (*iterator).tilesets.begin();
		while (tileIterator != (*iterator).tilesets.end())
		{
			App->tex->UnLoad((*tileIterator)->texture);
			tileIterator++;
		}
		iterator++;
	}

	playableMap.rooms.clear();

	LOG("Unloading map");

	// Remove all objectGroups
	list<ObjectGroup*>::const_iterator objectGroup;
	objectGroup = data.objectGroups.begin();

	while (objectGroup != data.objectGroups.end())
	{
		// Remove all objects inside the objectGroup
		list<Object*>::const_iterator object;
		object = (*objectGroup)->objects.begin();

		while (object != (*objectGroup)->objects.end())
		{
			delete (*object);
			object++;
		}
		(*objectGroup)->objects.clear();

		// Remove the objectGroup
		delete (*objectGroup);
		objectGroup++;
	}
	data.objectGroups.clear();

	// Remove all tilesets
	list<TileSet*>::const_iterator item;
	item = data.tilesets.begin();

	while (item != data.tilesets.end())
	{
		delete *item;
		item++;
	}
	data.tilesets.clear();

	// Remove all layers
	list<MapLayer*>::const_iterator item1;
	item1 = data.layers.begin();

	while (item1 != data.layers.end())
	{
		delete *item1;
		item1++;
	}
	data.layers.clear();

	delete collisionLayer;
	delete aboveLayer;

	collisionLayer = nullptr;
	aboveLayer = nullptr;

	for (list<StaticEntity*>::iterator iterator = App->entities->activeStaticEntities.begin(); iterator != App->entities->activeStaticEntities.end(); ++iterator)
	{
		delete (*iterator);
	}
	App->entities->activeStaticEntities.clear();

	return ret;
}

// Load map general properties
bool j1Map::LoadRoom()
{
	bool ret = true;
	pugi::xml_node map = mapFile.child("map");

	if (map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}

	else
	{
		Room tempRoom;
		
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tileWidth = map.attribute("tilewidth").as_int();
		data.tileHeight = map.attribute("tileheight").as_int();
		string backgroundColor(map.attribute("backgroundcolor").as_string());

		data.backgroundColor.r = 0;
		data.backgroundColor.g = 0;
		data.backgroundColor.b = 0;
		data.backgroundColor.a = 0;

		if (backgroundColor.size() > 0)
		{
			string red, green, blue;
			red = backgroundColor.substr(1, 2);
			green = backgroundColor.substr(3, 4);
			blue = backgroundColor.substr(5, 6);

			int v = 0;

			sscanf_s(red.data(), "%x", &v);
			if (v >= 0 && v <= 255) data.backgroundColor.r = v;

			sscanf_s(green.data(), "%x", &v);
			if (v >= 0 && v <= 255) data.backgroundColor.g = v;

			sscanf_s(blue.data(), "%x", &v);
			if (v >= 0 && v <= 255) data.backgroundColor.b = v;
		}

		string orientation(map.attribute("orientation").as_string());

		if (orientation == "orthogonal")
		{
			data.type = ROOMTYPE_ORTHOGONAL;
		}
		else if (orientation == "isometric")
		{
			data.type = ROOMTYPE_ISOMETRIC;
		}
		else if (orientation == "staggered")
		{
			data.type = ROOMTYPE_STAGGERED;
		}
		else
		{
			data.type = ROOMTYPE_UNKNOWN;
		}
	}


	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tilesetNode, TileSet* set)
{
	bool ret = true;
	set->name.assign(tilesetNode.attribute("name").as_string());
	set->firstgid = tilesetNode.attribute("firstgid").as_int();
	set->tileWidth = tilesetNode.attribute("tilewidth").as_int();
	set->tileHeight = tilesetNode.attribute("tileheight").as_int();
	set->margin = tilesetNode.attribute("margin").as_int();
	set->spacing = tilesetNode.attribute("spacing").as_int();
	pugi::xml_node offset = tilesetNode.child("tileoffset");

	if (offset != NULL)
	{
		set->offsetX = offset.attribute("x").as_int();
		set->offsetY = offset.attribute("y").as_int();
	}
	else
	{
		set->offsetX = 0;
		set->offsetY = 0;
	}

	return ret;
}

// Load new room
bool j1Map::Load(const char* fileName, int x, int y, int type)
{
	Room* newRoom = new Room;
	newRoom->x = x;
	newRoom->y = y;
	data = *newRoom;
	delete newRoom;

	bool ret = true;

	data.roomType = type;

	pugi::xml_parse_result result = mapFile.loadFile(fileName);

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", fileName, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if (ret)
	{
		ret = LoadRoom();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for (tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if (ret)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// TODO 4: Iterate all layers and load each of them
	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = mapFile.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* set = new MapLayer();

		if (ret)
		{
			ret = LoadLayer(layer, set);
		}

		data.layers.push_back(set);
	}

	// Load ObjectGroups and GameObjects
	pugi::xml_node objectGroup;
	pugi::xml_node object;

	for (objectGroup = mapFile.child("map").child("objectgroup"); objectGroup && ret; objectGroup = objectGroup.next_sibling("objectgroup"))
	{
		ObjectGroup* set = new ObjectGroup();

		if (ret)
		{
			ret = LoadObjectGroupDetails(objectGroup, set);
		}

		for (object = objectGroup.child("object"); object && ret; object = object.next_sibling("object"))
		{
			Object* set1 = new Object();

			if (ret)
			{
				ret = LoadObject(object, set1);
			}

			set->objects.push_back(set1);
		}

		data.objectGroups.push_back(set);
	}


	if (ret)
	{
		LOG("Successfully parsed map XML file: %s", fileName);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tileHeight: %d", data.tileWidth, data.tileHeight);

		list<TileSet*>::const_iterator item = data.tilesets.begin();
		while (item != data.tilesets.end())
		{
			TileSet* s = *item;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.data(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tileWidth, s->tileHeight);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item++;
		}

		// TODO 4: Add info here about your loaded layers
		// Adapt this vcode with your own variables
		list<MapLayer*>::const_iterator item_layer = data.layers.begin();
		while (item_layer != data.layers.end())
		{
			MapLayer* l = *item_layer;
			LOG("Layer ----");
			LOG("name: %s", l->name.data());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer++;
		}

		// Info about ObjectGroups and GameObjects
		list<ObjectGroup*>::const_iterator item_group = data.objectGroups.begin();
		while (item_group != data.objectGroups.end())
		{
			ObjectGroup* s = *item_group;
			LOG("Object Group ----");
			LOG("name: %s", s->name.data());

			list<Object*>::const_iterator item_object = (*item_group)->objects.begin();
			while (item_object != (*item_group)->objects.end())
			{
				Object* s = *item_object;
				LOG("Object ----");
				LOG("name: %s", s->name.data());
				LOG("id: %d", s->id);
				LOG("x: %d y: %d", s->x, s->y);
				LOG("width: %d height: %d", s->width, s->height);

				item_object++;
			}
			item_group++;
		}
	}

	isMapLoaded = ret;

	//Walkability map info
	if (ret)
	{
		data.walkabilityMap = CreateLowLevelWalkabilityMap(&data);
		data.collider = { data.x, data.y, data.width * data.tileWidth, data.height * data.tileHeight };
	}

	if (ret)
		playableMap.rooms.push_back(data);

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node imageInfo)
{
	bool ret = true;

	if (imageInfo == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		SDL_Texture* texture = App->tex->Load(PATH(folder.data(), imageInfo.attribute("source").as_string()));

		int w, h;
		SDL_QueryTexture(texture, NULL, NULL, &w, &h);

		for (list<Room>::iterator roomIterator = playableMap.rooms.begin(); roomIterator != playableMap.rooms.end(); ++roomIterator)
		{
			for (list<TileSet*>::iterator setIterator = (*roomIterator).tilesets.begin(); setIterator != (*roomIterator).tilesets.end(); ++setIterator) {

				(*setIterator)->texture = texture;

				(*setIterator)->texWidth = imageInfo.attribute("width").as_int();

			if ((*setIterator)->texWidth <= 0)
				{
					(*setIterator)->texWidth = w;
				}

				(*setIterator)->texHeight = imageInfo.attribute("height").as_int();

				if ((*setIterator)->texHeight <= 0)
				{
					(*setIterator)->texHeight = h;
				}

				(*setIterator)->numTilesWidth = (*setIterator)->texWidth / (*setIterator)->tileWidth;
				(*setIterator)->numTilesHeight = (*setIterator)->texHeight / (*setIterator)->tileHeight;
			}
		}
	}

	return ret;
}

// TODO 3: Create the definition for a function that loads a single layer
bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();

	// Set layer index
	if (layer->name == "Collision") {
		collisionLayer = layer;
		layer->index = LAYER_TYPE_COLLISION;
	}
	else if (layer->name == "Above") {
		aboveLayer = layer;
		layer->index = LAYER_TYPE_ABOVE;
	}
	else if (layer->name == "Parallax") {
		layer->index = LAYER_TYPE_PARALLAX;
	}

	layer->width = node.attribute("width").as_uint();
	layer->height = node.attribute("height").as_uint();
	LoadProperties(node, layer->properties);
	layer->sizeData = layer->width * layer->height;
	layer->data = new uint[layer->sizeData];

	memset(layer->data, 0, layer->width * layer->height);

	int i = 0;

	for (pugi::xml_node tileGid = node.child("data").child("tile"); tileGid; tileGid = tileGid.next_sibling("tile")) {
		layer->data[i++] = tileGid.attribute("gid").as_uint();
	}

	// Read layer properties
	pugi::xml_node speed = node.child("properties").child("property");
	if (speed != nullptr) {
		string name = speed.attribute("name").as_string();
		if (name == "Speed")
			layer->speed = speed.attribute("value").as_float();
	}

	return ret;
}

inline uint MapLayer::Get(int x, int y) const 
{
	return data[width * y + x];
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if (data != NULL)
	{
		pugi::xml_node prop;

		for (prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_bool();

			properties.properties.push_back(p);
		}
	}

	return ret;
}

bool j1Map::CreateWalkabilityMap()
{
	bool ret = true;

	hiLevelWalkabilityMap = CreateHiLevelWalkabilityMap();

//	hiLevelWalkabilityMap = (lowLevelWalkabilityMap.back());

	return ret;
}

bool j1Map::SetWalkabilityMap(int& hiWidth, int& hiHieight, uchar** hiBuffer, int& lowWidth, int& lowHeight, uchar** lowBuffer) const
{
	return true;
}

WalkabilityMap j1Map::CreateHiLevelWalkabilityMap()
{
	WalkabilityMap hiLevel;

	pugi::xml_node hiLevelWalkMap = hiLevelWalkabilityMapDocument;
	pugi::xml_node& node = hiLevelWalkMap.child("map").child("layer");

	int width = node.attribute("width").as_uint();
	int height = node.attribute("height").as_uint();


	hiLevelMapWidth = width;
	hiLevelMapHeight = height;
	hiLevelMapSize = hiLevelMapWidth * hiLevelMapHeight;

	hiLevelMap = new uint[hiLevelMapSize];

	int i = 0;
	for (pugi::xml_node tileGid = node.child("data").child("tile"); tileGid; tileGid = tileGid.next_sibling("tile")) {
		hiLevelMap[i++] = tileGid.attribute("gid").as_uint(0);
	}
	LOG("%i%i%i", hiLevelMap[0], hiLevelMap[1], hiLevelMap[2]);

	if (hiLevelMapSize > 0)
	{
		uchar* map = new uchar[hiLevelMapSize];
		memset(map, 1, hiLevelMapSize);

		for (int i = 0; i < hiLevelMapSize; ++i)
		{
			map[i] = (hiLevelMap[i]) > 0 ? 1 : 0;
		}

		LOG("%i%i%i", map[0], map[1], map[2]);

		hiLevel.map = map;
		hiLevel.width = hiLevelMapWidth;
		hiLevel.height = hiLevelMapHeight;
	}
	else
		LOG("No map loaded");

	return hiLevel;
}

WalkabilityMap j1Map::CreateLowLevelWalkabilityMap(Room* currRoom)
{
	WalkabilityMap wMap;
	list<MapLayer*>::const_iterator item;
	item = currRoom->layers.begin();
	for (item; item != currRoom->layers.end(); ++item)
	{
		MapLayer* layer = *item;
		///	if (!layer->properties.GetProperty("Navigation", false))
		if (!layer->properties.GetProperty("navigation", false))
			continue;

		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);

		for (int y = 0; y < currRoom->height; ++y)
		{
			for (int x = 0; x < currRoom->width; ++x)
			{
				int i = (y*layer->width) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;

				if (tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
					/*TileType* ts = tileset->GetTileType(tileId);
					if(ts != NULL)
					{
					map[i] = ts->properties.Get("walkable", 1);
					}*/
				}
			}
		}
		wMap.position = { currRoom->x, currRoom->y };
		wMap.map = map;
		wMap.width = currRoom->width;
		wMap.height = currRoom->height;

		lowLevelWalkabilityMap.push_back(wMap);
		break;

	}
	return wMap;

}

bool Properties::GetProperty(const char* value, bool default_value) const
{
	list<Property*>::const_iterator item = properties.begin();

	while (item != properties.end())
	{
		if ((*item)->name == value)
			return (*item)->value;
		item++;
	}

	return default_value;
}

bool j1Map::LoadObjectGroupDetails(pugi::xml_node& objectGroupNode, ObjectGroup* objectGroup)
{
	bool ret = true;
	objectGroup->name.assign(objectGroupNode.attribute("name").as_string());

	return ret;
}

bool j1Map::LoadObject(pugi::xml_node& objectNode, Object* object)
{
	bool ret = true;

	object->name = objectNode.attribute("name").as_string();
	object->id = objectNode.attribute("id").as_uint();
	object->width = objectNode.attribute("width").as_uint();
	object->height = objectNode.attribute("height").as_uint();
	object->x = objectNode.attribute("x").as_uint();
	object->y = objectNode.attribute("y").as_uint();
	object->type = objectNode.attribute("type").as_uint();

	return ret;
}

bool j1Map::CreateNewMap()
{
	bool ret = true;

	//Decide map type
	int mapType = 0;
	if (mapTypesNo > 0)
		mapType = (rand() % mapTypesNo);
	else
	{
		ret = false;
		LOG("Could not load map, no map types found");
	}
	//Search map type

	iPoint test = WorldToTile(TileToWorld({ 3,4 }));

	if (ret)
	{
		static char typePath[50];
		///sprintf_s(typePath, 50, "data/maps/mapTypes/map%i.xml", mapType);
		//sprintf_s(typePath, 50, "data/maps/mapTypes/mapStructure%i.tmx", mapType);
		sprintf_s(typePath, 50, "data/maps/mapTypes/mapStructure12.tmx");
		mapInfoDocument.loadFile(typePath);

		pugi::xml_node mapInfo = mapInfoDocument;
		ret = LoadMapInfo(mapInfo);

		if (!ret)
			LOG("Could not load rooms");

		else
		{
			static char mapPath[50];
			///sprintf_s(typePath, 50, "data/maps/mapTypes/map%i.xml", mapType);
			//sprintf_s(typePath, 50, "data/maps/mapTypes/hiLevelWalkabilityMap%i.tmx", mapType);
			sprintf_s(mapPath, 50, "data/maps/mapTypes/hiLevelWalkabilityMap1.tmx");
			hiLevelWalkabilityMapDocument.loadFile(mapPath);	
		}


	}
	if (!ret) 
	{

		LOG("Could not find map with specific type (type is %i)", mapType);
			ret = false;
	}
		
	if (ret)
		ret = SelectRooms();

	if (ret)
		ret = LoadRooms();

	if (ret)
		ret = LoadLogic();

//	if (ret)
//		ret = LoadCorridors();

	if (ret)
	{
		pugi::xml_node imageInfo = mapInfoDocument.child("map").child("image");

		ret = LoadTilesetImage(imageInfo);
	}

	return ret;
}

bool j1Map::LoadMapInfo(pugi::xml_node& mapInfoDocument)
{
	bool ret = true;

	DIRECTION direction = DIRECTION_NONE;

	pugi::xml_node& node = mapInfoDocument.child("map").child("layer");

	int width = node.attribute("width").as_uint();
	int height = node.attribute("height").as_uint();

	mapWidth = width;
	mapHeight = height;
	mapSize = mapWidth * mapHeight;

	mapDistribution = new uint[mapSize];
	memset(mapDistribution, 0, mapSize);

	int i = 0;
	for (pugi::xml_node tileGid = node.child("data").child("tile"); tileGid; tileGid = tileGid.next_sibling("tile")) {
		mapDistribution[i++] = tileGid.attribute("gid").as_uint();
	}


	for (i = 0; i < mapSize; ++i)
	{
		RoomInfo newRoom;

		if (mapDistribution[i] > 0)
		{
			if (mapDistribution[i] != 3 && mapDistribution[i] != 4 && mapDistribution[i] != 5 && mapDistribution[i] != 6)
			{
				if (i < mapSize - 1)
					if (mapDistribution[i + 1] > 0)
						newRoom.doors.push_back(DIRECTION_EAST);

				if (i + width < mapSize)
					if (mapDistribution[i + width] > 0)
						newRoom.doors.push_back(DIRECTION_SOUTH);
			}
			int x = i % width;
			int y = i / width;
			iPoint pos = MapToWorld(x, y);

			switch (mapDistribution[i])
			{
				// Player base
			case 1:
				newRoom.type = -1;
				
				newRoom.x = ((pos.x * defaultRoomSize)  + (pos.x * defaultHallHeight) + ((defaultRoomSize - playerBaseSize) / 2)) * defaultTileSize;
				newRoom.y = ((pos.y * defaultRoomSize) + (pos.y * defaultHallHeight)) * defaultTileSize;
				break;	
				// Enemy base
			case 2:
				newRoom.type = -2;

				pos = TileToWorld(pos);
				newRoom.x = pos.x;
				newRoom.y = pos.y;
				break;
				// Default Room
			case 3:
				newRoom.type = 0;

				pos = TileToWorld(pos);
				newRoom.x = pos.x;
				newRoom.y = pos.y;
				break;
				// Little room N
			case 4:
				newRoom.type = -3;

				pos = TileToWorld(pos);
				newRoom.x = ((pos.x / defaultTileSize) + ((defaultRoomSize - defaultLittleRoomSize) / 2)) * defaultTileSize;
				newRoom.y = ((pos.y / defaultTileSize) + (defaultRoomSize - defaultLittleRoomSize)) * defaultTileSize;
				break;
				// Little room E
			case 5:
				newRoom.type = -3;

				pos = TileToWorld(pos);

				newRoom.x = pos.x;
				newRoom.y = ((pos.y / defaultTileSize) + ((defaultRoomSize - defaultLittleRoomSize) / 2)) * defaultTileSize;

				break;
				// Little room S	
			case 6:
				newRoom.type = -3;

				pos = TileToWorld(pos);
				newRoom.x = ((pos.x / defaultTileSize) + ((defaultRoomSize - defaultLittleRoomSize) / 2)) * defaultTileSize;
				newRoom.y = pos.y;

				break;
				// Little room W
			case 7:
				newRoom.type = -3;

				pos = TileToWorld(pos);
				newRoom.x = ((pos.x / defaultTileSize) + (defaultRoomSize - defaultLittleRoomSize)) * defaultTileSize;
				newRoom.y = ((pos.y / defaultTileSize) + ((defaultRoomSize - defaultLittleRoomSize) / 2)) * defaultTileSize;
				break;
				// Vertical hall
			case 9:
				newRoom.type = -4;

				pos = TileToWorld(pos);
				newRoom.x = ((pos.x / defaultTileSize) + ((defaultRoomSize - defaultHallWidth) / 2)) * defaultTileSize;
				newRoom.y = pos.y;

				break;
				// Horizontal hall
			case 10:
				newRoom.type = -5;

				pos = TileToWorld(pos);
				newRoom.x = pos.x;
				newRoom.y = ((pos.y / defaultTileSize) + ((defaultRoomSize - defaultHallWidth) / 2)) * defaultTileSize;

					break;
			default:
				break;
			}
			roomsInfo.push_back(newRoom);
		}

	}

	return ret;
}

bool j1Map::SelectRooms()
{
	bool ret = true;
	int room = 0;

	list<RoomInfo>::iterator roomIterator = roomsInfo.begin();
	while (roomIterator != roomsInfo.end())
	{
		if (noPullRoom.size() > (*roomIterator).type)
		{
			room = rand() % noPullRoom[(*roomIterator).type];
			(*roomIterator).pullRoomNo = room;
		}
		else if ((*roomIterator).type == -1 || (*roomIterator).type == -2)
		{
			(*roomIterator).pullRoomNo = (*roomIterator).type;
		}
		else if ((*roomIterator).type == -3)
		{
			(*roomIterator).pullRoomNo = (*roomIterator).type;
		}
		else if ((*roomIterator).type == -4)
		{
			(*roomIterator).pullRoomNo = (*roomIterator).type;
		}
		else if ((*roomIterator).type == -5)
		{
			(*roomIterator).pullRoomNo = (*roomIterator).type;
		}
		else
		{
			LOG("Error, room type exeed pull size");
			ret = false;
			break;
		}
		roomIterator++;
	}
	return ret;
}

bool j1Map::LoadRooms()
{
	bool ret = true;
	list<RoomInfo>::iterator roomIterator = roomsInfo.begin();

	while (roomIterator != roomsInfo.end())

	{
		if ((*roomIterator).type >= 0 && (*roomIterator).pullRoomNo >= 0)
		{
			static char roomPath[50];
			///sprintf_s(roomPath, 50, "data/maps/rooms/pull%i/room%i.tmx", (*roomIterator).type, (*roomIterator).pullRoomNo);
			sprintf_s(roomPath, 50, "data/maps/rooms/pull0/room7.tmx");
			ret = Load(roomPath, (*roomIterator).x, (*roomIterator).y);
		}
		else if ((*roomIterator).type == -1)
		{
			ret = Load("data/maps/rooms/base/base.tmx", (*roomIterator).x, (*roomIterator).y, (*roomIterator).type);
		}
		else if ((*roomIterator).type == -2)
		{
			ret = Load("data/maps/rooms/boss/boss.tmx", (*roomIterator).x, (*roomIterator).y, (*roomIterator).type);
		}
		else if ((*roomIterator).type == -3)
		{
			ret = Load("data/maps/rooms/littleroom/littleRoom0.tmx", (*roomIterator).x, (*roomIterator).y, (*roomIterator).type);
		}
		else if ((*roomIterator).type == -4)
		{
			ret = Load("data/maps/corridors/corridorV.tmx", (*roomIterator).x, (*roomIterator).y, (*roomIterator).type);
		}
		else if ((*roomIterator).type == -5)
		{
			ret = Load("data/maps/corridors/corridorH.tmx", (*roomIterator).x, (*roomIterator).y, (*roomIterator).type);
		}
		if (!ret)
			break;

		roomIterator++;

	}
return ret;
}

bool j1Map::LoadCorridors()
{
	bool ret = true;
	list<RoomInfo>::iterator roomIterator = roomsInfo.begin();
	list<Room>::iterator mapIterator = playableMap.rooms.begin();

	while (roomIterator != roomsInfo.end() && mapIterator != playableMap.rooms.end())
	{
		list<DIRECTION>::iterator doorIterator = (*roomIterator).doors.begin();

		while (doorIterator != (*roomIterator).doors.end())
		{
			CreateCorridor((*mapIterator), (*doorIterator));

			doorIterator++;

		}
		roomIterator++;
		mapIterator++;
	}
	return ret;
}

bool j1Map::CreateCorridor(Room room, DIRECTION direction)
{
	bool ret = true;


	int roomX = room.x;
	int roomY = room.y;
	int tileNoX = room.width;
	int tileNoY = room.height;
	int tileWidth = room.tileWidth;
	int tileHeight = room.tileHeight;

	int corridorX = 0, corridorY = 0;

	int CORRIDOR_WIDTH = 4;
	int CORRIDOR_HEIGHT = 20;

	switch ((direction))
	{
	case DIRECTION_NONE:
		break;
	case DIRECTION_NORTH:
		corridorX = roomX + (((tileNoX / 2) - CORRIDOR_WIDTH) * tileWidth);
		corridorY = roomY - (CORRIDOR_HEIGHT * tileHeight);
		ret = Load("data/maps/corridors/corridorV.tmx", corridorX, corridorY);
		break;
	case DIRECTION_EAST:
		corridorX = roomX + (tileNoX * tileWidth);
		corridorY = roomY + (((tileNoY / 2) - CORRIDOR_WIDTH) * tileWidth);
		ret = Load("data/maps/corridors/corridorH.tmx", corridorX, corridorY);
		break;
	case DIRECTION_SOUTH:
		corridorX = roomX + (((tileNoX / 2) - CORRIDOR_WIDTH) * tileWidth);
		corridorY = roomY + (tileNoY * tileHeight);
		ret = Load("data/maps/corridors/corridorV.tmx", corridorX, corridorY);
		break;
	case DIRECTION_WEST:
		corridorX = roomX - (CORRIDOR_HEIGHT * tileWidth);
		corridorY = roomY + (((tileNoY / 2) - CORRIDOR_WIDTH) * tileWidth);
		ret = Load("data/maps/corridors/corridorH.tmx", corridorX, corridorY);
		break;
	default:
		break;
	}

	return true;
}

bool j1Map::LoadLogic()
{
	bool ret = true;
	// Iterate all rooms
	for (list<Room>::iterator iterator = playableMap.rooms.begin();
		iterator != playableMap.rooms.end(); ++iterator)
	{
		// Iterate all layers
		for (list<MapLayer*>::iterator layerIterator = (*iterator).layers.begin();
			layerIterator != (*iterator).layers.end(); ++layerIterator)
		{
			// Check if layer is a logic layer
			if ((*layerIterator)->name == "logic")
			{
				// Iterate layer
				for (int i = 0; i < (*layerIterator)->sizeData; ++i)
				{
					// Check if tile is not empty
					if ((*layerIterator)->data[i] > 0)
					{
						int x = i % (*layerIterator)->width;
						int y = i / (*layerIterator)->width;

						iPoint auxPos = MapToWorld(x, y);
						fPoint pos;
						pos.x = auxPos.x + (*iterator).x;
						pos.y = auxPos.y + (*iterator).y;

						UnitInfo unitInfo;

						ENTITY_TYPE entityType = (ENTITY_TYPE)((*layerIterator)->data[i]);
						switch (entityType)
						{
						case EntityType_TOWN_HALL:
							App->player->townHall = (StaticEntity*)App->entities->AddEntity(entityType, pos, App->entities->GetBuildingInfo(entityType), unitInfo, (j1Module*)App->player);
							break;
						case EntityType_CHICKEN_FARM:
							App->player->chickenFarm.push_back((StaticEntity*)App->entities->AddEntity(entityType, pos, App->entities->GetBuildingInfo(entityType), unitInfo, (j1Module*)App->player));
							break;
						case EntityType_BARRACKS:
							App->player->barracks = (StaticEntity*)App->entities->AddEntity(entityType, pos, App->entities->GetBuildingInfo(entityType), unitInfo, (j1Module*)App->player);
							break;
						default:
							App->entities->AddEntity(entityType, pos, App->entities->GetBuildingInfo(entityType), unitInfo);
							break;
						}


						//App->entities->AddEntity(EntityType_FOOTMAN, pos, App->entities->GetBuildingInfo(EntityType_FOOTMAN));

//						ret = App->entities->AddEntity(x, y, (*layerIterator)->data[i]);
					}
				}
			}
		}

		int x = (*iterator).x;
		int y = (*iterator).y;

		int tileWidth = (*iterator).tileWidth;
		int tileHeight = (*iterator).tileHeight;

		int width = (*iterator).width * tileWidth;
		int height = (*iterator).height * tileHeight;

		switch ((*iterator).roomType)
		{
		case -1:

			break;
		case -2:

			break;
		case -3:

			break;
		default:

			(*iterator).exitPointN = { x + (width / 2),			y - tileHeight };
			(*iterator).exitPointE = { x + width,				y + (height / 2) };
			(*iterator).exitPointS = { x + (width / 2),			y + height};
			(*iterator).exitPointW = { x - tileWidth,			y + (height / 2) };

			break;
		}
	}



	return ret;
}

iPoint j1Map::TileToWorld(iPoint pos)
{
	iPoint ret{ 0,0 };
	
	if ((pos.x % 2) == 0)
		ret.x = (((pos.x / 2) * defaultRoomSize * defaultTileSize) + 
				 ((pos.x / 2) * defaultHallHeight * defaultTileSize));
	else
		ret.x = ((((pos.x / 2) + 1) * defaultRoomSize * defaultTileSize) + 
				  ((pos.x / 2) * defaultHallHeight * defaultTileSize));

	if ((pos.y % 2) == 0)
		ret.y = (((pos.y / 2) * defaultRoomSize * defaultTileSize) + 
		         ((pos.y / 2) * defaultHallHeight * defaultTileSize));
	else
		ret.y = ((((pos.y / 2) + 1) * defaultRoomSize * defaultTileSize) + 
		          ((pos.y / 2) * defaultHallHeight * defaultTileSize));
	
	return ret;
}

iPoint j1Map::WorldToTile(iPoint pos)
{
	iPoint ret{ 0,0 };
	int x = ((defaultRoomSize + defaultHallHeight) * defaultTileSize);
	if ((pos.x % x) == 0)
		ret.x = pos.x / ((defaultRoomSize + defaultHallHeight) * defaultTileSize) * 2;

	else
		ret.x = (pos.x / ((defaultRoomSize + defaultHallHeight) * defaultTileSize)) ;

	if ((pos.y % ((defaultRoomSize + defaultHallHeight) * defaultTileSize)) == 0)
		ret.y = pos.y / ((defaultRoomSize + defaultHallHeight) * defaultTileSize) * 2;

	else
		ret.y = (pos.y / ((defaultRoomSize + defaultHallHeight) * defaultTileSize)) ;

	return ret;
}
//----------------------------------

fPoint Room::GetObjectPosition(string groupObject, string object)
{
	fPoint pos = { 0,0 };

	list<ObjectGroup*>::const_iterator item;
	item = objectGroups.begin();

	int ret = true;

	while (item != objectGroups.end() && ret)
	{
		if ((*item)->name == groupObject) {

			list<Object*>::const_iterator item1;
			item1 = (*item)->objects.begin();

			while (item1 != (*item)->objects.end() && ret)
			{
				if ((*item1)->name == object) {
					pos.x = (*item1)->x;
					pos.y = (*item1)->y;

					ret = false;
				}
				item1++;
			}
		}
		item++;
	}

	return pos;
}

fPoint Room::GetObjectSize(string groupObject, string object)
{
	fPoint size = { 0,0 };

	list<ObjectGroup*>::const_iterator item;
	item = objectGroups.begin();

	int ret = true;

	while (item != objectGroups.end() && ret)
	{
		if ((*item)->name == groupObject) {

			list<Object*>::const_iterator item1;
			item1 = (*item)->objects.begin();

			while (item1 != (*item)->objects.end() && ret)
			{
				if ((*item1)->name == object) {
					size.x = (*item1)->width;
					size.y = (*item1)->height;

					ret = false;
				}
				item1++;
			}
		}
		item++;
	}

	return size;
}

Object* Room::GetObjectByName(string groupObject, string object)
{

	Object* obj = nullptr;

	list<ObjectGroup*>::const_iterator item;
	item = objectGroups.begin();

	int ret = true;

	while (item != objectGroups.end() && ret)
	{
		if ((*item)->name == groupObject) {

			list<Object*>::const_iterator item1;
			item1 = (*item)->objects.begin();

			while (item1 != (*item)->objects.end() && ret)
			{
				if ((*item1)->name == object) {
					obj = *item1;

					ret = false;
				}
				item1++;
			}
		}
		item++;
	}

	return obj;
}

bool Room::CheckIfEnter(string groupObject, string object, fPoint position)
{

	fPoint objectPos = GetObjectPosition(groupObject, object);
	fPoint objectSize = GetObjectSize(groupObject, object);

	return (objectPos.x < position.x + 1 && objectPos.x + objectSize.x > position.x &&
		objectPos.y < position.y + 1 && objectSize.y + objectPos.y > position.y);
}

