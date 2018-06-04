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
	~EnemyBlacksmith();

	void Move(float dt);

private:

	EnemyBlacksmithInfo enemyBlacksmithInfo;

	// Reconstruction
	Particle* peon = nullptr;

	float secondsReconstruction = 0.0f;

	float startReconstructionTimer = 0.0f;
	float inProgressReconstructionTimer = 0.0f;

	bool isRestartReconstructionTimer = false;
	bool isStartReconstructionTimer = false;
	bool isInProgressReconstructionTimer = false;

	BuildingState buildingStateBeforeReconstruction = BuildingState_NoState;
};

#endif

