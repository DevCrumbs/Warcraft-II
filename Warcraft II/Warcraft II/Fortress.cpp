#include "Fortress.h"

Fortress::Fortress(fPoint pos, iPoint size, int currLife, uint maxLife, const FortressInfo& fortressInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), fortressInfo(fortressInfo)
{
	texArea = &fortressInfo.completeTexArea;
}

void Fortress::Move(float dt) {


}

// Animations
void Fortress::LoadAnimationsSpeed()
{

}
void Fortress::UpdateAnimations(float dt)
{

}

