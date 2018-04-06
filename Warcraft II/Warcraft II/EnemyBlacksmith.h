#ifndef __EnemyBlacksmith_H__
#define __EnemyBlacksmith_H__

#include "StaticEntity.h"

struct EnemyBlacksmithInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class EnemyBlacksmith :public StaticEntity
{
public:
	EnemyBlacksmith(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyBlacksmithInfo& enemyBlacksmithInfo, j1Module* listener);
	virtual ~EnemyBlacksmith(){}

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	EnemyBlacksmithInfo enemyBlacksmithInfo;

};

#endif

