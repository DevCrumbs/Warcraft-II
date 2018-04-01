#include "Runestone.h"

Runestone::Runestone(fPoint pos, iPoint size, int currLife, uint maxLife, const RunestoneInfo& runestoneInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), runestoneInfo(runestoneInfo)
{
	texArea = &runestoneInfo.completeTexArea;
	isBuilt = true;
}

void Runestone::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(entityEvent);

	if (buildingState == BuildingState_Destroyed) {
			UpdateAnimations(dt);
	}
}

// Animations
void Runestone::LoadAnimationsSpeed()
{

}
void Runestone::UpdateAnimations(float dt)
{
	buildingState = BuildingState_Normal;
	texArea = &runestoneInfo.inProgressTexArea;
}