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

	
	//Fire
	pugi::xml_node spritesheets = config.child("spritesheets");
<<<<<<< HEAD
	particlesTexName = spritesheets.child("sprite").attribute("name").as_string();
=======
	atlasTexName = spritesheets.child("atlas").attribute("name").as_string();
>>>>>>> Develompent


	//spritesheets = config.child("fire");

	//Footman animations
	pugi::xml_node fireAnimation = config.child("fire");
	pugi::xml_node currentAnimation;

	//Buildings Fire
	currentAnimation = fireAnimation.child("low");
	lowFire.anim.speed = currentAnimation.attribute("speed").as_float();
	lowFire.anim.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		lowFire.anim.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	currentAnimation = fireAnimation.child("hard");
	hardFire.anim.speed = currentAnimation.attribute("speed").as_float();
	hardFire.anim.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		hardFire.anim.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	//Tower arrows
	pugi::xml_node towerArrows = config.child("towerArrows");
	towerArrowParticles.up.anim.PushBack({ towerArrows.child("up").attribute("x").as_int(), towerArrows.child("up").attribute("y").as_int(), towerArrows.child("up").attribute("w").as_int(), towerArrows.child("up").attribute("h").as_int() });
	towerArrowParticles.down.anim.PushBack({ towerArrows.child("down").attribute("x").as_int(), towerArrows.child("down").attribute("y").as_int(), towerArrows.child("down").attribute("w").as_int(), towerArrows.child("down").attribute("h").as_int() });
	towerArrowParticles.left.anim.PushBack({ towerArrows.child("left").attribute("x").as_int(), towerArrows.child("left").attribute("y").as_int(), towerArrows.child("left").attribute("w").as_int(), towerArrows.child("left").attribute("h").as_int() });
	towerArrowParticles.right.anim.PushBack({ towerArrows.child("right").attribute("x").as_int(), towerArrows.child("right").attribute("y").as_int(), towerArrows.child("right").attribute("w").as_int(), towerArrows.child("right").attribute("h").as_int() });
	towerArrowParticles.upLeft.anim.PushBack({ towerArrows.child("upLeft").attribute("x").as_int(), towerArrows.child("upLeft").attribute("y").as_int(), towerArrows.child("upLeft").attribute("w").as_int(), towerArrows.child("upLeft").attribute("h").as_int() });
	towerArrowParticles.upRight.anim.PushBack({ towerArrows.child("upRight").attribute("x").as_int(), towerArrows.child("upRight").attribute("y").as_int(), towerArrows.child("upRight").attribute("w").as_int(), towerArrows.child("upRight").attribute("h").as_int() });
	towerArrowParticles.downLeft.anim.PushBack({ towerArrows.child("downLeft").attribute("x").as_int(), towerArrows.child("downLeft").attribute("y").as_int(), towerArrows.child("downLeft").attribute("w").as_int(), towerArrows.child("downLeft").attribute("h").as_int() });
	towerArrowParticles.downRight.anim.PushBack({ towerArrows.child("downRight").attribute("x").as_int(), towerArrows.child("downRight").attribute("y").as_int(), towerArrows.child("downRight").attribute("w").as_int(), towerArrows.child("downRight").attribute("h").as_int() });

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

<<<<<<< HEAD
	fireText = App->tex->Load(particlesTexName.data());
=======
	atlasTex = App->tex->Load(atlasTexName.data());
>>>>>>> Develompent
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
			App->render->Blit(atlasTex, p->position.x, p->position.y, &(p->anim.GetCurrentFrame()));
			
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

Particle* j1Particles::AddParticle(const Particle& particle, int x, int y, ColliderType colliderType, Uint32 delay, fPoint speed)
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

			return p;
		}
	}
}

void j1Particles::OnCollision(Collider* c1, Collider* c2, CollisionState collisionState)
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
	// TODO (Sandra): delete the colliders
	/*
	if (collider != nullptr)
		collider->toDelete = true;
	*/
}

bool Particle::Update(float dt)
{
	bool ret = true;

	if (life = 0)
	{
		//if ((SDL_GetTicks() - born) > life)
		ret = false;
	}
	if (isDeleted) {
		ret = false;
	}

		/*if (anim.Finished() || life == 0)
			ret = false;*/

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

