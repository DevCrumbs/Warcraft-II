#include "Footman.h"

Footman::Footman(fPoint pos, iPoint size, int life, float speed, const FootmanInfo& footmanInfo, j1Module* listener) :DynamicEntity(pos, size, life, speed, listener),footmanInfo(footmanInfo)
{

}

void Footman::Move(float dt)
{

}

// Animations
void Footman::LoadAnimationsSpeed()
{

}
void Footman::UpdateAnimations(float dt)
{

}