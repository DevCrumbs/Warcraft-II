#ifndef __GoldMine_H__
#define __GoldMine_H__

#include "StaticEntity.h"

struct GoldMineInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
	SDL_Rect destroyedTexArea = { 0,0,0,0 };
};

enum GoldMineState {

	GoldMineState_Untouched,
	GoldMineState_Gathering,
	GoldMineState_Gathered,
};

class GoldMine :public StaticEntity
{
public:

	GoldMine(fPoint pos, iPoint size, int currLife, uint maxLife, const GoldMineInfo& goldMineInfo, j1Module* listener);
	~GoldMine() {};

	void Move(float dt);

	// Gather gold
	bool IsUnitGatheringGold() const;
	void SetUnitGatheringGold(bool isUnitGatheringGold);

	// Tex area
	void SwapTexArea();

	// Gold Mine state
	void SetGoldMineState(GoldMineState goldMineState);
	GoldMineState GetGoldMineState() const;

public:

	uint totalGold = 0;
	uint currGold = 0;
	float secondsGathering = 0.0f;
	float currentSec = 0.0f;

private:

	GoldMineInfo goldMineInfo;
	EntitiesEvent entityEvent = EntitiesEvent_NONE;
	GoldMineState goldMineState = GoldMineState_Untouched;

	bool startTimer = true;

	// Gather gold
	bool isUnitGatheringGold = false;
};

#endif //__GoldMine_H__