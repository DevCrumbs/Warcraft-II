#include "Grunt.h"

Grunt::Grunt(fPoint pos, iPoint size, int life, float speed, const GruntInfo& gruntInfo) :DynamicEntity(pos, size, life, speed), gruntInfo(gruntInfo) 
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