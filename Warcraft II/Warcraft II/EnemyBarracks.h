#ifndef __EnemyBarracks_H__
#define __EnemyBarracks_H__

#include "StaticEntity.h"

struct EnemyBarracksInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class EnemyBarracks :public StaticEntity
{
public:
	EnemyBarracks(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyBarracksInfo& enemyBarracksInfo, j1Module* listener);
	~EnemyBarracks();

	void Move(float dt);

private:

	EnemyBarracksInfo enemyBarracksInfo;

	// Reconstruction
	Particle* peon = nullptr;

	float secondsReconstruction = 0.0f;

	float startReconstructionTimer = 0.0f;
	float inProgressReconstructionTimer = 0.0f;

	bool isRestartReconstructionTimer = false;
	bool isStartReconstructionTimer = false;
	bool isInProgressReconstructionTimer = false;

	BuildingState buildingStateBeforeReconstruction = BuildingState_NoState;

	// Respawn
	float secondsRespawn = 0.0f;

	float respawnTimer = 0.0f;
	bool isRespawnTimer = false;
};

#endif

