#include "TempleOfTheDamned.h"

TempleOfTheDamned::TempleOfTheDamned(fPoint pos, iPoint size, int currLife, uint maxLife, const TempleOfTheDamnedInfo& templeOfTheDamnedInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), templeOfTheDamnedInfo(templeOfTheDamnedInfo)
{
texArea = &templeOfTheDamnedInfo.completeTexArea;
}

void TempleOfTheDamned::Move(float dt) {


}

// Animations
void TempleOfTheDamned::LoadAnimationsSpeed()
{

}
void TempleOfTheDamned::UpdateAnimations(float dt)
{

}



