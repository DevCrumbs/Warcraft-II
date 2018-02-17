#ifndef __Player_H__
#define __Player_H__

#include "Entity.h"
#include "Animation.h"

#define GRAVITY 125
#define P_GRAVITY 90
#define P_NORMAL_SPEED 125
#define P_JUMP_SPEED 60
#define P_JUMP 100
#define P_ATTACK_SPEED 10
#define P_ATTACK_OFFSET_X 35
#define P_ATTACK_OFFSET_Y 20
#define CULING_OFFSET 50
#define CROUCH_SHOT_X 5
#define CROUCH_SHOT_Y 38
#define SHOT_X 5
#define SHOT_Y 22

enum playerstates {
	null_,
	stop_,
	idle_, 
	forward_
};

struct PlayerInfo
{
	Animation idle;

	iPoint collSize = { 0,0 };
	SDL_Rect coll_offset = { 0,0,0,0 };
	uint check_collision_offset = 0;

	playerstates state = null_;
	float gravity = 0.0f;
	fPoint speed = { 0,0 };

	PlayerInfo();
	PlayerInfo(const PlayerInfo& i);
	~PlayerInfo();

	void SetState(playerstates);
	playerstates GetState() { return state; }
};

class Player : public Entity
{

public:
	Player(float x, float y);
	void OnCollision(Collider* c1, Collider* c2);
	void Move(float dt);
	void UpdateAnimations(float dt);
	void LoadAnimationsSpeed();

	void PlayerStateMachine();

	void CheckCollision(iPoint position, iPoint size, int offset, bool &up, bool &down, bool &left, bool &right, playerstates state = null_);
	void CalculateCollision(iPoint position, iPoint size, uint x, uint y, uint id, int offset, bool &up, bool &down, bool &left, bool &right, playerstates state = null_);

public:

	// General info
	playerstates default_state = null_;
	Animation* animationPlayer = nullptr;

private:

	// Movement
	bool up = true, down = true, left = true, right = true, gravitySpeed = false;

	// Animations speed
	float idle_speed = 0.0f;

public:
	PlayerInfo player;
};

#endif //__Player_H__
