#include "Grunt.h"

Grunt::Grunt(fPoint pos, const GruntInfo& gruntInfo, j1Module* listener) :DynamicEntity(pos, gruntInfo.size, gruntInfo.life, gruntInfo.speed, listener), gruntInfo(gruntInfo)
{
	currentLife = gruntInfo.life;

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