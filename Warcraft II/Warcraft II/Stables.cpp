// Allows the creation of the Paladin

#include "Stables.h"

Stables::Stables(fPoint pos, iPoint size, int maxLife, const StablesInfo& stablesInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), stablesInfo(stablesInfo)
{
	texArea = &constructionPlanks.constructionPlanksBig1;                                               
	currentLife = maxLife;
	this->constructionTimer.Start();
}

void Stables::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);

	UpdateAnimations(dt);
}

// Animations
void Stables::LoadAnimationsSpeed()
{

}
void Stables::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
		texArea = &constructionPlanks.constructionPlanksBig2;

	if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
		texArea = &stablesInfo.inProgressTexArea;

	if (constructionTimer.Read() >= constructionTime * 1000)
		texArea = &stablesInfo.completeTexArea;
}