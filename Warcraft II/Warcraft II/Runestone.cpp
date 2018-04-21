#include "Defs.h"
#include "p2Log.h"

#include "Runestone.h"

#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Movement.h"

Runestone::Runestone(fPoint pos, iPoint size, int currLife, uint maxLife, const RunestoneInfo& runestoneInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), runestoneInfo(runestoneInfo)
{
	buildingSize = Small;

	// Update the walkability map (invalidate the tiles of the building placed)
	vector<iPoint> walkability;
	iPoint buildingTile = App->map->WorldToMap(pos.x, pos.y);
	App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
	walkability.push_back({ buildingTile.x, buildingTile.y });
	App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
	walkability.push_back({ buildingTile.x + 1, buildingTile.y });
	App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
	walkability.push_back({ buildingTile.x, buildingTile.y + 1 });
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
	walkability.push_back({ buildingTile.x + 1, buildingTile.y + 1 });
	App->movement->UpdateUnitsWalkability(walkability);
	// -----

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