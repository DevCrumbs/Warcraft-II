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
		activeParticles[i] = nullptr;
}

j1Particles::~j1Particles()
{}

// Called before render is available
bool j1Particles::Awake(pugi::xml_node& config) {

	bool ret = true;

	// Load spritesheets
	pugi::xml_node spritesheets = config.child("spritesheets");
	pawsTexName = spritesheets.child("paws").attribute("name").as_string();
	atlasTexName = spritesheets.child("atlas").attribute("name").as_string();

	// Load animations

	// Fire
	pugi::xml_node fireAnimation = config.child("fire");
	pugi::xml_node currentAnimation;

	// Buildings Fire
	currentAnimation = fireAnimation.child("low");
	lowFire.animation.speed = currentAnimation.attribute("speed").as_float();
	lowFire.animation.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		lowFire.animation.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	currentAnimation = fireAnimation.child("hard");
	hardFire.animation.speed = currentAnimation.attribute("speed").as_float();
	hardFire.animation.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		hardFire.animation.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	// Tower arrows
	pugi::xml_node towerArrows = config.child("towerArrows");
	towerArrowParticles.up.animation.PushBack({ towerArrows.child("up").attribute("x").as_int(), towerArrows.child("up").attribute("y").as_int(), towerArrows.child("up").attribute("w").as_int(), towerArrows.child("up").attribute("h").as_int() });
	towerArrowParticles.down.animation.PushBack({ towerArrows.child("down").attribute("x").as_int(), towerArrows.child("down").attribute("y").as_int(), towerArrows.child("down").attribute("w").as_int(), towerArrows.child("down").attribute("h").as_int() });
	towerArrowParticles.left.animation.PushBack({ towerArrows.child("left").attribute("x").as_int(), towerArrows.child("left").attribute("y").as_int(), towerArrows.child("left").attribute("w").as_int(), towerArrows.child("left").attribute("h").as_int() });
	towerArrowParticles.right.animation.PushBack({ towerArrows.child("right").attribute("x").as_int(), towerArrows.child("right").attribute("y").as_int(), towerArrows.child("right").attribute("w").as_int(), towerArrows.child("right").attribute("h").as_int() });
	towerArrowParticles.upLeft.animation.PushBack({ towerArrows.child("upLeft").attribute("x").as_int(), towerArrows.child("upLeft").attribute("y").as_int(), towerArrows.child("upLeft").attribute("w").as_int(), towerArrows.child("upLeft").attribute("h").as_int() });
	towerArrowParticles.upRight.animation.PushBack({ towerArrows.child("upRight").attribute("x").as_int(), towerArrows.child("upRight").attribute("y").as_int(), towerArrows.child("upRight").attribute("w").as_int(), towerArrows.child("upRight").attribute("h").as_int() });
	towerArrowParticles.downLeft.animation.PushBack({ towerArrows.child("downLeft").attribute("x").as_int(), towerArrows.child("downLeft").attribute("y").as_int(), towerArrows.child("downLeft").attribute("w").as_int(), towerArrows.child("downLeft").attribute("h").as_int() });
	towerArrowParticles.downRight.animation.PushBack({ towerArrows.child("downRight").attribute("x").as_int(), towerArrows.child("downRight").attribute("y").as_int(), towerArrows.child("downRight").attribute("w").as_int(), towerArrows.child("downRight").attribute("h").as_int() });

	//Cannon from the cannon tower
	pugi::xml_node bulletsCannon = config.child("cannon");
	cannonBullet.animation.PushBack({ bulletsCannon.attribute("x").as_int(), bulletsCannon.attribute("y").as_int(), bulletsCannon.attribute("w").as_int(), bulletsCannon.attribute("h").as_int() });

	//Troll's axe
	pugi::xml_node trollAxeAnimation = config.child("trollAxe");
	trollAxe.animation.speed = trollAxeAnimation.attribute("speed").as_float();
	trollAxe.animation.loop = trollAxeAnimation.attribute("loop").as_bool();
	for (currentAnimation = trollAxeAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxe.animation.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	// Sheep Paws
	pugi::xml_node sheepPawsAnimation = config.child("animations").child("sheepPaws");

	// up
	currentAnimation = sheepPawsAnimation.child("up");
	sheepPawsInfo.up.speed = currentAnimation.attribute("speed").as_float();
	sheepPawsInfo.up.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		sheepPawsInfo.up.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// down
	currentAnimation = sheepPawsAnimation.child("down");
	sheepPawsInfo.down.speed = currentAnimation.attribute("speed").as_float();
	sheepPawsInfo.down.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		sheepPawsInfo.down.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// left
	currentAnimation = sheepPawsAnimation.child("left");
	sheepPawsInfo.left.speed = currentAnimation.attribute("speed").as_float();
	sheepPawsInfo.left.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		sheepPawsInfo.left.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// right
	currentAnimation = sheepPawsAnimation.child("right");
	sheepPawsInfo.right.speed = currentAnimation.attribute("speed").as_float();
	sheepPawsInfo.right.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		sheepPawsInfo.right.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// up-left
	currentAnimation = sheepPawsAnimation.child("upLeft");
	sheepPawsInfo.upLeft.speed = currentAnimation.attribute("speed").as_float();
	sheepPawsInfo.upLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		sheepPawsInfo.upLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// up-right
	currentAnimation = sheepPawsAnimation.child("upRight");
	sheepPawsInfo.upRight.speed = currentAnimation.attribute("speed").as_float();
	sheepPawsInfo.upRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		sheepPawsInfo.upRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// down-left
	currentAnimation = sheepPawsAnimation.child("downLeft");
	sheepPawsInfo.downLeft.speed = currentAnimation.attribute("speed").as_float();
	sheepPawsInfo.downLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		sheepPawsInfo.downLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// down-right
	currentAnimation = sheepPawsAnimation.child("downRight");
	sheepPawsInfo.downRight.speed = currentAnimation.attribute("speed").as_float();
	sheepPawsInfo.downRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		sheepPawsInfo.downRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	// Boar Paws
	pugi::xml_node boarPawsAnimations = config.child("animations").child("boarPaws");

	// up
	currentAnimation = boarPawsAnimations.child("up");
	boarPawsInfo.up.speed = currentAnimation.attribute("speed").as_float();
	boarPawsInfo.up.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		boarPawsInfo.up.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// down
	currentAnimation = boarPawsAnimations.child("down");
	boarPawsInfo.down.speed = currentAnimation.attribute("speed").as_float();
	boarPawsInfo.down.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		boarPawsInfo.down.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// left
	currentAnimation = boarPawsAnimations.child("left");
	boarPawsInfo.left.speed = currentAnimation.attribute("speed").as_float();
	boarPawsInfo.left.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		boarPawsInfo.left.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// right
	currentAnimation = boarPawsAnimations.child("right");
	boarPawsInfo.right.speed = currentAnimation.attribute("speed").as_float();
	boarPawsInfo.right.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		boarPawsInfo.right.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// up-left
	currentAnimation = boarPawsAnimations.child("upLeft");
	boarPawsInfo.upLeft.speed = currentAnimation.attribute("speed").as_float();
	boarPawsInfo.upLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		boarPawsInfo.upLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// up-right
	currentAnimation = boarPawsAnimations.child("upRight");
	boarPawsInfo.upRight.speed = currentAnimation.attribute("speed").as_float();
	boarPawsInfo.upRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		boarPawsInfo.upRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// down-left
	currentAnimation = boarPawsAnimations.child("downLeft");
	boarPawsInfo.downLeft.speed = currentAnimation.attribute("speed").as_float();
	boarPawsInfo.downLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		boarPawsInfo.downLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// down-right
	currentAnimation = boarPawsAnimations.child("downRight");
	boarPawsInfo.downRight.speed = currentAnimation.attribute("speed").as_float();
	boarPawsInfo.downRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		boarPawsInfo.downRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	return ret;
}

// Load assets
bool j1Particles::Start()
{
	bool ret = true;

	LOG("Loading particles");

	paws.particleType = ParticleType_Paws;
	towerArrowParticles.up.particleType = ParticleType_NoType;
	towerArrowParticles.down.particleType = ParticleType_NoType;
	towerArrowParticles.left.particleType = ParticleType_NoType;
	towerArrowParticles.right.particleType = ParticleType_NoType;
	towerArrowParticles.upLeft.particleType = ParticleType_NoType;
	towerArrowParticles.upRight.particleType = ParticleType_NoType;
	towerArrowParticles.downLeft.particleType = ParticleType_NoType;
	towerArrowParticles.downRight.particleType = ParticleType_NoType;

	towerArrowParticles.up.life = 800;
	towerArrowParticles.down.life = 800;
	towerArrowParticles.left.life = 800;
	towerArrowParticles.right.life = 800;
	towerArrowParticles.upLeft.life = 800;
	towerArrowParticles.upRight.life = 800;
	towerArrowParticles.downLeft.life = 800;
	towerArrowParticles.downRight.life = 800;
	cannonBullet.life = 800;
	trollAxe.life = 800;
	paws.life = 800;

	LoadAnimationsSpeed();

	atlasTex = App->tex->Load(atlasTexName.data());
	pawsTex = App->tex->Load(pawsTexName.data());

	return ret;
}

// Unload assets
bool j1Particles::CleanUp()
{
	LOG("Unloading particles");

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (activeParticles[i] != nullptr)
		{
			delete activeParticles[i];
			activeParticles[i] = nullptr;
		}
	}

	// UnLoad textures
	App->tex->UnLoad(pawsTex);
	App->tex->UnLoad(atlasTex);

	return true;
}

// Update: draw background
bool j1Particles::Update(float dt)
{
	bool ret = true;

	UpdateAnimations(dt);

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		Particle* p = activeParticles[i];

		if (p == nullptr)
			continue;

		if (!p->Update(dt))
		{
			delete p;
			activeParticles[i] = nullptr;
		}
		if (SDL_GetTicks() >= p->born)
		{
			if (p->particleType != ParticleType_Paws)
				App->render->Blit(atlasTex, p->pos.x, p->pos.y, &(p->animation.GetCurrentFrame()));
		}
	}

	return ret;
}

void j1Particles::Draw()
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		Particle* p = activeParticles[i];

		if (p == nullptr)
			continue;

		if (SDL_GetTicks() >= p->born)
		{
			if (p->particleType != ParticleType_Paws)
				App->render->Blit(atlasTex, p->pos.x, p->pos.y, &(p->animation.GetCurrentFrame()));
		}
	}
}

void j1Particles::DrawPaws() 
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		Particle* p = activeParticles[i];

		if (p == nullptr)
			continue;

		if (SDL_GetTicks() >= p->born)
		{
			if (p->particleType == ParticleType_Paws)
				App->render->Blit(pawsTex, p->pos.x, p->pos.y, &(p->animation.GetCurrentFrame()));
		}
	}
}

Particle* j1Particles::AddParticle(const Particle& particle, iPoint pos, ColliderType colliderType, Uint32 delay, fPoint speed)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (activeParticles[i] == nullptr)
		{
			Particle* p = new Particle(particle);

			p->born = SDL_GetTicks() + delay;
			p->pos = { (float)pos.x, (float)pos.y };
			p->speed = speed;

			activeParticles[i] = p;

			return p;
		}
	}
}

void j1Particles::OnCollision(Collider* c1, Collider* c2, CollisionState collisionState)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		// Always destroy particles that collide
		if (activeParticles[i] != nullptr && activeParticles[i]->collider == c1)
		{
			delete activeParticles[i];
			activeParticles[i] = nullptr;
			break;
		}
	}
}

bool j1Particles::IsParticleOnTile(iPoint tile) const
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		Particle* p = activeParticles[i];

		if (p == nullptr)
			continue;

		iPoint particleTile = App->map->WorldToMap(p->pos.x, p->pos.y);

		if (particleTile == tile)
			return true;
	}

	return false;
}

void j1Particles::LoadAnimationsSpeed()
{
	/// Boar Paws
	boarPawsUpSpeed = boarPawsInfo.up.speed;
	boarPawsDownSpeed = boarPawsInfo.down.speed;
	boarPawsLeftSpeed = boarPawsInfo.left.speed;
	boarPawsRightSpeed = boarPawsInfo.right.speed;
	boarPawsUpLeftSpeed = boarPawsInfo.upLeft.speed;
	boarPawsUpRightSpeed = boarPawsInfo.upRight.speed;
	boarPawsDownLeftSpeed = boarPawsInfo.downLeft.speed;
	boarPawsDownRightSpeed = boarPawsInfo.downRight.speed;

	/// Sheep Paws
	sheepPawsUpSpeed = sheepPawsInfo.up.speed;
	sheepPawsDownSpeed = sheepPawsInfo.down.speed;
	sheepPawsLeftSpeed = sheepPawsInfo.left.speed;
	sheepPawsRightSpeed = sheepPawsInfo.right.speed;
	sheepPawsUpLeftSpeed = sheepPawsInfo.upLeft.speed;
	sheepPawsUpRightSpeed = sheepPawsInfo.upRight.speed;
	sheepPawsDownLeftSpeed = sheepPawsInfo.downLeft.speed;
	sheepPawsDownRightSpeed = sheepPawsInfo.downRight.speed;
}

void j1Particles::UpdateAnimations(float dt)
{
	/// Boar Paws
	boarPawsInfo.up.speed = boarPawsUpSpeed * dt;
	boarPawsInfo.down.speed = boarPawsDownSpeed * dt;
	boarPawsInfo.left.speed = boarPawsLeftSpeed * dt;
	boarPawsInfo.right.speed = boarPawsRightSpeed * dt;
	boarPawsInfo.upLeft.speed = boarPawsUpLeftSpeed * dt;
	boarPawsInfo.upRight.speed = boarPawsUpRightSpeed * dt;
	boarPawsInfo.downLeft.speed = boarPawsDownLeftSpeed * dt;
	boarPawsInfo.downRight.speed = boarPawsDownRightSpeed * dt;

	/// Sheep Paws
	sheepPawsInfo.up.speed = sheepPawsUpSpeed * dt;
	sheepPawsInfo.down.speed = sheepPawsDownSpeed * dt;
	sheepPawsInfo.left.speed = sheepPawsLeftSpeed * dt;
	sheepPawsInfo.right.speed = sheepPawsRightSpeed * dt;
	sheepPawsInfo.upLeft.speed = sheepPawsUpLeftSpeed * dt;
	sheepPawsInfo.upRight.speed = sheepPawsUpRightSpeed * dt;
	sheepPawsInfo.downLeft.speed = sheepPawsDownLeftSpeed * dt;
	sheepPawsInfo.downRight.speed = sheepPawsDownRightSpeed * dt;

	trollAxe.animation.speed = 10.0f * dt;
}

BoarPawsInfo& j1Particles::GetBoarPawsInfo()
{
	return boarPawsInfo;
}

SheepPawsInfo& j1Particles::GetSheepPawsInfo()
{
	return sheepPawsInfo;
}

// -------------------------------------------------------------
// -------------------------------------------------------------

Particle::Particle()
{
	pos.SetToZero();
	speed.SetToZero();
}

Particle::Particle(const Particle& p) :
	animation(p.animation), pos(p.pos), speed(p.speed), particleType(p.particleType),
	fx(p.fx), born(p.born), life(p.life), collisionSize(p.collisionSize)
{}

Particle::~Particle()
{
}

bool Particle::Update(float dt)
{
	bool ret = true;

	if (particleType == ParticleType_Paws) {

		if (animation.Finished() && isRemove)
			return false;

		return true;
	}

	/*
	if (life > 0)
	{
	if ((SDL_GetTicks() - born) > life)
	ret = false;
	}
	else
	if (anim.Finished() || life == 0)
	ret = false;
	*/

	if (life > 0)
	{
		if ((SDL_GetTicks() - born) > life)
			ret = false;
	}

	if (isRemove)
		ret = false;

	return ret;
}

