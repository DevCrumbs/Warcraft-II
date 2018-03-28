// Creates the Footman and the Elven Archer
// Upgrades to Barracks 2: creates the Paladin (needed Stables created) and the Ballista

#include "Barracks.h"
#include "j1Player.h"

Barracks::Barracks(fPoint pos, iPoint size, int maxLife, const BarracksInfo& barracksInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), barracksInfo(barracksInfo)
{
	texArea = &barracksInfo.barracksCompleteTexArea;
	currentLife = maxLife;
	isBuilt = true;
	buildingState = BuildingState_Normal;
	//this->constructionTimer.Start();
}

void Barracks::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(entityEvent);

	if (App->player->barracksUpgrade) {
		if (startTimer) {
			this->constructionTimer.Start();
			startTimer = false;
		}
		UpdateAnimations(dt);
		barracksInfo.barracksType = BarracksType_Barracks2;
	}
	
	//if (constructionTimer.Read() >= (constructionTime * 1000))
	//	isBuilt = true;
	
}

// Animations
void Barracks::LoadAnimationsSpeed()
{

}
void Barracks::UpdateAnimations(float dt)
{


	if (constructionTimer.Read() >= constructionTime * 1000) {
		if (barracksInfo.barracksType == BarracksType_Barracks2) {
			texArea = &barracksInfo.barracks2CompleteTexArea;
			buildingState = BuildingState_Normal;
			SetMaxLife(1200);
			SetCurrLife(1200);
			//entityEvent = EntitiesEvent_CREATED;
		}
	}
	else {
		texArea = &barracksInfo.constructionPlanks2;
		buildingState = BuildingState_Building;
	}

}