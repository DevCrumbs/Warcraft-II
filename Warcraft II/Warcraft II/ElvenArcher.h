#ifndef __ElvenArcher_H__
#define __ElvenArcher_H__

#include "DynamicEntity.h"

struct Particle;

struct ElvenArcherInfo
{
	UnitInfo unitInfo;

	Animation up, down, left, right;
	Animation upLeft, upRight, downLeft, downRight;
	Animation attackUp, attackDown, attackLeft, attackRight;
	Animation attackUpLeft, attackUpRight, attackDownLeft, attackDownRight;
	Animation deathUp, deathDown;

	float arrowSpeed = 0.0f;
};

class ElvenArcher :public DynamicEntity
{
public:

	ElvenArcher(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const ElvenArcherInfo& elvenArcherInfo, j1Module* listener);
	~ElvenArcher() {};
	void Move(float dt);
	void Draw(SDL_Texture* sprites);
	void DebugDrawSelected();
	void OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState);

	// State machine
	void UnitStateMachine(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimationsSpeed(float dt);
	bool ChangeAnimation();

	float GetArrowSpeed() const;

private:

	ElvenArcherInfo elvenArcherInfo;

	// Animations speed
	float upSpeed = 0.0f, downSpeed = 0.0f, leftSpeed = 0.0f, rightSpeed = 0.0f;
	float upLeftSpeed = 0.0f, upRightSpeed = 0.0f, downLeftSpeed = 0.0f, downRightSpeed = 0.0f;
	float attackUpSpeed = 0.0f, attackDownSpeed = 0.0f, attackLeftSpeed = 0.0f, attackRightSpeed = 0.0f;
	float attackUpLeftSpeed = 0.0f, attackUpRightSpeed = 0.0f, attackDownLeftSpeed = 0.0f, attackDownRightSpeed = 0.0f;
	float deathUpSpeed = 0.0f, deathDownSpeed = 0.0f;
};

#endif //__ElvenArcher_H__