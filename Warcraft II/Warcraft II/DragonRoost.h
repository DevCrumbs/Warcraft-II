#ifndef __DragonRoost_H__
#define __DragonRoost_H__

#include "StaticEntity.h"

struct DragonRoostInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class DragonRoost :public StaticEntity
{
public:
	DragonRoost(fPoint pos, iPoint size, int currLife, uint maxLife, const DragonRoostInfo& dragonRoostInfo, j1Module* listener);
	~DragonRoost();

	void Move(float dt);

private:

	DragonRoostInfo dragonRoostInfo;

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

