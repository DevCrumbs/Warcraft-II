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
#include "j1Printer.h"

#include "Entity.h"
#include "DynamicEntity.h"
#include "j1Movement.h"

#include <math.h>

#include "SDL/include/SDL_timer.h"

j1Particles::j1Particles()
{
	name.assign("particles");
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

	// Dragon Fire (and Sub Fire)
	pugi::xml_node dragonFireAnimation = config.child("dragonFire");
	dragonSubFire.animation.speed = dragonFireAnimation.attribute("speed").as_float();
	dragonSubFire.animation.loop = dragonFireAnimation.attribute("loop").as_bool();
	for (currentAnimation = dragonFireAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonSubFire.animation.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	dragonSubFire.size = { 32,32 };
	dragonFire.size = { 32,32 };

	// Gryphon Fire (and Sub Fire)
	pugi::xml_node gryphonFireAnimation = config.child("gryphonFire");
	gryphonSubFire.animation.speed = gryphonFireAnimation.attribute("speed").as_float();
	gryphonSubFire.animation.loop = gryphonFireAnimation.attribute("loop").as_bool();
	for (currentAnimation = gryphonFireAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonSubFire.animation.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	gryphonSubFire.size = { 32,32 };
	gryphonFire.size = { 32,32 };

	// Cannon from the cannon tower
	pugi::xml_node bulletsCannon = config.child("cannon");
	cannonBullet.animation.PushBack({ bulletsCannon.attribute("x").as_int(), bulletsCannon.attribute("y").as_int(), bulletsCannon.attribute("w").as_int(), bulletsCannon.attribute("h").as_int() });
	cannonBullet.size = { bulletsCannon.attribute("w").as_int(), bulletsCannon.attribute("h").as_int() };

	// Troll's axe
	pugi::xml_node trollAxeAnimation = config.child("trollAxe");
	trollAxe.animation.speed = trollAxeAnimation.attribute("speed").as_float();
	trollAxe.animation.loop = trollAxeAnimation.attribute("loop").as_bool();
	for (currentAnimation = trollAxeAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxe.animation.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	trollAxe.size = { trollAxeAnimation.child("frame").attribute("w").as_int(), trollAxeAnimation.child("frame").attribute("h").as_int() };

	// Health
	/// Player health
	pugi::xml_node playerHealthAnimation = config.child("playerHealth");
	playerHealth.animation.speed = playerHealthAnimation.attribute("speed").as_float();
	playerHealth.animation.loop = playerHealthAnimation.attribute("loop").as_bool();
	for (currentAnimation = playerHealthAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		playerHealth.animation.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	playerHealth.size = { playerHealthAnimation.child("frame").attribute("w").as_int(), playerHealthAnimation.child("frame").attribute("h").as_int() };
	
	/// Enemy health
	pugi::xml_node enemyHealthAnimation = config.child("enemyHealth");
	enemyHealth.animation.speed = enemyHealthAnimation.attribute("speed").as_float();
	enemyHealth.animation.loop = enemyHealthAnimation.attribute("loop").as_bool();
	for (currentAnimation = enemyHealthAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		enemyHealth.animation.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	enemyHealth.size = { enemyHealthAnimation.child("frame").attribute("w").as_int(), enemyHealthAnimation.child("frame").attribute("h").as_int() };

	// Sheep Paws
	pugi::xml_node sheepPawsAnimation = config.child("animations").child("sheepPaws");

	// Cross for the mouse press
	pugi::xml_node crossAnimation = config.child("cross");
	cross.animation.speed = crossAnimation.attribute("speed").as_float();
	cross.animation.loop = crossAnimation.attribute("loop").as_bool();
	for (currentAnimation = crossAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		cross.animation.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	cross.size = { crossAnimation.child("frame").attribute("w").as_int(), crossAnimation.child("frame").attribute("h").as_int() };

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
	sheepPaws.animation = sheepPawsInfo.right;

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
	boarPaws.animation = boarPawsInfo.right;

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

	// Particles info
	/// Type
	lowFire.particleType = ParticleType_Fire;
	hardFire.particleType = ParticleType_Fire;

	playerArrows.particleType = ParticleType_Player_Projectile;
	enemyArrows.particleType = ParticleType_Enemy_Projectile;

	cannonBullet.particleType = ParticleType_Cannon_Projectile;

	trollAxe.particleType = ParticleType_Enemy_Projectile;

	dragonFire.particleType = ParticleType_DragonFire;
	dragonSubFire.particleType = ParticleType_DragonSubFire;
	gryphonFire.particleType = ParticleType_GryphonFire;
	gryphonSubFire.particleType = ParticleType_GryphonSubFire;

	boarPaws.particleType = ParticleType_Paws;
	sheepPaws.particleType = ParticleType_Paws;

	playerHealth.particleType = ParticleType_Health;
	enemyHealth.particleType = ParticleType_Health;

	cross.particleType = ParticleType_Cross;

	/// Life
	boarPaws.life = 800;
	sheepPaws.life = 800;
	dragonSubFire.life = 800;
	gryphonSubFire.life = 800;

	/// Others
	dragonSubFire.secondsToDamage = 0.3f;
	gryphonSubFire.secondsToDamage = 0.3f;

	/// Animations speed
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

	if(!isAnimationSpeedCharged)
	LoadAnimationsSpeed();

	atlasTex = App->tex->Load(atlasTexName.data());
	pawsTex = App->tex->Load(pawsTexName.data());

	return ret;
}

// Unload assets
bool j1Particles::CleanUp()
{
	LOG("Unloading particles");

	list<Particle*>::const_iterator it = activeParticles.begin();

	while (it != activeParticles.end()) {
	
		delete *it;
		it++;
	}
	activeParticles.clear();

	// UnLoad textures
	App->tex->UnLoad(pawsTex);
	App->tex->UnLoad(atlasTex);

	return true;
}

bool j1Particles::PreUpdate() 
{
	bool ret = true;

	list<Particle*>::const_iterator it = toSpawnParticles.begin();

	while (it != toSpawnParticles.end()) {

		activeParticles.push_back(*it);
		it++;
	}
	toSpawnParticles.clear();

	return ret;
}

// Update: draw background
bool j1Particles::Update(float dt)
{
	bool ret = true;

	// Update the particles
	list<Particle*>::const_iterator it = activeParticles.begin();

	while (it != activeParticles.end()) {

		if (!(*it)->Update(dt))
			(*it)->isDelete = true;

		it++;
	}

	// Update the speed of the animations
	UpdateAnimations(dt);

	return ret;
}

bool j1Particles::PostUpdate()
{
	bool ret = true;

	// Remove particles with isDelete == true
	list<Particle*>::const_iterator it = activeParticles.begin();

	while (it != activeParticles.end()) {

		if ((*it)->isDelete) {

			delete *it;
			activeParticles.remove(*it);
			it = activeParticles.begin();
			continue;
		}
		it++;
	}

	// Send active particles to blit
	it = activeParticles.begin();

	while (it != activeParticles.end()) {

		if (!App->render->IsInScreen({ (int)(*it)->pos.x, (int)(*it)->pos.y, (*it)->size.x, (*it)->size.y })) {
		
			it++;
			continue;
		}

		if ((*it)->particleType == ParticleType_Paws)
			App->printer->PrintSprite({ (int)(*it)->pos.x, (int)(*it)->pos.y }, pawsTex, (*it)->animation.GetCurrentFrame(), Layers_Paws, (*it)->angle);
		else if ((*it)->particleType == ParticleType_Health)
			App->printer->PrintSprite({ (int)(*it)->pos.x - 3, (int)(*it)->pos.y - 15 }, atlasTex, (*it)->animation.GetCurrentFrame(), Layers_BasicParticles, (*it)->angle);
		else
			App->printer->PrintSprite({ (int)(*it)->pos.x, (int)(*it)->pos.y }, atlasTex, (*it)->animation.GetCurrentFrame(), Layers_BasicParticles, (*it)->angle);

		it++;
	}

	return ret;
}

Particle* j1Particles::AddParticle(const Particle& particle, iPoint pos, fPoint destination, float speed, uint damage, Uint32 delay, double angle)
{
	Particle* currPart = new Particle(particle);

	currPart->born = SDL_GetTicks() + delay;
	currPart->pos = { (float)pos.x, (float)pos.y };
	currPart->destination = destination;
	currPart->speed = speed;
	currPart->damage = damage;
	currPart->angle = angle;

	switch (currPart->particleType) {

	case ParticleType_Player_Projectile:
	case ParticleType_Enemy_Projectile:
	case ParticleType_Cannon_Projectile:
	{
		// Calculate the orientation of the particle
		currPart->orientation.x = currPart->destination.x - pos.x;
		currPart->orientation.y = currPart->destination.y - pos.y;

		float m = sqrtf(pow(currPart->orientation.x, 2.0f) + pow(currPart->orientation.y, 2.0f));

		if (m > 0.0f) {
			currPart->orientation.x /= m;
			currPart->orientation.y /= m;
		}

		// Calculate the angle of the particle
		currPart->angle = (atan2(currPart->orientation.y, currPart->orientation.x) * 180.0f / (float)M_PI);

		if (currPart->angle >= 360.0f)
			currPart->angle = 0.0f;
	}
	break;

	case ParticleType_DragonFire:
	case ParticleType_GryphonFire:
	{
		// Calculate the orientation of the particle
		currPart->orientation.x = currPart->destination.x - pos.x;
		currPart->orientation.y = currPart->destination.y - pos.y;

		float m = sqrtf(pow(currPart->orientation.x, 2.0f) + pow(currPart->orientation.y, 2.0f));

		if (m > 0.0f) {
			currPart->orientation.x /= m;
			currPart->orientation.y /= m;
		}
	}
	break;

	case ParticleType_DragonSubFire:
	case ParticleType_GryphonSubFire:

		currPart->life = rand() % 800 + 300;
		currPart->damageTimer.Start();
		break;

	default:
		break;
	}

	toSpawnParticles.push_back(currPart);

	return currPart;
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

	/// Fire
	lowFireSpeed = lowFire.animation.speed;
	hardFireSpeed = hardFire.animation.speed;

	/// Health
	healthSpeed = playerHealth.animation.speed;

	/// Dragon and Gryphon Fire
	dragonSubFireSpeed = dragonSubFire.animation.speed;
	gryphonSubFireSpeed = gryphonSubFire.animation.speed;

	/// Cross Speed
	crossSpeed = cross.animation.speed;

	isAnimationSpeedCharged = true;
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

	boarPaws.animation.speed = boarPawsRightSpeed * dt;

	/// Sheep Paws
	sheepPawsInfo.up.speed = sheepPawsUpSpeed * dt;
	sheepPawsInfo.down.speed = sheepPawsDownSpeed * dt;
	sheepPawsInfo.left.speed = sheepPawsLeftSpeed * dt;
	sheepPawsInfo.right.speed = sheepPawsRightSpeed * dt;
	sheepPawsInfo.upLeft.speed = sheepPawsUpLeftSpeed * dt;
	sheepPawsInfo.upRight.speed = sheepPawsUpRightSpeed * dt;
	sheepPawsInfo.downLeft.speed = sheepPawsDownLeftSpeed * dt;
	sheepPawsInfo.downRight.speed = sheepPawsDownRightSpeed * dt;

	sheepPaws.animation.speed = sheepPawsRightSpeed * dt;

	/// Troll Axe
	trollAxe.animation.speed = trollAxeSpeed * dt;

	/// Fire
	lowFire.animation.speed = lowFireSpeed * dt;
	hardFire.animation.speed = hardFireSpeed * dt;

	/// Health
	playerHealth.animation.speed = healthSpeed * dt;
	enemyHealth.animation.speed = healthSpeed * dt;

	/// Dragon and Gryphon Fire
	dragonSubFire.animation.speed = dragonSubFireSpeed * dt;
	gryphonSubFire.animation.speed = gryphonSubFireSpeed * dt;

	/// Cross
	cross.animation.speed = crossSpeed * dt;
}

PawsInfo& j1Particles::GetPawsInfo(bool isSheep, bool isBoar)
{
	if (isSheep)
		return sheepPawsInfo;
	else
		return boarPawsInfo;
}

bool j1Particles::IsParticleOnTile(iPoint tile, ParticleType particleType)
{
	list<Particle*>::const_iterator it = activeParticles.begin();

	while (it != activeParticles.end()) {

		iPoint currPartTile = App->map->WorldToMap((*it)->pos.x, (*it)->pos.y);

		if (particleType == ParticleType_NoType) {

			if (tile == currPartTile)
				return true;
		}
		else {

			if (particleType == (*it)->particleType) {

				if (tile == currPartTile)
					return true;
			}
		}
		it++;
	}

	return false;
}

// -------------------------------------------------------------
// -------------------------------------------------------------

Particle::Particle() {}

Particle::Particle(const Particle& p) :
	animation(p.animation), pos(p.pos), destination(p.destination),
	speed(p.speed), particleType(p.particleType), born(p.born), life(p.life),
	damage(p.damage), orientation(p.orientation), isRemove(p.isRemove), angle(p.angle),
	size(p.size), secondsToDamage(p.secondsToDamage), isDelete(p.isDelete)
{}

Particle::~Particle() 
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
	isDelete = false;

	secondsToDamage = 0.0f;
}

bool Particle::Update(float dt)
{
	bool ret = true;

	switch (particleType) {

	case ParticleType_Player_Projectile:
	case ParticleType_Enemy_Projectile:
	case ParticleType_Cannon_Projectile:
	{
		const SDL_Rect rectA = { (int)pos.x, (int)pos.y, size.x, size.y };
		const SDL_Rect rectB = { destination.x, destination.y,  App->map->data.tileWidth, App->map->data.tileHeight };

		if (SDL_HasIntersection(&rectA, &rectB)) {

			// Apply damage on dynamic entities on the way of the particle, if the particle is a cannon bullet
			Entity* entity = App->entities->AreEntitiesColliding({ (int)destination.x, (int)destination.y, size.x, size.y });

			if (particleType == ParticleType_Cannon_Projectile) {

				if (entity != nullptr) {

					if (entity->entitySide == EntitySide_Player ||
						entity->entitySide == EntitySide_Neutral || entity->entitySide == EntitySide_Enemy) {

						if (entity->entityType == EntityCategory_STATIC_ENTITY) {
							entity->ApplyDamage(damage);
							
							StaticEntity* statEnt = (StaticEntity*)entity;
							statEnt->CheckBuildingState();
						}
						else
							entity->ApplyDamage(damage);
					}

					return false;
				}
			}

			// Apply damage and kill the particle if it reaches its target
			if (entity != nullptr) {

				if (particleType == ParticleType_Player_Projectile) {

					if (entity->entitySide == EntitySide_Enemy || entity->entitySide == EntitySide_Neutral) {
					
						if (entity->entityType == EntityCategory_STATIC_ENTITY) {
							entity->ApplyDamage(damage);

							StaticEntity* statEnt = (StaticEntity*)entity;
							statEnt->CheckBuildingState();
						}
						else
							entity->ApplyDamage(damage);
					}

					return false;
				}
				else if (particleType == ParticleType_Enemy_Projectile) {

					if (entity->entitySide == EntitySide_Player || entity->entitySide == EntitySide_Neutral) {
					
						if (entity->entityType == EntityCategory_STATIC_ENTITY) {
							entity->ApplyDamage(damage);

							StaticEntity* statEnt = (StaticEntity*)entity;
							statEnt->CheckBuildingState();
						}
						else
							entity->ApplyDamage(damage);
					}

					return false;
				}
				else if (particleType == ParticleType_Cannon_Projectile) {

					if (entity->entitySide == EntitySide_Player ||
						entity->entitySide == EntitySide_Neutral || entity->entitySide == EntitySide_Enemy) {
					
						if (entity->entityType == EntityCategory_STATIC_ENTITY) {
							entity->ApplyDamage(damage);

							StaticEntity* statEnt = (StaticEntity*)entity;
							statEnt->CheckBuildingState();
						}
						else
							entity->ApplyDamage(damage);
					}

					return false;
				}
			}

			return false;
		}

		pos.x += orientation.x * dt * speed;
		pos.y += orientation.y * dt * speed;

		if (isRemove)
			return false;

		return true;
	}
	break;

	case ParticleType_Paws:

		if (isRemove && animation.Finished())
			return false;
		else
			return true;

		break;

	case ParticleType_Health:
	case ParticleType_Cross:
	{
		if (animation.Finished())
			return false;
		else
			return true;
	}
	break;

	case ParticleType_Fire:
		break;

	case ParticleType_DragonFire:
	{
		// Explodes into a new DragonSubFire particle on each tile it touches
		iPoint currTile = App->map->WorldToMap(pos.x, pos.y);
		iPoint currTilePos = App->map->MapToWorld(currTile.x, currTile.y);

		/// Move the invisible particle towards its destination
		const SDL_Rect rectA = { (int)pos.x, (int)pos.y, size.x, size.y };
		const SDL_Rect rectB = { destination.x, destination.y,  App->map->data.tileWidth, App->map->data.tileHeight };

		//App->printer->PrintQuad(rectA, ColorWhite, false);
		//App->printer->PrintQuad(rectB, ColorGreen, false);

		if (SDL_HasIntersection(&rectA, &rectB)) {

			iPoint destinationTile = App->map->WorldToMap(destination.x, destination.y);

			if (!App->particles->IsParticleOnTile(destinationTile, ParticleType_DragonSubFire))

				App->particles->AddParticle(App->particles->dragonSubFire, { (int)destination.x, (int)destination.y }, { 0,0 }, 0.0f, damage);

			return false;
		}

		/// Explode into a new DragonSubFire when a new tile is reached
		if (!App->particles->IsParticleOnTile(currTile, ParticleType_DragonSubFire)) {

			Entity* player = App->entities->IsEntityUnderMouse({ (int)pos.x, (int)pos.y }, EntityCategory_DYNAMIC_ENTITY, EntitySide_Player);
			Entity* playerBuilding = App->entities->IsEntityUnderMouse({ (int)pos.x, (int)pos.y }, EntityCategory_STATIC_ENTITY, EntitySide_Player);

			if (player != nullptr || playerBuilding != nullptr)

				App->particles->AddParticle(App->particles->dragonSubFire, currTilePos, { 0,0 }, 0.0f, damage);
			else {

				int random = rand() % 10;

				if (random < 8)
					App->particles->AddParticle(App->particles->dragonSubFire, currTilePos, { 0,0 }, 0.0f, damage);
			}
		}

		pos.x += orientation.x * dt * speed;
		pos.y += orientation.y * dt * speed;
	}
	break;

	case ParticleType_DragonSubFire:
	{
		// Hits the entities under the fire (per time)
		Entity* player = App->entities->AreEntitiesColliding({ (int)pos.x, (int)pos.y, size.x, size.y }, EntityCategory_DYNAMIC_ENTITY, EntitySide_Player);
		Entity* playerBuilding = App->entities->AreEntitiesColliding({ (int)pos.x, (int)pos.y, size.x, size.y }, EntityCategory_STATIC_ENTITY, EntitySide_Player);
		Entity* critter = App->entities->AreEntitiesColliding({ (int)pos.x, (int)pos.y, size.x, size.y }, EntityCategory_DYNAMIC_ENTITY, EntitySide_Neutral);

		//SDL_Rect r = { (int)pos.x, (int)pos.y, size.x, size.y };
		//App->printer->PrintQuad(r, ColorBlue, false);

		if (player != nullptr || playerBuilding != nullptr || critter != nullptr) {

			if (damageTimer.ReadSec() >= secondsToDamage) {

				if (player != nullptr)
					player->ApplyDamage(damage);
				else if (critter != nullptr)
					critter->ApplyDamage(damage);
				else if (playerBuilding != nullptr) {
					playerBuilding->ApplyDamage(damage);

					StaticEntity* statEnt = (StaticEntity*)playerBuilding;
					statEnt->CheckBuildingState();
				}

				damageTimer.Start();
			}
		}
	}
	break;

	case ParticleType_GryphonFire:
	{
		// Explodes into a new GryphonSubFire particle on each tile it touches
		iPoint currTile = App->map->WorldToMap(pos.x, pos.y);
		iPoint currTilePos = App->map->MapToWorld(currTile.x, currTile.y);

		/// Move the invisible particle towards its destination
		const SDL_Rect rectA = { (int)pos.x, (int)pos.y, size.x, size.y };
		const SDL_Rect rectB = { destination.x, destination.y,  App->map->data.tileWidth, App->map->data.tileHeight };

		//App->printer->PrintQuad(rectA, ColorWhite, false);
		//App->printer->PrintQuad(rectB, ColorGreen, false);

		if (SDL_HasIntersection(&rectA, &rectB)) {

			iPoint destinationTile = App->map->WorldToMap(destination.x, destination.y);

			if (!App->particles->IsParticleOnTile(destinationTile, ParticleType_GryphonSubFire))

				App->particles->AddParticle(App->particles->gryphonSubFire, { (int)destination.x, (int)destination.y }, { 0,0 }, 0.0f, damage);

			return false;
		}

		/// Explode into a new GryphonSubFire when a new tile is reached
		if (!App->particles->IsParticleOnTile(currTile, ParticleType_GryphonSubFire)) {

			Entity* enemy = App->entities->IsEntityUnderMouse({ (int)pos.x, (int)pos.y }, EntityCategory_DYNAMIC_ENTITY, EntitySide_Enemy);
			Entity* enemyBuilding = App->entities->IsEntityUnderMouse({ (int)pos.x, (int)pos.y }, EntityCategory_STATIC_ENTITY, EntitySide_Enemy);
			Entity* critter = App->entities->IsEntityUnderMouse({ (int)pos.x, (int)pos.y }, EntityCategory_DYNAMIC_ENTITY, EntitySide_Neutral);

			if (enemy != nullptr || enemyBuilding != nullptr || critter != nullptr)

				App->particles->AddParticle(App->particles->gryphonSubFire, currTilePos, { 0,0 }, 0.0f, damage);
			else {

				int random = rand() % 10;

				if (random < 8)
					App->particles->AddParticle(App->particles->gryphonSubFire, currTilePos, { 0,0 }, 0.0f, damage);
			}
		}

		pos.x += orientation.x * dt * speed;
		pos.y += orientation.y * dt * speed;
	}
	break;

	case ParticleType_GryphonSubFire:
	{
		// Hits the entities under the fire (per time)
		Entity* enemy = App->entities->AreEntitiesColliding({ (int)pos.x, (int)pos.y, size.x, size.y }, EntityCategory_DYNAMIC_ENTITY, EntitySide_Enemy);
		Entity* critter = App->entities->AreEntitiesColliding({ (int)pos.x, (int)pos.y, size.x, size.y }, EntityCategory_DYNAMIC_ENTITY, EntitySide_Neutral);
		Entity* enemyBuilding = App->entities->AreEntitiesColliding({ (int)pos.x, (int)pos.y, size.x, size.y }, EntityCategory_STATIC_ENTITY, EntitySide_Enemy);

		//SDL_Rect r = { (int)pos.x, (int)pos.y, size.x, size.y };
		//App->printer->PrintQuad(r, ColorBlue, false);

		if (enemy != nullptr || enemyBuilding != nullptr || critter != nullptr) {

			if (damageTimer.ReadSec() >= secondsToDamage) {

				if (enemy != nullptr)
					enemy->ApplyDamage(damage);
				else if (critter != nullptr)
					critter->ApplyDamage(damage);
				else if (enemyBuilding != nullptr) {
					enemyBuilding->ApplyDamage(damage);

					StaticEntity* statEnt = (StaticEntity*)enemyBuilding;
					statEnt->CheckBuildingState();
				}

				damageTimer.Start();
			}
		}
	}
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