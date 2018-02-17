#include "j1App.h"
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

#include "Brofiler\Brofiler.h"

#include <math.h>

#include "SDL/include/SDL_timer.h"

Player::Player(float x, float y) : Entity(x, y)
{
	/*
	player = App->entities->GetPlayerInfo();

	LoadAnimationsSpeed();

	// Set player animation (state)
	player.SetState(idle_);

	// Create player collider
	position = { 600, 100 };

	collider_pos = { (int)position.x + player.coll_offset.x, (int)position.y + player.coll_offset.y };
	collider = App->collision->AddCollider({ collider_pos.x, collider_pos.y, player.collSize.x + player.coll_offset.w, player.collSize.y + player.coll_offset.h }, COLLIDER_PLAYER, App->entities);
	*/
}

void Player::LoadAnimationsSpeed()
{
	/*
	idle_speed = player.idle.speed;
	*/
}

void Player::Move(float dt)
{
	/*
	this->dt = dt;

	i_pos.x = (int)position.x;
	i_pos.y = (int)position.y;

	GodMode();

	player.gravity = GRAVITY * dt;

	if (App->fade->GetStep() == FADE_STEP::FADE_STEP_TO_BLACK || App->fade->GetStep() == FADE_STEP::FADE_STEP_FROM_BLACK)
		player.gravity = 0;

	// Check for collisions
	up = true;
	down = true;
	left = true;
	right = true;
	CheckCollision(collider_pos, { player.collSize.x + player.coll_offset.w, player.collSize.y + player.coll_offset.h }, player.check_collision_offset, up, down, left, right, player.GetState());

	// Update state
	if (!App->scene->pause)
		PlayerStateMachine();

	ApplySpeed();

	// Update animations speed
	UpdateAnimations(dt);

	// Update collider
	collider_pos = { (int)position.x + player.coll_offset.x, (int)position.y + player.coll_offset.y };
	collider->SetPos(collider_pos.x, collider_pos.y);

	animationPlayer = animation;
	*/
}

void Player::UpdateAnimations(float dt)
{
	/*
	player.idle.speed = idle_speed * dt;
	*/
}

void Player::OnCollision(Collider* c1, Collider* c2) 
{
}

void PlayerInfo::SetState(playerstates state) {
	this->state = state;
}

void Player::PlayerStateMachine() {

	switch (player.state) {

	case idle_:

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT && right) {
			player.state = forward_;
			break;

			animation = &player.idle;

			break;

	case forward_:

		break;
		}
	}
}

void Player::CheckCollision(iPoint position, iPoint size, int offset, bool &up, bool &down, bool &left, bool &right, playerstates state) {

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

void Player::CalculateCollision(iPoint position, iPoint size, uint x, uint y, uint id, int offset, bool &up, bool &down, bool &left, bool &right, playerstates state) {

	if (App->toCap && App->capFrames <= 30) {
		offset = 6;
	}

	if (App->toCap && App->capFrames <= 10) {
		offset = 15;
	}

	SDL_Rect B = { x, y, 16, 16 }; //object rectangle

	iPoint c_up = { 0, -offset };
	iPoint c_down = { 0, offset };
	iPoint c_left = { -offset, 0 };
	iPoint c_right = { offset, 0 };

	// TODO: polish collisions

	//UP
	SDL_Rect A_up = { position.x + c_up.x, position.y + c_up.y, size.x, size.y }; //player rectangle
	if (SDL_HasIntersection(&A_up, &B)) {
		/*
		if (id == 1181 || (id == 1182 && App->scene->gate == false))
			up = false;
		else if (id == 1183 && state != null_) {
			player.SetState(punished_);
			lava_dead = true;
		}
		*/
	}

	//DOWN
	SDL_Rect A_down = { position.x + c_down.x, position.y + c_down.y, size.x, size.y }; //player rectangle
	if (SDL_HasIntersection(&A_down, &B))
		if (id == 1181)
			down = false;
	
	//LEFT
	SDL_Rect A_left = { position.x + c_left.x, position.y + c_left.y, size.x, size.y }; //player rectangle
	if (SDL_HasIntersection(&A_left, &B))
		if (id == 1181)
			left = false;

	//RIGHT
	SDL_Rect A_right = { position.x + c_right.x, position.y + c_right.y, size.x, size.y }; //player rectangle
	if (SDL_HasIntersection(&A_right, &B))
		if (id == 1181)
			right = false;
}

// -------------------------------------------------------------
// -------------------------------------------------------------

PlayerInfo::PlayerInfo() {}

PlayerInfo::PlayerInfo(const PlayerInfo& i) :
	idle(i.idle),
	collSize(i.collSize), coll_offset(i.coll_offset),
	gravity(i.gravity), speed(i.speed), state(i.state),
	check_collision_offset(i.check_collision_offset)
{}

PlayerInfo::~PlayerInfo() {}