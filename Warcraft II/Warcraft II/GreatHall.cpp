#include "GreatHall.h"

GreatHall::GreatHall(fPoint pos, iPoint size, int currLife, uint maxLife, const GreatHallInfo& greatHallInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), greatHallInfo(greatHallInfo)
{
	texArea = &greatHallInfo.completeTexArea;
}

void GreatHall::Move(float dt) {


}

// Animations
void GreatHall::LoadAnimationsSpeed()
{

}
void GreatHall::UpdateAnimations(float dt)
{

}

