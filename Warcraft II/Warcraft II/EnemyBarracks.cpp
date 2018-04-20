#include "EnemyBarracks.h"
#include "j1Collision.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"

EnemyBarracks::EnemyBarracks(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyBarracksInfo& enemyBarracksInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), enemyBarracksInfo(enemyBarracksInfo)
{
	buildingSize = Medium;

	iPoint buildingTile = App->map->WorldToMap(pos.x, pos.y);
	App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y) + (buildingTile.x + 2)] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 2) + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 1)] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 2)] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 2)] = 0u;

	texArea = &enemyBarracksInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy);
	entityCollider->isTrigger = true;
}

void EnemyBarracks::Move(float dt) {


}

// Animations
void EnemyBarracks::LoadAnimationsSpeed()
{

}
void EnemyBarracks::UpdateAnimations(float dt)
{

}


