#include "Defs.h"
#include "p2Log.h"

#include "OrcShip.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1EntityFactory.h"
#include "j1Movement.h"
#include "j1PathManager.h"
#include "Goal.h"
#include "j1Audio.h"
#include "j1Player.h"
#include "j1Printer.h"
#include "j1EnemyWave.h"

#include "j1Scene.h" // isFrameByFrame
#include "j1Input.h" // isFrameByFrame

#include "Brofiler\Brofiler.h"

// Ignores the walkability of the map

OrcShip::OrcShip(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const OrcShipInfo& orcShipInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), orcShipInfo(orcShipInfo)
{
	// XML loading
	/// Animations
	OrcShipInfo info = (OrcShipInfo&)App->entities->GetUnitInfo(EntityType_ORC_SHIP);
	this->unitInfo = this->orcShipInfo.unitInfo;
	this->orcShipInfo.up = info.up;
	this->orcShipInfo.down = info.down;
	this->orcShipInfo.left = info.left;
	this->orcShipInfo.right = info.right;
	this->orcShipInfo.upLeft = info.upLeft;
	this->orcShipInfo.upRight = info.upRight;
	this->orcShipInfo.downLeft = info.downLeft;
	this->orcShipInfo.downRight = info.downRight;

	this->size = this->unitInfo.size;
	offsetSize = this->unitInfo.offsetSize;

	// IA
	spawnTile = { singleUnit->currTile.x, singleUnit->currTile.y };
	iPoint spawnPos = App->map->MapToWorld(spawnTile.x, spawnTile.y);

	isSpawnedWave = false;
}

void OrcShip::Move(float dt)
{
	animation = &orcShipInfo.down;

	switch(orcShipInfo.orcShipType)
	{
	case ShipType_UP_LEFT:
		if (movementTimer.Read() < 5000) {
		pos.x += orcShipInfo.unitInfo.maxSpeed * dt;
		pos.y += orcShipInfo.unitInfo.maxSpeed * dt;
		animation = &orcShipInfo.downRight;
		}
		else if (movementTimer.Read() >= 15000) {
			pos.x -= orcShipInfo.unitInfo.maxSpeed * dt;
			animation = &orcShipInfo.left;
		}
		else if (movementTimer.Read() >= 10000) {
			if (!isSpawnedWave) {
				App->wave->PerformWave(ShipType_UP_LEFT);
				isSpawnedWave = true;
			}
		}
		else if (movementTimer.Read() >= 5000) {
			animation = &orcShipInfo.down;
		}
		break;

	case ShipType_BOTTOM:

		break;

	case ShipType_BOTTOM_RIGHT:
		
		break;

	case ShipType_UP_RIGHT:
		
		break;	
	}

	// --------------------------------------------------------------------
	
}

void OrcShip::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr)
	{
		// Not draw if not on fow sight
		//if (App->fow->IsOnSight(pos))
		{
			fPoint offset = { 0.0f,0.0f };
			offset = { animation->GetCurrentFrame().w / 2.8f, animation->GetCurrentFrame().h / 2.5f };

			App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_Entities);
		}
	}

	//if (isSelected)
	//DebugDrawSelected();
}

void OrcShip::DebugDrawSelected()
{
	const SDL_Rect entitySize = { pos.x + offsetSize.x, pos.y + offsetSize.y, size.x, size.y };
	App->printer->PrintQuad(entitySize, color);
}

// -------------------------------------------------------------

