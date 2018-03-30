#ifndef __DynamicEntity_H__
#define __DynamicEntity_H__

#include "p2Point.h"
#include "Animation.h"
#include "Entity.h"

struct SDL_Texture;

enum ENTITY_TYPE;

class DynamicEntity :public Entity
{
public:

	DynamicEntity(fPoint pos, iPoint size, int maxLife, float speed, j1Module* listener);
	virtual ~DynamicEntity() {};
	virtual void Draw(SDL_Texture* sprites);
	//virtual void DebugDrawSelected();

	//virtual void Move(float dt) {};

	// Animations
	virtual void LoadAnimationsSpeed() {};
	virtual void UpdateAnimations(float dt) {};

public:

	ENTITY_TYPE dynamicEntityType = EntityType_NONE;
	bool isSpawned = false;

protected:

	float speed = 1.0f;
	Animation* animation = nullptr;

};

#endif //__DynamicEntity_H__