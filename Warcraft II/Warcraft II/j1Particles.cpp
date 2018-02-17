#include "p2Log.h"

#include "j1App.h"

#include "j1Render.h"
#include "j1Textures.h"
#include "j1Particles.h"
#include "j1Collision.h"
#include "j1Audio.h"
#include "j1EntityFactory.h"
#include "j1Scene.h"
#include "j1Map.h"

#include <math.h>

#include "SDL/include/SDL_timer.h"

j1Particles::j1Particles()
{
	name.assign("particles");

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
		active[i] = nullptr;
}

j1Particles::~j1Particles()
{}

// Called before render is available
bool j1Particles::Awake(pugi::xml_node& config) {

	bool ret = true;

	/*
	//Sparkle
	node = animations_node.child("sparkle");
	for (node = node.child("frame"); node; node = node.next_sibling("frame")) {
		sparkle.anim.PushBack({ node.attribute("x").as_int(), node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() });
	}
	node = animations_node.child("sparkle");
	sparkle.life = node.attribute("life").as_uint();
	sparkle.anim.speed = node.attribute("speed").as_float();
	sparkle.anim.loop = node.attribute("loop").as_bool();
	node = node.child("frame");
	sparkle.collisionSize = { node.attribute("w").as_int(), node.attribute("h").as_int() };

	LoadAnimationsSpeed();
	*/

	return ret;
}

void j1Particles::LoadAnimationsSpeed()
{
	/*
	sparkle_speed = sparkle.anim.speed;
	*/
}

// Load assets
bool j1Particles::Start()
{
	bool ret = true;

	LOG("Loading particles");

	return ret;
}

// Unload assets
bool j1Particles::CleanUp()
{
	LOG("Unloading particles");

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] != nullptr)
		{
			delete active[i];
			active[i] = nullptr;
		}
	}

	return true;
}

// Update: draw background
bool j1Particles::Update(float dt)
{
	bool ret = true;

	UpdateAnimations(dt);

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		Particle* p = active[i];

		if (p == nullptr)
			continue;

		if (!p->Update(dt))
		{
			delete p;
			active[i] = nullptr;
		}
		else if (SDL_GetTicks() >= p->born)
		{
			/*
			if (p->collider->type == COLLIDER_TYPE::COLLIDER_CATPEASANT_SHOT)
				App->render->Blit(App->entities->CatPeasantTex, p->position.x, p->position.y, &(p->anim.GetCurrentFrame()));
			else if (p->collider->type == COLLIDER_TYPE::COLLIDER_IMP_BOMB || p->collider->type == COLLIDER_TYPE::COLLIDER_IMP_BOMB_EXPLOSION)
				App->render->Blit(App->entities->ImpTex, p->position.x, p->position.y, &(p->anim.GetCurrentFrame()));
			else
				App->render->Blit(App->entities->PlayerTex, p->position.x, p->position.y, &(p->anim.GetCurrentFrame()));
			*/
		}
	}

	return ret;
}

void j1Particles::UpdateAnimations(const float dt)
{
	/*
	sparkle.anim.speed = sparkle_speed * dt;
	*/
}

void j1Particles::AddParticle(const Particle& particle, int x, int y, COLLIDER_TYPE colliderType, Uint32 delay, fPoint speed)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] == nullptr)
		{
			Particle* p = new Particle(particle);
			p->born = SDL_GetTicks() + delay;
			p->position.x = x;
			p->position.y = y;
			p->speed.x = speed.x;
			p->speed.y = speed.y;

			/*
			if (colliderType != COLLIDER_NONE && colliderType != COLLIDER_CATPEASANT_SHOT && colliderType != COLLIDER_MONKEY_HIT)
				p->collider = App->collision->AddCollider(p->anim.GetCurrentFrame(), colliderType, this);
			else if (colliderType == COLLIDER_CATPEASANT_SHOT) {
				if (App->entities->playerData != nullptr) {

					// Shot towards something
					float m = sqrtf(pow(App->entities->playerData->position.x - p->position.x, 2.0f) + pow(App->entities->playerData->position.y - p->position.y, 2.0f));
					p->destination.x = (App->entities->playerData->position.x - p->position.x) / m;
					p->destination.y = (App->entities->playerData->position.y - p->position.y) / m;
				}

				p->collider = App->collision->AddCollider({ 0, 0, p->collisionSize.x - 40, p->collisionSize.y - 40 }, colliderType, this);
			}
			else if (colliderType == COLLIDER_MONKEY_HIT)
				p->collider = App->collision->AddCollider({ 0, 0, p->collisionSize.x, p->collisionSize.y }, colliderType, this);
			*/

			active[i] = p;
			break;
		}
	}
}

void j1Particles::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		// Always destroy particles that collide
		if (active[i] != nullptr && active[i]->collider == c1)
		{
			/*
			if ((c1->type == COLLIDER_ARROW && c2->type == COLLIDER_IMP) || (c1->type == COLLIDER_ARROW && c2->type == COLLIDER_IMP)
				|| (c1->type == COLLIDER_ARROW && c2->type == COLLIDER_CATPEASANT) || (c1->type == COLLIDER_ARROW && c2->type == COLLIDER_CATPEASANT)
				|| (c1->type == COLLIDER_ARROW && c2->type == COLLIDER_MONKEY) || (c1->type == COLLIDER_ARROW && c2->type == COLLIDER_MONKEY)
				|| (c1->type == COLLIDER_ARROW && c2->type == COLLIDER_CAT) || (c1->type == COLLIDER_ARROW && c2->type == COLLIDER_CAT)) {
			*/
				delete active[i];
				active[i] = nullptr;
				break;
			/*
			}
			*/
		}
	}
}

// -------------------------------------------------------------
// -------------------------------------------------------------

Particle::Particle()
{
	position.SetToZero();
	speed.SetToZero();
}

Particle::Particle(const Particle& p) :
	anim(p.anim), position(p.position), speed(p.speed),
	fx(p.fx), born(p.born), life(p.life), collisionSize(p.collisionSize)
{}

Particle::~Particle()
{
	if (collider != nullptr)
		collider->toDelete = true;
}

bool Particle::Update(float dt)
{
	bool ret = true;

	if (life > 0)
	{
		if ((SDL_GetTicks() - born) > life)
			ret = false;
	}
	else
		if (anim.Finished() || life == 0)
			ret = false;

	/*
	if (collider->type == COLLIDER_ARROW) {
		if (left || right)
			position.x += speed.x * dt;
		else
			position.x = position.x;
	}
	else if (collider->type == COLLIDER_CATPEASANT_SHOT) {
		position.y += destination.y * dt * speed.x;
		position.x += destination.x * dt * speed.y;
	}

	if (collider != nullptr)
		collider->SetPos(position.x, position.y);
	*/

	return ret;
}

