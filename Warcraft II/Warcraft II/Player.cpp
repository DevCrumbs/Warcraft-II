
#include <math.h>

#include "SDL/include/SDL_timer.h"

#include "Brofiler\Brofiler.h"


#include "j1App.h"
#include "p2Log.h"
#include "j1EntityFactory.h"
#include "Player.h"

#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1Collision.h"
#include "j1Audio.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1Particles.h"


Player::Player(float x, float y) : Entity(x, y)
{
	/*
	player = App->entities->GetPlayerInfo();

	LoadAnimationsSpeed();

	// Set player animation (state)
	player.SetState(PLAYERSTATE_IDLE);

	// Create player collider
	position = { 600, 100 };

	colliderPos = { (int)position.x + player.collisionOffset.x, (int)position.y + player.collisionOffset.y };
	collider = App->collision->AddCollider({ colliderPos.x, colliderPos.y, player.collisionSize.x + player.collisionOffset.w, player.collisionSize.y + player.collisionOffset.h }, COLLIDER_PLAYER, App->entities);
	*/
}

void Player::LoadAnimationsSpeed()
{
	/*
	idleSpeed = player.idle.speed;
	*/
}

void Player::Move(float dt)
{
	/*
	this->dt = dt;

	iPos.x = (int)position.x;
	iPos.y = (int)position.y;

	GodMode();

	player.gravity = GRAVITY * dt;

	if (App->fade->GetStep() == FADE_STEP::FADE_STEP_TO_BLACK || App->fade->GetStep() == FADE_STEP::FADE_STEP_FROM_BLACK)
		player.gravity = 0;

	// Check for collisions
	up = true;
	down = true;
	left = true;
	right = true;
	CheckCollision(colliderPos, { player.collisionSize.x + player.collisionOffset.w, player.collisionSize.y + player.collisionOffset.h }, player.checkCollisionOffset, up, down, left, right, player.GetState());

	// Update state
	if (!App->scene->pause)
		PlayerStateMachine();

	ApplySpeed();

	// Update animations speed
	UpdateAnimations(dt);

	// Update collider
	colliderPos = { (int)position.x + player.collisionOffset.x, (int)position.y + player.collisionOffset.y };
	collider->SetPos(colliderPos.x, colliderPos.y);

	animationPlayer = animation;
	*/

}

void Player::UpdateAnimations(float dt)
{
	/*
	player.idle.speed = idleSpeed * dt;
	*/
}

void Player::OnCollision(Collider* c1, Collider* c2) 
{
}

void PlayerInfo::SetState(PLAYERSTATE state) {
	this->state = state;
}

void Player::PlayerStateMachine() {

	switch (player.state) {

	case PLAYERSTATE_IDLE:

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT && right) {
			player.state = PLAYERSTATE_FORWARD;
			break;

			animation = &player.idle;

			break;

	case PLAYERSTATE_FORWARD:

		break;

	default:
		break;

		}
	}
}

void Player::CheckCollision(iPoint position, iPoint size, int offset, bool &up, bool &down, bool &left, bool &right, PLAYERSTATE state) {

	BROFILER_CATEGORY("CheckForCollision", Profiler::Color::Azure);

	App->map->culingOffset = CULING_OFFSET;

	for (int i = position.x - App->map->culingOffset; i < position.x + App->map->culingOffset; i++) {
		for (int j = position.y - App->map->culingOffset; j < position.y + App->map->culingOffset; j++) {

			iPoint ij = App->map->WorldToMap(i, j);

			uint id = App->map->collisionLayer->Get(ij.x, ij.y);

			if (id != 0) {
				iPoint world = App->map->MapToWorld(ij.x, ij.y);
				CalculateCollision(position, size, world.x, world.y, id, offset, up, down, left, right, state);
			}
		}
	}
}

void Player::CalculateCollision(iPoint position, iPoint size, uint x, uint y, uint id, int offset, bool &up, bool &down, bool &left, bool &right, PLAYERSTATE state) {

	if (App->toCap && App->capFrames <= 30) {
		offset = 6;
	}

	if (App->toCap && App->capFrames <= 10) {
		offset = 15;
	}

	SDL_Rect B = { x, y, 16, 16 }; //object rectangle

	iPoint cUp = { 0, -offset };
	iPoint cDown = { 0, offset };
	iPoint cLeft = { -offset, 0 };
	iPoint cRight = { offset, 0 };

	/// TODO: polish collisions

	//UP
	SDL_Rect aUp = { position.x + cUp.x, position.y + cUp.y, size.x, size.y }; //player rectangle
	if (SDL_HasIntersection(&aUp, &B)) {
		/*
		if (id == 1181 || (id == 1182 && !App->scene->gate))
			up = false;
		else if (id == 1183 && state != PLAYERSTATE_NONE) {
			player.SetState(punished_);
			lava_dead = true;
		}
		*/
	}

	//DOWN
	SDL_Rect aDown = { position.x + cDown.x, position.y + cDown.y, size.x, size.y }; //player rectangle
	if (SDL_HasIntersection(&aDown, &B))
		if (id == 1181)
			down = false;
	
	//LEFT
	SDL_Rect aLeft = { position.x + cLeft.x, position.y + cLeft.y, size.x, size.y }; //player rectangle
	if (SDL_HasIntersection(&aLeft, &B))
		if (id == 1181)
			left = false;

	//RIGHT
	SDL_Rect aRight = { position.x + cRight.x, position.y + cRight.y, size.x, size.y }; //player rectangle
	if (SDL_HasIntersection(&aRight, &B))
		if (id == 1181)
			right = false;
}


// -------------------------------------------------------------
// -------------------------------------------------------------

PlayerInfo::PlayerInfo() {}

PlayerInfo::PlayerInfo(const PlayerInfo& i) :
	idle(i.idle),
	collisionSize(i.collisionSize), collisionOffset(i.collisionOffset),
	gravity(i.gravity), speed(i.speed), state(i.state),
	checkCollisionOffset(i.checkCollisionOffset)
{}

PlayerInfo::~PlayerInfo() {}

