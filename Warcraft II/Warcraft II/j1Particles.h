#ifndef __j1PARTICLES_H__
#define __j1PARTICLES_H__

#include "j1Module.h"

#include "Defs.h"
#include "p2Point.h"
#include "Animation.h"
#include "j1Collision.h"

#include <string>
using namespace std;

#define MAX_ACTIVE_PARTICLES 5000 // if max particles are exceeded, they should not be created nor printed (not crush!)

struct SDL_Texture;

struct Collider;
struct ColliderGroup;
enum ColliderType;

enum ParticleType {

	ParticleType_NoType,

	ParticleType_Player_Projectile,

	ParticleType_Enemy_Projectile,

	ParticleType_Paws,

	ParticleType_MaxTypes
};

struct PawsInfo {

	Animation up, down, left, right;
	Animation upLeft, upRight, downLeft, downRight;
};

struct Particle {

	ParticleType particleType = ParticleType_NoType;

	Animation animation;

	fPoint pos = { 0.0f,0.0f };
	iPoint destinationTile = { 0,0 };

	float speed = 0.0f;
	Uint32 life = 0;
	uint damage = 0;

	fPoint orientation = { 0.0f,0.0f };
	Uint32 born = 0;
	double angle = 0.0f;

	bool isRemove = false;

	Particle();
	Particle(const Particle& p);
	~Particle();
	bool Update(float dt);
};

class j1Particles : public j1Module
{
public:

	j1Particles();
	~j1Particles();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	bool Start();
	bool Update(float dt);
	void Draw();
	void DrawPaws();
	bool CleanUp();

	Particle* AddParticle(const Particle& particle, iPoint pos, iPoint destinationTile = { -1,-1 }, float speed = 0.0f, uint damage = 0, Uint32 delay = 0);

	bool IsParticleOnTile(iPoint tile) const;

	void UpdateAnimations(const float dt);
	void LoadAnimationsSpeed();

	PawsInfo& GetPawsInfo(bool isSheep = false, bool isBoar = false);

private:

	string atlasTexName;
	SDL_Texture* atlasTex = nullptr;

	Particle* activeParticles[MAX_ACTIVE_PARTICLES];
	uint lastParticle = 0;

	string pawsTexName;
	SDL_Texture* pawsTex = nullptr;

	PawsInfo boarPawsInfo;
	PawsInfo sheepPawsInfo;

	// Animations speed
	/// Boar Paws
	float boarPawsUpSpeed = 0.0f, boarPawsDownSpeed = 0.0f, boarPawsLeftSpeed = 0.0f, boarPawsRightSpeed = 0.0f;
	float boarPawsUpLeftSpeed = 0.0f, boarPawsUpRightSpeed = 0.0f, boarPawsDownLeftSpeed = 0.0f, boarPawsDownRightSpeed = 0.0f;

	/// Sheep Paws
	float sheepPawsUpSpeed = 0.0f, sheepPawsDownSpeed = 0.0f, sheepPawsLeftSpeed = 0.0f, sheepPawsRightSpeed = 0.0f;
	float sheepPawsUpLeftSpeed = 0.0f, sheepPawsUpRightSpeed = 0.0f, sheepPawsDownLeftSpeed = 0.0f, sheepPawsDownRightSpeed = 0.0f;

	/// Troll Axe
	float trollAxeSpeed = 0.0f;

	/// Fire Speed
	float lowFireSpeed = 0.0f;
	float hardFireSpeed = 0.0f;

public:

	// Fire
	Particle lowFire;
	Particle hardFire;


	// Arrows, axes and cannon bullets
	Particle playerArrows;
	Particle enemyArrows;

	Particle playerCannonBullet;
	Particle enemyCannonBullet;

	Particle trollAxe;
	
	// Paws
	Particle paws;
};

#endif //__j1PARTICLES_H__
