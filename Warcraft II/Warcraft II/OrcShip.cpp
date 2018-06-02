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

OrcShip::OrcShip(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const OrcShipInfo& orcShipInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener, false), orcShipInfo(orcShipInfo)
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

	isSpawnedWave = false;

	speed = orcShipInfo.unitInfo.currSpeed - 25;

	movementTimer.Start();
}

void OrcShip::Move(float dt)
{
	animation = &orcShipInfo.down;

	switch(orcShipInfo.orcShipType)
	{
	case ShipType_UP_LEFT:
		if (movementTimer.Read() < 4000) {
		pos.x += speed * dt;
		pos.y += speed * dt;
		animation = &orcShipInfo.downRight;
		}
		else if (movementTimer.Read() >= 30000)
			isRemove = true;
		else if (movementTimer.Read() >= 15000) {
			pos.x -= speed * dt;
			animation = &orcShipInfo.left;
		}
		else if (movementTimer.Read() >= 12000) {
			if (!isSpawnedWave) {
				App->wave->PerformWave(ShipType_UP_LEFT);
				isSpawnedWave = true;
			}
		}
		else if (movementTimer.Read() >= 7000) {
			animation = &orcShipInfo.down;
		}
		else if (movementTimer.Read() >= 4000) {
			pos.y += speed * dt;
			animation = &orcShipInfo.down;
		}
		break;

	case ShipType_BOTTOM:
		if (movementTimer.Read() < 4000) {
			pos.x += speed * dt;
			pos.y -= speed * dt;
			animation = &orcShipInfo.upRight;
		}
		else if (movementTimer.Read() >= 30000)
			isRemove = true;
		else if (movementTimer.Read() >= 15000) {
			pos.y += speed * dt;
			animation = &orcShipInfo.down;
		}
		else if (movementTimer.Read() >= 12000) {
			if (!isSpawnedWave) {
				App->wave->PerformWave(ShipType_BOTTOM);
				isSpawnedWave = true;
			}
			animation = &orcShipInfo.right;
		}
		else if (movementTimer.Read() >= 6000) {
			animation = &orcShipInfo.right;
		}
		else if (movementTimer.Read() >= 4000) {
			pos.x += speed * dt;
			animation = &orcShipInfo.right;
		}
		break;

	case ShipType_BOTTOM_RIGHT:
		if (movementTimer.Read() < 6000) {
			pos.x -= speed * dt;
			pos.y -= speed * dt;
			animation = &orcShipInfo.upLeft;
		}
		else if (movementTimer.Read() >= 30000)
			isRemove = true;
		else if (movementTimer.Read() >= 15000) {
			pos.x += speed * dt;
			animation = &orcShipInfo.right;
		}
		else if (movementTimer.Read() >= 12000) {
			if (!isSpawnedWave) {
				App->wave->PerformWave(ShipType_BOTTOM_RIGHT);
				isSpawnedWave = true;
			}
			animation = &orcShipInfo.up;
		}
		else if (movementTimer.Read() >= 9000) {
			animation = &orcShipInfo.up;
		}
		else if (movementTimer.Read() >= 6000) {
			pos.y -= speed * dt;
			animation = &orcShipInfo.up;
		}
		break;

	case ShipType_UP_RIGHT:
		if (movementTimer.Read() < 8000) {
			pos.x -= speed * dt;
			animation = &orcShipInfo.left;
		}
		else if (movementTimer.Read() >= 30000)
			isRemove = true;
		else if (movementTimer.Read() >= 15000) {
			pos.x += speed * dt;
			animation = &orcShipInfo.right;
		}
		else if (movementTimer.Read() >= 12000) {
			if (!isSpawnedWave) {
				App->wave->PerformWave(ShipType_UP_RIGHT);
				isSpawnedWave = true;
			}
			animation = &orcShipInfo.down;
		}
		else if (movementTimer.Read() >= 9500) {
			animation = &orcShipInfo.down;
		}
		else if (movementTimer.Read() >= 8000) {
			pos.y += speed * dt;
			animation = &orcShipInfo.down;
		}

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

