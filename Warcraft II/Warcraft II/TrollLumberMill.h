#ifndef __TrollLumberMill_H__
#define __TrollLumberMill_H__

#include "StaticEntity.h"

struct TrollLumberMillInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class TrollLumberMill :public StaticEntity
{
public:
	TrollLumberMill(fPoint pos, iPoint size, int currLife, uint maxLife, const TrollLumberMillInfo& trollLumberMillInfo, j1Module* listener);
	~TrollLumberMill();

	void Move(float dt);

private:

	TrollLumberMillInfo trollLumberMillInfo;

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
