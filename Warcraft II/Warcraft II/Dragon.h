#ifndef __Dragon_H__
#define __Dragon_H__

#include "DynamicEntity.h"

struct Collider;
struct ColliderGroup;

enum CollisionState;

struct DragonInfo
{
	UnitInfo unitInfo;

	Animation up, down, left, right;
	Animation upLeft, upRight, downLeft, downRight;
	Animation attackUp, attackDown, attackLeft, attackRight;
	Animation attackUpLeft, attackUpRight, attackDownLeft, attackDownRight;
	Animation deathUp, deathDown;

	float fireSpeed = 0.0f;
	uint droppedGold = 0;
};

class Dragon :public DynamicEntity
{
public:

	Dragon(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const DragonInfo& dragonInfo, j1Module* listener);
	~Dragon() {};
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

	float GetFireSpeed() const;

private:

	DragonInfo dragonInfo;

	// Animations speed
	float upSpeed = 0.0f, downSpeed = 0.0f, leftSpeed = 0.0f, rightSpeed = 0.0f;
	float upLeftSpeed = 0.0f, upRightSpeed = 0.0f, downLeftSpeed = 0.0f, downRightSpeed = 0.0f;
	float attackUpSpeed = 0.0f, attackDownSpeed = 0.0f, attackLeftSpeed = 0.0f, attackRightSpeed = 0.0f;
	float attackUpLeftSpeed = 0.0f, attackUpRightSpeed = 0.0f, attackDownLeftSpeed = 0.0f, attackDownRightSpeed = 0.0f;
	float deathUpSpeed = 0.0f, deathDownSpeed = 0.0f;
};

#endif //__Dragon_H__