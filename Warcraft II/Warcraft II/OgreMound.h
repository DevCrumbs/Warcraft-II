#ifndef __OgreMound_H__
#define __OgreMound_H__

#include "StaticEntity.h"

struct OgreMoundInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class OgreMound :public StaticEntity
{
public:
	OgreMound(fPoint pos, iPoint size, int currLife, uint maxLife, const OgreMoundInfo& ogreMoundInfo, j1Module* listener);
	~OgreMound();

	void Move(float dt);

private:

	OgreMoundInfo ogreMoundInfo;

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

