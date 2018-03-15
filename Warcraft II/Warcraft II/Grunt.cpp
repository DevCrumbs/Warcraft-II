#include "Grunt.h"

Grunt::Grunt(fPoint pos, iPoint size, int life, float speed, const GruntInfo& gruntInfo, j1Module* listener) :DynamicEntity(pos, size, life, speed, listener), gruntInfo(gruntInfo)
{

}

void Grunt::Move(float dt)
{

}

// Animations
void Grunt::LoadAnimationsSpeed()
{

}
void Grunt::UpdateAnimations(float dt)
{

}