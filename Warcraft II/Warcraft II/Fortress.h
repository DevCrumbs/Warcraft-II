#ifndef __Fortress_H__
#define __Fortress_H__

#include "StaticEntity.h"

struct FortressInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class Fortress :public StaticEntity
{
public:
	Fortress(fPoint pos, iPoint size, int currLife, uint maxLife, const FortressInfo& fortressInfo, j1Module* listener);
	~Fortress();

	void Move(float dt);

private:

	FortressInfo fortressInfo;

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

