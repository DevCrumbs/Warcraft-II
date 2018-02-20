#ifndef __Entity_H__
#define __Entity_H__

#include "p2Point.h"
#include "Animation.h"

struct SDL_Texture;
struct Collider;

class Entity
{
public:

	Entity(float x, float y);
	virtual ~Entity();

	const Collider* GetCollider() const;

	virtual void Move(float dt) {};
	virtual void LoadAnimationsSpeed() {};
	virtual void UpdateAnimations(float dt) {};
	virtual void Draw(SDL_Texture* sprites);
	virtual void OnCollision(Collider* collider, Collider* c2);

protected:

	Collider* collider = nullptr;
	Animation* animation = nullptr;
	float deltaTime = 0.0f;

public:

	int type = 0;
	int lives = 0;
	bool remove = false;

	// Entity movement
	fPoint position = { 0,0 };
	iPoint iPos = { 0,0 };
	fPoint startPos = { 0,0 };

	fPoint speed = { 0,0 };

	fPoint lastPos = { 0,0 };
	bool up = false, down = false, left = false, right = false;
	//_enemy_movement

	// Particles
	iPoint colliderPos = { 0,0 };
	iPoint colliderSize = { 0,0 };


};

#endif //__Entity_H__