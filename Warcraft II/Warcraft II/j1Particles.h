#ifndef __j1PARTICLES_H__
#define __j1PARTICLES_H__

#include "j1Module.h"

#include "Defs.h"
#include "p2Point.h"
#include "Animation.h"

#define MAX_ACTIVE_PARTICLES 100

struct SDL_Texture;
struct Collider;
enum COLLIDER_TYPE;

struct Particle
{
	Collider* collider = nullptr;
	Animation anim;
	uint fx = 0;
	fPoint position = { 0,0 };
	fPoint speed = { 0, 0 };
	Uint32 born = 0;
	Uint32 life = 0;

	iPoint collisionSize = { 0,0 };
	fPoint destination = { 0,0 };

	bool left = true, right = true, up = true, down = true;

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
	bool CleanUp();

	void AddParticle(const Particle& particle, int x, int y, COLLIDER_TYPE colliderType, Uint32 delay, fPoint speed);
	void OnCollision(Collider* c1, Collider* c2);

	void UpdateAnimations(const float dt);
	void LoadAnimationsSpeed();

private:
	Particle* active[MAX_ACTIVE_PARTICLES];
	uint lastParticle = 0;

public:
	Particle arrowRight;
};

#endif //__j1PARTICLES_H__
