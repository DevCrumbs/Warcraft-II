#include "TrollLumberMill.h"

TrollLumberMill::TrollLumberMill(fPoint pos, iPoint size, int currLife, uint maxLife, const TrollLumberMillInfo& trollLumberMillInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), trollLumberMillInfo(trollLumberMillInfo)
{
	texArea = &trollLumberMillInfo.completeTexArea;
}

void TrollLumberMill::Move(float dt) {


}

// Animations
void TrollLumberMill::LoadAnimationsSpeed()
{

}
void TrollLumberMill::UpdateAnimations(float dt)
{

}



