#include "Runestone.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"

Runestone::Runestone(fPoint pos, iPoint size, int currLife, uint maxLife, const RunestoneInfo& runestoneInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), runestoneInfo(runestoneInfo)
{
	buildingSize = Small;

	iPoint buildingTile = App->map->WorldToMap(pos.x, pos.y);
	App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;

	texArea = &runestoneInfo.completeTexArea;
	isBuilt = true;
}

void Runestone::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(entityEvent);

	if (buildingState == BuildingState_Destroyed)
		texArea = &runestoneInfo.inProgressTexArea;
}

// Animations
void Runestone::LoadAnimationsSpeed()
{

}
void Runestone::UpdateAnimations(float dt)
{
	//buildingState = BuildingState_Normal;
	//texArea = &runestoneInfo.inProgressTexArea;
}