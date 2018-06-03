#ifndef __GreatHall_H__
#define __GreatHall_H__

#include "StaticEntity.h"

struct GreatHallInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class GreatHall :public StaticEntity
{
public:
	GreatHall(fPoint pos, iPoint size, int currLife, uint maxLife, const GreatHallInfo& greatHallInfo, j1Module* listener);
	~GreatHall();

	void Move(float dt);

private:

	GreatHallInfo greatHallInfo;

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
