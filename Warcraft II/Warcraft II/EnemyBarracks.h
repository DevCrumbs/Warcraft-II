#ifndef __EnemyBarracks_H__
#define __EnemyBarracks_H__

#include "StaticEntity.h"

struct EnemyBarracksInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class EnemyBarracks :public StaticEntity
{
public:
	EnemyBarracks(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyBarracksInfo& enemyBarracksInfo, j1Module* listener);
	virtual ~EnemyBarracks(){}

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	EnemyBarracksInfo enemyBarracksInfo;

};

#endif

