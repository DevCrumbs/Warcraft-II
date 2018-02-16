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

#include "Brofiler\Brofiler.h"

#include <math.h>

j1Map::j1Map() : j1Module(), map_loaded(false)
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

	folder.assign(config.child("folder").child_value());

	blit_offset = config.child("general").child("blit").attribute("offset").as_int();
	camera_blit = config.child("general").child("camera_blit").attribute("value").as_bool();
	culing_offset = config.child("general").child("culing").attribute("value").as_int();

	return ret;
}

void j1Map::Draw()
{
	BROFILER_CATEGORY("Draw(notAbove)", Profiler::Color::Azure);

	if (map_loaded == false)
		return;

	// TODO 5: Prepare the loop to draw all tilesets + Blit
	list<TileSet*>::const_iterator draw_tilesets = data.tilesets.begin();
	while (draw_tilesets != data.tilesets.end())
	{
		list<MapLayer*>::const_iterator draw_layers = data.layers.begin();
		while (draw_layers != data.layers.end()) {

			if ((*draw_layers)->index != ABOVE && (*draw_layers)->index != PARALLAX) {
				
				for (int i = 0; i < (*draw_layers)->width; i++) {
					for (int j = 0; j < (*draw_layers)->height; j++) {

						if ((*draw_layers)->Get(i, j) != 0) {

							SDL_Rect rect = (*draw_tilesets)->GetTileRect((*draw_layers)->Get(i, j));

							SDL_Rect* section = &rect;
							iPoint world = MapToWorld(i, j);

							if ((*draw_layers)->index == PARALLAX) {
								App->render->Blit((*draw_tilesets)->texture, world.x, world.y, section, (*draw_layers)->speed);
							}
							else if ((*draw_layers)->index == COLLISION) {
								if (App->collision->GetDebug())
									App->render->Blit((*draw_tilesets)->texture, world.x, world.y, section, (*draw_layers)->speed);
							}
							else {
								App->render->Blit((*draw_tilesets)->texture, world.x, world.y, section, (*draw_layers)->speed);
							}
						}
					}
				}//for
			}

			if ((*draw_layers)->index == PARALLAX) {

				for (int i = 0; i < (*draw_layers)->width; i++) {
					for (int j = 0; j < (*draw_layers)->height; j++) {

						if ((*draw_layers)->Get(i, j) != 0) {

							SDL_Rect rect = (*draw_tilesets)->GetTileRect((*draw_layers)->Get(i, j));

							SDL_Rect* section = &rect;
							iPoint world = MapToWorld(i, j);

							if ((*draw_layers)->index == PARALLAX) {
								App->render->Blit((*draw_tilesets)->texture, world.x, world.y, section, (*draw_layers)->speed);
							}
						}
					}
				}
			}
			draw_layers++;
		}
		draw_tilesets++;
	}
}

void j1Map::DrawAboveLayer()
{
	BROFILER_CATEGORY("DrawAboveLayer", Profiler::Color::Azure);

	if (map_loaded == false)
		return;

	if (aboveLayer != nullptr) {
		
		list<TileSet*>::const_iterator draw_tilesets = data.tilesets.begin();
		while (draw_tilesets != data.tilesets.end())
		{
			for (int i = 0; i < aboveLayer->width; i++) {
				for (int j = 0; j < aboveLayer->height; j++) {

					if (aboveLayer->Get(i, j) != 0) {

						SDL_Rect rect = (*draw_tilesets)->GetTileRect(aboveLayer->Get(i, j));
						SDL_Rect* section = &rect;

						iPoint world = MapToWorld(i, j);

						App->render->Blit((*draw_tilesets)->texture, world.x, world.y, section, aboveLayer->speed);
					}
				}
			}
			draw_tilesets++;
		}
	}
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
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
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
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
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
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	
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
	map_file.reset();

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
	pugi::xml_node map = map_file.child("map");

	if (map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		string bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if (bg_color.size() > 0)
		{
			string red, green, blue;
			red = bg_color.substr(1, 2);
			green = bg_color.substr(3, 4);
			blue = bg_color.substr(5, 6);

			int v = 0;

			sscanf_s(red.data(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.data(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.data(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.b = v;
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

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.assign(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if (offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	string tmp = folder.data();
	tmp += file_name;

	pugi::xml_parse_result result = map_file.load_file(tmp.data());

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if (ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for (tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if (ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if (ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// TODO 4: Iterate all layers and load each of them
	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* set = new MapLayer();

		if (ret == true)
		{
			ret = LoadLayer(layer, set);
		}

		data.layers.push_back(set);
	}

	// Load ObjectGroups and GameObjects
	pugi::xml_node objectGroup;
	pugi::xml_node object;

	for (objectGroup = map_file.child("map").child("objectgroup"); objectGroup && ret; objectGroup = objectGroup.next_sibling("objectgroup"))
	{
		ObjectGroup* set = new ObjectGroup();

		if (ret == true)
		{
			ret = LoadObjectGroupDetails(objectGroup, set);
		}

		for (object = objectGroup.child("object"); object && ret; object = object.next_sibling("object"))
		{
			Object* set1 = new Object();

			if (ret == true)
			{
				ret = LoadObject(object, set1);
			}

			set->objects.push_back(set1);
		}

		data.objectGroups.push_back(set);
	}


	if (ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		list<TileSet*>::const_iterator item = data.tilesets.begin();
		while (item != data.tilesets.end())
		{
			TileSet* s = *item;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.data(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
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

	map_loaded = ret;

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

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
		set->tex_width = image.attribute("width").as_int();

		if (set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if (set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
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
		layer->index = COLLISION;
	}
	else if (layer->name == "Above") {
		aboveLayer = layer;
		layer->index = ABOVE;
	}
	else if (layer->name == "Parallax") {
		layer->index = PARALLAX;
	}

	layer->width = node.attribute("width").as_uint();
	layer->height = node.attribute("height").as_uint();

	layer->data = new uint[layer->width * layer->height];

	memset(layer->data, 0, layer->width * layer->height);

	int i = 0;

	for (pugi::xml_node tile_gid = node.child("data").child("tile"); tile_gid; tile_gid = tile_gid.next_sibling("tile")) {
		layer->data[i++] = tile_gid.attribute("gid").as_uint();
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
			p->value = prop.attribute("value").as_int();

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

	for (item; item != data.layers.end(); item++)
	{
		MapLayer* layer = *item;

		//if (layer->properties.Get("Navigation", 0) == 0)
			//continue;

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

bool j1Map::LoadObjectGroupDetails(pugi::xml_node& objectGroup_node, ObjectGroup* objectGroup)
{
	bool ret = true;
	objectGroup->name.assign(objectGroup_node.attribute("name").as_string());

	return ret;
}

bool j1Map::LoadObject(pugi::xml_node& object_node, Object* object)
{
	bool ret = true;

	object->name = object_node.attribute("name").as_string();
	object->id = object_node.attribute("id").as_uint();
	object->width = object_node.attribute("width").as_uint();
	object->height = object_node.attribute("height").as_uint();
	object->x = object_node.attribute("x").as_uint();
	object->y = object_node.attribute("y").as_uint();
	object->type = object_node.attribute("type").as_uint();

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