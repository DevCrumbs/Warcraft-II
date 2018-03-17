// Creates the Footman and the Elven Archer
// Upgrades to Barracks 2: creates the Paladin (needed Stables created) and the Ballista

#include "Barracks.h"

Barracks::Barracks(fPoint pos, iPoint size, int maxLife, const BarracksInfo& barracksInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), barracksInfo(barracksInfo)
{
	texArea = &barracksInfo.barracksCompleteTexArea;
	currentLife = maxLife;

}

void Barracks::Move(float dt)
{

}

// Animations
void Barracks::LoadAnimationsSpeed()
{

}
void Barracks::UpdateAnimations(float dt)
{

}