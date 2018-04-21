#include "Defs.h"
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
	lowFire.size = { fireAnimation.child("low").child("frame").attribute("w").as_int(), fireAnimation.child("low").child("frame").attribute("h").as_int() };

	currentAnimation = fireAnimation.child("hard");
	hardFire.animation.speed = currentAnimation.attribute("speed").as_float();
	hardFire.animation.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		hardFire.animation.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	hardFire.size = { fireAnimation.child("hard").child("frame").attribute("w").as_int(), fireAnimation.child("hard").child("frame").attribute("h").as_int() };

	// Arrows
	pugi::xml_node towerArrows = config.child("towerArrows");
	playerArrows.animation.PushBack({ towerArrows.child("right").attribute("x").as_int(), towerArrows.child("right").attribute("y").as_int(), towerArrows.child("right").attribute("w").as_int(), towerArrows.child("right").attribute("h").as_int() });
	enemyArrows.animation.PushBack({ towerArrows.child("right").attribute("x").as_int(), towerArrows.child("right").attribute("y").as_int(), towerArrows.child("right").attribute("w").as_int(), towerArrows.child("right").attribute("h").as_int() });
	playerArrows.size = { towerArrows.child("right").attribute("w").as_int(), towerArrows.child("right").attribute("h").as_int() };
	enemyArrows.size = { towerArrows.child("right").attribute("w").as_int(), towerArrows.child("right").attribute("h").as_int() };

	// Cannon from the cannon tower
	pugi::xml_node bulletsCannon = config.child("cannon");
	playerCannonBullet.animation.PushBack({ bulletsCannon.attribute("x").as_int(), bulletsCannon.attribute("y").as_int(), bulletsCannon.attribute("w").as_int(), bulletsCannon.attribute("h").as_int() });
	enemyCannonBullet.animation.PushBack({ bulletsCannon.attribute("x").as_int(), bulletsCannon.attribute("y").as_int(), bulletsCannon.attribute("w").as_int(), bulletsCannon.attribute("h").as_int() });
	playerCannonBullet.size = { bulletsCannon.attribute("w").as_int(), bulletsCannon.attribute("h").as_int() };
	enemyCannonBullet.size = { bulletsCannon.attribute("w").as_int(), bulletsCannon.attribute("h").as_int() };

	// Troll's axe
	pugi::xml_node trollAxeAnimation = config.child("trollAxe");
	trollAxe.animation.speed = trollAxeAnimation.attribute("speed").as_float();
	trollAxe.animation.loop = trollAxeAnimation.attribute("loop").as_bool();
	for (currentAnimation = trollAxeAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxe.animation.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	trollAxe.size = { trollAxeAnimation.child("frame").attribute("w").as_int(), trollAxeAnimation.child("frame").attribute("h").as_int() };

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

	trollAxe.particleType = ParticleType_Enemy_Projectile;
	playerArrows.particleType = ParticleType_Player_Projectile;
	enemyArrows.particleType = ParticleType_Enemy_Projectile;	
	playerCannonBullet.particleType = ParticleType_Player_Projectile;
	enemyCannonBullet.particleType = ParticleType_Enemy_Projectile;
	lowFire.particleType = ParticleType_Fire;
	hardFire.particleType = ParticleType_Fire;

	paws.life = 800;

	sheepPawsInfo.up.speed = 1.0f;
	sheepPawsInfo.down.speed = 1.0f;
	sheepPawsInfo.left.speed = 1.0f;
	sheepPawsInfo.right.speed = 1.0f;
	sheepPawsInfo.upLeft.speed = 1.0f;
	sheepPawsInfo.upRight.speed = 1.0f;
	sheepPawsInfo.downLeft.speed = 1.0f;
	sheepPawsInfo.downRight.speed = 1.0f;

	boarPawsInfo.up.speed = 1.0f;
	boarPawsInfo.down.speed = 1.0f;
	boarPawsInfo.left.speed = 1.0f;
	boarPawsInfo.right.speed = 1.0f;
	boarPawsInfo.upLeft.speed = 1.0f;
	boarPawsInfo.upRight.speed = 1.0f;
	boarPawsInfo.downLeft.speed = 1.0f;
	boarPawsInfo.downRight.speed = 1.0f;

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
		Particle* currPart = activeParticles[i];

		if (currPart == nullptr)
			continue;

		if (!currPart->Update(dt))
		{
			delete currPart;
			activeParticles[i] = nullptr;
		}

		if (SDL_GetTicks() >= currPart->born)
		{
			if (currPart->particleType != ParticleType_Paws)
				App->render->Blit(atlasTex, currPart->pos.x, currPart->pos.y, &(currPart->animation.GetCurrentFrame()), 1.0f, currPart->angle);
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
				App->render->Blit(atlasTex, p->pos.x, p->pos.y, &(p->animation.GetCurrentFrame()), 1.0f, p->angle);
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
				App->render->Blit(pawsTex, p->pos.x, p->pos.y, &(p->animation.GetCurrentFrame()), 1.0f, p->angle);
		}
	}
}

Particle* j1Particles::AddParticle(const Particle& particle, iPoint pos, fPoint destination, float speed, uint damage, Uint32 delay)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (activeParticles[i] == nullptr)
		{
			Particle* currPart = new Particle(particle);

			currPart->born = SDL_GetTicks() + delay;
			currPart->pos = { (float)pos.x, (float)pos.y };
			currPart->destination = destination;
			currPart->speed = speed;
			currPart->damage = damage;

			activeParticles[i] = currPart;

			switch (currPart->particleType) {

			case ParticleType_Player_Projectile:
			case ParticleType_Enemy_Projectile:
			{
				// Calculate the orientation of the particle
				currPart->orientation.x = currPart->destination.x - pos.x;
				currPart->orientation.y = currPart->destination.y - pos.y;

				float m = sqrtf(pow(currPart->orientation.x, 2.0f) + pow(currPart->orientation.y, 2.0f));

				if (m > 0) {

					currPart->orientation.x /= m;
					currPart->orientation.y /= m;
				}

				// Calculate the angle of the particle
				currPart->angle = (atan2(currPart->orientation.y, currPart->orientation.x) * 180.0f / (float)M_PI);

				if (currPart->angle >= 360.0f)
					currPart->angle = 0.0f;
			}
			break;

			default:
				break;
			}

			return currPart;
		}
	}
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

	/// Troll Axe
	trollAxeSpeed = trollAxe.animation.speed;

	/// Fire Speed
	lowFireSpeed = lowFire.animation.speed;
	hardFireSpeed = hardFire.animation.speed;
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

	/// Troll Axe
	trollAxe.animation.speed = trollAxeSpeed * dt;

	/// Fire Speed
	lowFire.animation.speed = lowFireSpeed * dt;
	hardFire.animation.speed = hardFireSpeed * dt;
}

PawsInfo& j1Particles::GetPawsInfo(bool isSheep, bool isBoar)
{
	if (isSheep)
		return sheepPawsInfo;
	else
		return boarPawsInfo;
}

// -------------------------------------------------------------
// -------------------------------------------------------------

Particle::Particle()
{
	particleType = ParticleType_NoType;

	pos = { 0.0f,0.0f };
	size = { 0,0 };
	destination = { 0.0f,0.0f };

	speed = 0.0f;
	life = 0;
	damage = 0;

	orientation = { 0.0f,0.0f };
	born = 0;
	angle = 0.0f;

	isRemove = false;
}

Particle::Particle(const Particle& p) :
	animation(p.animation), pos(p.pos), destination(p.destination),
	speed(p.speed), particleType(p.particleType), born(p.born), life(p.life),
	damage(p.damage), orientation(p.orientation), isRemove(p.isRemove), angle(p.angle),
	size(p.size)
{}

Particle::~Particle() {}

bool Particle::Update(float dt)
{
	bool ret = true;

	switch (particleType) {

	case ParticleType_Player_Projectile:
	case ParticleType_Enemy_Projectile:
	{
		iPoint destinationTile = App->map->WorldToMap(destination.x, destination.y);

		const SDL_Rect rectA = { (int)pos.x, (int)pos.y, size.x, size.y };
		const SDL_Rect rectB = { destination.x, destination.y,  App->map->data.tileWidth, App->map->data.tileHeight };

		if (SDL_HasIntersection(&rectA, &rectB)) {

			// Apply damage and kill the particle if it reaches its target
			Entity* entity = App->entities->IsEntityOnTile(destinationTile);

			if (particleType == ParticleType_Player_Projectile) {
				if (entity != nullptr) {
					if (entity->entitySide == EntitySide_Enemy || entity->entitySide == EntitySide_Neutral)
						entity->ApplyDamage(damage);
				}
			}
			else if (particleType == ParticleType_Enemy_Projectile) {
				if (entity != nullptr) {
					if (entity->entitySide == EntitySide_Player || entity->entitySide == EntitySide_Neutral)
						entity->ApplyDamage(damage);
				}
			}

			return false;
		}

		pos.x += orientation.x * dt * speed;
		pos.y += orientation.y * dt * speed;

		return true;
	}
	break;

	case ParticleType_Paws:

		if (animation.Finished() && isRemove)
			return false;
		else
			return true;

		break;

	case ParticleType_Fire:
		break;

	default:
		break;
	}

	// Remove the particle depending on its life
	if (life > 0)
		if ((SDL_GetTicks() - born) > life)
			ret = false;

	// Remove the particle depending on its isRemove
	if (isRemove)
		ret = false;

	return ret;
}