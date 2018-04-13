#include "ChickenFarm.h"
#include "j1Player.h"
#include "j1Scene.h"


ChickenFarm::ChickenFarm(fPoint pos, iPoint size, int currLife, uint maxLife, const ChickenFarmInfo& chickenFarmInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), chickenFarmInfo(chickenFarmInfo)
{
	isBuilt = chickenFarmInfo.isBuilt;

	if (isBuilt)
		texArea = &chickenFarmInfo.completeTexArea;
	else if (!isBuilt) {
		texArea = &chickenFarmInfo.constructionPlanks1;
		this->constructionTimer.Start();
		App->audio->PlayFx(2, 0); //Construction sound
	}
}

void ChickenFarm::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);

	if(!isBuilt)
		UpdateAnimations(dt);
	
	if (constructionTimer.Read() >= (constructionTime * 1000) && isBuilt == false) {
		isBuilt = true;
		App->player->currentFood += 4;
		App->scene->hasFoodChanged = true;

	}
}

// Animations
void ChickenFarm::LoadAnimationsSpeed()
{

}
void ChickenFarm::UpdateAnimations(float dt)
{
		if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
			texArea = &chickenFarmInfo.constructionPlanks2;

		if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
			texArea = &chickenFarmInfo.inProgressTexArea;

		if (constructionTimer.Read() >= constructionTime * 1000)
			texArea = &chickenFarmInfo.completeTexArea;

}
