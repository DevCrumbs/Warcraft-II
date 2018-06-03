#ifndef __TempleOfTheDamned_H__
#define __TempleOfTheDamned_H__

#include "StaticEntity.h"

struct TempleOfTheDamnedInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class TempleOfTheDamned :public StaticEntity
{
public:
	TempleOfTheDamned(fPoint pos, iPoint size, int currLife, uint maxLife, const TempleOfTheDamnedInfo& templeOfTheDamnedInfo, j1Module* listener);
	~TempleOfTheDamned();

	void Move(float dt);

private:

	TempleOfTheDamnedInfo templeOfTheDamnedInfo;

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

