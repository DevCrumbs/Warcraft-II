#include "Defs.h"
#include "p2Log.h"

#include "Runestone.h"

#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Movement.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "j1Printer.h"

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

	// Colliders
	CreateEntityCollider(EntitySide_Neutral);
	sightRadiusCollider = CreateRhombusCollider(ColliderType_NeutralBuilding, runestoneInfo.sightRadius, DistanceHeuristic_DistanceManhattan);
	sightRadiusCollider->isTrigger = true;
	entityCollider->isTrigger = true;
}

void Runestone::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(entityEvent);

	if (buildingState == BuildingState_Destroyed && !isUnitHealingArea)
		texArea = &runestoneInfo.inProgressTexArea;
}

// Heal area
bool Runestone::IsUnitHealingArea() const
{
	return isUnitHealingArea;
}

void Runestone::SetUnitHealingArea(bool isUnitHealingArea)
{
	this->isUnitHealingArea = isUnitHealingArea;
}

// Tex area
void Runestone::SwapTexArea()
{
	if (texArea == &runestoneInfo.completeTexArea)
		texArea = &runestoneInfo.inProgressTexArea;
	else
		texArea = &runestoneInfo.completeTexArea;
}

// Blit alpha area
void Runestone::BlitSightArea(Uint8 alpha)
{
	if (sightRadiusCollider == nullptr)
		return;

	for (uint i = 0; i < sightRadiusCollider->colliders.size(); ++i) {

		iPoint colliderTile = App->map->WorldToMap(sightRadiusCollider->colliders[i]->colliderRect.x, sightRadiusCollider->colliders[i]->colliderRect.y);
		Entity* entity = App->entities->IsEntityOnTile(colliderTile, EntityCategory_DYNAMIC_ENTITY, EntitySide_Player);

		if (entity != nullptr) {

			DynamicEntity* dynEnt = (DynamicEntity*)entity;
			iPoint pos = App->map->MapToWorld(dynEnt->GetSingleUnit()->currTile.x, dynEnt->GetSingleUnit()->currTile.y);

			SDL_Color lightBlue = ColorLightBlue;
			App->printer->PrintQuad({ pos.x, pos.y, 32, 32 }, { lightBlue.r,lightBlue.g,lightBlue.b,alpha }, Layers_FloorColliders);
		}
		else
			App->printer->PrintQuad(sightRadiusCollider->colliders[i]->colliderRect, { 255,255,255,alpha }, Layers_FloorColliders);
	}
}

// Runestone state
void Runestone::SetRunestoneState(RunestoneState runestoneState) 
{
	this->runestoneState = runestoneState;
}

RunestoneState Runestone::GetRunestoneState() const 
{
	return runestoneState;
}