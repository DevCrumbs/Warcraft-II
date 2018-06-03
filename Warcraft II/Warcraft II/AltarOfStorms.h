#ifndef __AltarOfStorms_H__
#define __AltarOfStorms_H__

#include "StaticEntity.h"

struct AltarOfStormsInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class AltarOfStorms :public StaticEntity
{
public:
	AltarOfStorms(fPoint pos, iPoint size, int currLife, uint maxLife, const AltarOfStormsInfo& altarOfStormsInfo, j1Module* listener);
	~AltarOfStorms();

	void Move(float dt);

private:

	AltarOfStormsInfo altarOfStormsInfo;

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

