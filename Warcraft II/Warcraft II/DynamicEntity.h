#ifndef __DynamicEntity_H__
#define __DynamicEntity_H__

#include "p2Point.h"
#include "Animation.h"
#include "Entity.h"

struct SDL_Texture;

enum DynamicEntityType
{
	DynamicEntityType_NoType,

	// Player types
	DynamicEntityType_Footman,
	DynamicEntityType_ElvenArcher,
	DynamicEntityType_GryphonRider,
	DynamicEntityType_Mage,
	DynamicEntityType_Paladin,

	DynamicEntityType_Turalyon,
	DynamicEntityType_Khadgar,
	DynamicEntityType_Alleria,

	// Enemy types
	DynamicEntityType_Grunt,
	DynamicEntityType_TrollAxethrower,
	DynamicEntityType_Dragon,

	DynamicEntityType_MaxTypes
};

class DynamicEntity :public Entity
{
public:

	DynamicEntity(fPoint pos, iPoint size, int life, float speed, j1Module* listener);
	virtual ~DynamicEntity() {};
	virtual void Draw(SDL_Texture* sprites);
	//virtual void DebugDrawSelected();

	//virtual void Move(float dt) {};

	// Animations
	virtual void LoadAnimationsSpeed() {};
	virtual void UpdateAnimations(float dt) {};

public:

	DynamicEntityType dynamicEntityType = DynamicEntityType_NoType;

protected:

	float speed = 1.0f;
	Animation* animation = nullptr;

	//j1Module* listener = nullptr;
};

#endif //__DynamicEntity_H__