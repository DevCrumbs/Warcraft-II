#include "Defs.h"
#include "p2Log.h"

#include "EnemyBlacksmith.h"

#include "j1Collision.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Movement.h"

EnemyBlacksmith::EnemyBlacksmith(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyBlacksmithInfo& enemyBlacksmithInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), enemyBlacksmithInfo(enemyBlacksmithInfo)
{
	// Update the walkability map (invalidate the tiles of the building placed)
	vector<iPoint> walkability;
	iPoint buildingTile = App->map->WorldToMap(pos.x, pos.y);
	walkability.push_back({ buildingTile.x, buildingTile.y });
	App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
	walkability.push_back({ buildingTile.x, buildingTile.y });
	App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
	walkability.push_back({ buildingTile.x + 1, buildingTile.y });
	App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
	walkability.push_back({ buildingTile.x, buildingTile.y + 1 });
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
	walkability.push_back({ buildingTile.x + 1, buildingTile.y + 1 });
	App->scene->data[App->scene->w * (buildingTile.y) + (buildingTile.x + 2)] = 0u;
	walkability.push_back({ buildingTile.x + 2, buildingTile.y });
	App->scene->data[App->scene->w * (buildingTile.y + 2) + buildingTile.x] = 0u;
	walkability.push_back({ buildingTile.x, buildingTile.y + 2 });
	App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 1)] = 0u;
	walkability.push_back({ buildingTile.x + 1, buildingTile.y + 2 });
	App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 2)] = 0u;
	walkability.push_back({ buildingTile.x + 2, buildingTile.y + 2 });
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 2)] = 0u;
	walkability.push_back({ buildingTile.x + 2, buildingTile.y + 1 });
	App->movement->UpdateUnitsWalkability(walkability);
	// -----

	texArea = &enemyBlacksmithInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy, true);
	entityCollider->isTrigger = true;
}

void EnemyBlacksmith::Move(float dt) {


}

// Animations
void EnemyBlacksmith::LoadAnimationsSpeed()
{

}
void EnemyBlacksmith::UpdateAnimations(float dt)
{

}