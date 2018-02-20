#include <math.h>

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

	return ret;
}

void j1Map::Draw()
{
	BROFILER_CATEGORY("Draw(notAbove)", Profiler::Color::Azure);

	if (!isMapLoaded)
		return;

	// TODO 5: Prepare the loop to draw all tilesets + Blit
	for (list<MapLayer*>::const_iterator layer = data.layers.begin(); layer != data.layers.end(); ++layer) {

		if (!(*layer)->properties.GetProperty("Draw", false))
			continue;

		if ((*layer)->index != LAYER_TYPE_ABOVE) {

			for (int i = 0; i < (*layer)->width; ++i) {
				for (int j = 0; j < (*layer)->height; ++j) {

					int tile_id = (*layer)->Get(i, j);
					if (tile_id > 0) {

						TileSet* tileset = GetTilesetFromTileId(tile_id);

						SDL_Rect rect = tileset->GetTileRect(tile_id);

						SDL_Rect* section = &rect;
						iPoint world = MapToWorld(i, j);

						App->render->Blit(tileset->texture, world.x, world.y, section, (*layer)->speed);
					}
				}//for
			}//for
		}
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

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tileWidth;
		ret.y = y * data.tileHeight;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
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

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tileWidth;
		ret.y = y / data.tileHeight;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
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
	int relativeId = id - firstgid;
	SDL_Rect rect;
	rect.w = tileWidth;
	rect.h = tileHeight;
	rect.x = margin + ((rect.w + spacing) * (relativeId % numTilesWidth));
	rect.y = margin + ((rect.h + spacing) * (relativeId / numTilesWidth));
	
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

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
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
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if (orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if (orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
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

// Load new map
bool j1Map::Load(const char* fileName)
{
	bool ret = true;
	string tmp = folder.data();
	tmp += fileName;

	pugi::xml_parse_result result = mapFile.loadFile(tmp.data());

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", fileName, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if (ret)
	{
		ret = LoadMap();
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

		if (ret)
		{
			ret = LoadTilesetImage(tileset, set);
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

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tilesetNode, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tilesetNode.child("image");

	if (image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.data(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->texWidth = image.attribute("width").as_int();

		if (set->texWidth <= 0)
		{
			set->texWidth = w;
		}

		set->texHeight = image.attribute("height").as_int();

		if (set->texHeight <= 0)
		{
			set->texHeight = h;
		}

		set->numTilesWidth = set->texWidth / set->tileWidth;
		set->numTilesHeight = set->texHeight / set->tileHeight;
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
	layer->data = new uint[layer->width * layer->height];

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

bool j1Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = true;

	list<MapLayer*>::const_iterator item;
	item = data.layers.begin();

	for (item; item != data.layers.end(); ++item)
	{
		MapLayer* layer = *item;

		if (!layer->properties.GetProperty("Navigation", false))
			continue;

		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int i = (y*layer->width) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;

				if (tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
					/*TileType* ts = tileset->GetTileType(tile_id);
					if(ts != NULL)
					{
					map[i] = ts->properties.Get("walkable", 1);
					}*/
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
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

fPoint MapData::GetObjectPosition(string groupObject, string object)
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

fPoint MapData::GetObjectSize(string groupObject, string object)
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

Object* MapData::GetObjectByName(string groupObject, string object)
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

bool MapData::CheckIfEnter(string groupObject, string object, fPoint position)
{

	fPoint objectPos = GetObjectPosition(groupObject, object);
	fPoint objectSize = GetObjectSize(groupObject, object);

	return (objectPos.x < position.x + 1 && objectPos.x + objectSize.x > position.x && objectPos.y < position.y + 1 && objectSize.y + objectPos.y > position.y);
}