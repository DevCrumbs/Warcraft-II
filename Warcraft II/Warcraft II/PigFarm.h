#ifndef __PigFarm_H__
#define __PigFarm_H__

#include "StaticEntity.h"

struct PigFarmInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};


class PigFarm : public StaticEntity
{
public:

	PigFarm(fPoint pos, iPoint size, int currLife, uint maxLife, const PigFarmInfo& pigFarmInfo, j1Module* listener);
	~PigFarm();

	void Move(float dt);

private:

	PigFarmInfo pigFarmInfo;

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
