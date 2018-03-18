// Creates the Footman and the Elven Archer
// Upgrades to Barracks 2: creates the Paladin (needed Stables created) and the Ballista

#include "Barracks.h"

Barracks::Barracks(fPoint pos, iPoint size, int maxLife, const BarracksInfo& barracksInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), barracksInfo(barracksInfo)
{
	texArea = &barracksInfo.constructionPlanks1;
	currentLife = maxLife;
	this->constructionTimer.Start();
}

void Barracks::Move(float dt)
{
	UpdateAnimations(dt);

	if (constructionTimer.Read() >= (constructionTime * 1000))
		isBuilt = true;
}

// Animations
void Barracks::LoadAnimationsSpeed()
{

}
void Barracks::UpdateAnimations(float dt)
{

	if (constructionTimer.Read() >= (constructionTime / 2) * 1000)
		texArea = &barracksInfo.constructionPlanks2;

	if (barracksInfo.barracksType == BarracksType_Barracks) {
		if (constructionTimer.Read() >= constructionTime * 1000)
			texArea = &barracksInfo.barracksCompleteTexArea;
	}

	else if (barracksInfo.barracksType == BarracksType_Barracks2) {
		if (constructionTimer.Read() >= constructionTime * 1000)
			texArea = &barracksInfo.barracks2CompleteTexArea;
	}

}