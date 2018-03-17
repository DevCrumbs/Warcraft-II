#include "Grunt.h"

Grunt::Grunt(fPoint pos, iPoint size, int maxLife, float speed, const GruntInfo& gruntInfo, j1Module* listener) :DynamicEntity(pos, size, maxLife, speed, listener), gruntInfo(gruntInfo)
{
	currentLife = maxLife;

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