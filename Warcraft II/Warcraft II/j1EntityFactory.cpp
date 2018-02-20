#include "Defs.h"
#include "p2Log.h"

#include "j1Module.h"
#include "j1App.h"

#include "j1EntityFactory.h"
#include "j1Render.h"

#include "j1Textures.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Collision.h"

#include "Entity.h"

j1EntityFactory::j1EntityFactory()
{
	name.assign("entities");
}

// Destructor
j1EntityFactory::~j1EntityFactory()
{
}

bool j1EntityFactory::Awake(pugi::xml_node& config) {

	bool ret = true;

	pugi::xml_node spritesheets = config.child("spritesheets");

	footmanTexName = spritesheets.child("footman").attribute("name").as_string();

	return ret;
}

bool j1EntityFactory::Start()
{
	bool ret = true;

	LOG("Loading entities textures");

	footmanTex = App->tex->Load(footmanTexName.data());

	return ret;
}

bool j1EntityFactory::PreUpdate()
{
	bool ret = true;

	// Spawn entities
	list<Entity*>::const_iterator it = toSpawnEntities.begin();

	while (it != toSpawnEntities.end()) {

		// Move the entity from the waiting list to the active list
		activeEntities.push_back(*it);

		/*
		int x = (*it)->entityInfo.pos.x * App->scene->scale;
		int y = (*it)->entityInfo.pos.y * App->scene->scale;
		App->map->WorldToMap(x, y);
		LOG("Spawning entity at tile %d,%d", x, y);
		*/
		it++;
	}
	toSpawnEntities.clear();

	return ret;
}

// Called before render is available
bool j1EntityFactory::Update(float dt)
{
	bool ret = true;

	// Update active entities
	list<Entity*>::const_iterator it = activeEntities.begin();

	while (it != activeEntities.end()) {
		(*it)->Move(dt);
		it++;
	}

	// TODO: redefine blit order

	// Draw map
	//App->map->Draw();

	// Draw entities
	/*
	for (uint i = 0; i < MAX_ENTITIES; ++i)
	if (entities[i] != nullptr) {

	if (entities[i]->type == EntityType_Unit)
	entities[i]->Draw(CatPeasantTex);

	}
	*/

	// Draw above layer
	//App->map->DrawAboveLayer();

	return ret;
}

void j1EntityFactory::Draw()
{
	// Blit active entities
	list<Entity*>::const_iterator it = activeEntities.begin();

	while (it != activeEntities.end()) {

		switch ((*it)->type) {
		/*
		case EntityType_Unit:
			(*it)->Draw(footmanTex);
			break;
		*/
		}
		it++;
	}
}

list<Entity*> j1EntityFactory::SelectEntitiesWithinRectangle(SDL_Rect rectangleRect)
{

	list<Entity*>::const_iterator it = activeEntities.begin();
	/*
	while (it != activeEntities.end()) {

		SDL_Rect entityRect = { (*it)->entityInfo.pos.x, (*it)->entityInfo.pos.y, (*it)->entityInfo.size.x, (*it)->entityInfo.size.y };

		// If the entity is within the selection:
		if (SDL_HasIntersection(&entityRect, &rectangleRect)) {

			// It there are less entities than MAX_ENTITIES_SELECTED selected:
			if (unitsSelected.size() < MAX_ENTITIES_SELECTED) {

				// If the entity isn't in the unitsSelected list, add it
				if (find(unitsSelected.begin(), unitsSelected.end(), *it) == unitsSelected.end()) {
					unitsSelected.push_back(*it);
					(*it)->isSelected = true;
				}
			}
		}
		else {

			// If the entity is in the unitsSelected list, remove it
			if (find(unitsSelected.begin(), unitsSelected.end(), *it) != unitsSelected.end()) {
				unitsSelected.remove(*it);
				(*it)->isSelected = false;
			}
		}
		it++;
	}
	*/

	return unitsSelected;
}

bool j1EntityFactory::IsAnotherEntityOnTile(Entity* entity, iPoint tile) const
{
	/*
	list<Entity*>::const_iterator it = activeEntities.begin();

	while (it != activeEntities.end()) {

		if (*it != entity) {
			iPoint entityTile = { (int)(*it)->entityInfo.pos.x, (int)(*it)->entityInfo.pos.y };
			entityTile = App->map->WorldToMap(entityTile.x, entityTile.y);

			if (entityTile == tile)
				return true;
		}
		it++;
	}
	*/

	return false;
}

iPoint j1EntityFactory::FindClosestWalkableTile(Entity* entity, iPoint tile) const
{
	iPoint currTile = { -1,-1 };
	vector<iPoint> visitedTiles;

	visitedTiles.push_back(tile);

	while (visitedTiles.size() > 0) {

		currTile = visitedTiles.front();
		visitedTiles.erase(visitedTiles.begin());

		if (App->pathfinding->IsWalkable(currTile) && !IsAnotherEntityOnTile(entity, currTile))
			return currTile;

		iPoint neighbors[4];
		neighbors[0].create(currTile.x + 1, currTile.y + 0);
		neighbors[1].create(currTile.x + 0, currTile.y + 1);
		neighbors[2].create(currTile.x - 1, currTile.y + 0);
		neighbors[3].create(currTile.x + 0, currTile.y - 1);

		for (uint i = 0; i < 4; ++i)
		{
			if (find(visitedTiles.begin(), visitedTiles.end(), neighbors[i]) == visitedTiles.end())
				visitedTiles.push_back(neighbors[i]);
		}
	}

	return currTile;
}

bool j1EntityFactory::PostUpdate()
{
	bool ret = true;

	// Remove entities
	list<Entity*>::const_iterator it = activeEntities.begin();

	while (it != activeEntities.end()) {
		if ((*it)->remove) {
			delete *it;
			activeEntities.remove(*it);
		}
		it++;
	}

	return ret;
}

// Called before quitting
bool j1EntityFactory::CleanUp()
{
	bool ret = true;

	LOG("Freeing all entities");

	// Remove all entities
	list<Entity*>::const_iterator it = activeEntities.begin();

	while (it != activeEntities.end()) {
		delete *it;
		it++;
	}
	activeEntities.clear();

	it = toSpawnEntities.begin();

	while (it != toSpawnEntities.end()) {
		delete *it;
		it++;
	}
	toSpawnEntities.clear();

	// Free all textures
	App->tex->UnLoad(footmanTex);

	return ret;
}

void j1EntityFactory::OnCollision(Collider* c1, Collider* c2)
{
	// Check for collisions

	list<Entity*>::const_iterator it = activeEntities.begin();

	while (it != activeEntities.end()) {
		if ((*it)->GetCollider() == c1) {
			(*it)->OnCollision(c1, c2);
			break;
		}
		it++;
	}
}

Unit* j1EntityFactory::AddUnit(const EntityInfo& entityInfo, const UnitInfo& unitInfo)
{
	/*
	Unit* unit = new Unit(entityInfo, unitInfo);
	toSpawnEntities.push_back(unit);

	return unit;
	*/

	return nullptr;
}

// -------------------------------------------------------------
// -------------------------------------------------------------

bool j1EntityFactory::Load(pugi::xml_node& save)
{
	bool ret = true;

	pugi::xml_node node;

	list<Entity*>::const_iterator it = activeEntities.begin();

	while (it != activeEntities.end()) {
		// MYTODO: Add some code here
		it++;
	}

	return ret;
}

bool j1EntityFactory::Save(pugi::xml_node& save) const
{
	bool ret = true;

	pugi::xml_node node;

	list<Entity*>::const_iterator it = activeEntities.begin();

	while (it != activeEntities.end()) {
		// MYTODO: Add some code here
		it++;
	}

	return ret;
}