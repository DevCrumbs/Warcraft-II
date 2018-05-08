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

	LoadAnimationsSpeed();

	// IA
	spawnTile = { singleUnit->currTile.x, singleUnit->currTile.y };
	iPoint spawnPos = App->map->MapToWorld(spawnTile.x, spawnTile.y);
}

void OrcShip::Move(float dt)
{
	// Save mouse position (world and map coords)
	switch(orcShipType)
	{

	case ShipType_UP_LEFT:
	
		break;

	case ShipType_BOTTOM:

		break;

	case ShipType_BOTTOM_RIGHT:
		
		break;

	case ShipType_UP_RIGHT:
		
		break;	
	}

	// ---------------------------------------------------------------------
	UpdateAnimationsSpeed(dt);
	ChangeAnimation();
}

void OrcShip::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr)
	{
		// Not draw if not on fow sight
		if (App->fow->IsOnSight(pos))
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

// Animations
void OrcShip::LoadAnimationsSpeed()
{
	upSpeed = orcShipInfo.up.speed;
	downSpeed = orcShipInfo.down.speed;
	leftSpeed = orcShipInfo.left.speed;
	rightSpeed = orcShipInfo.right.speed;
	upLeftSpeed = orcShipInfo.upLeft.speed;
	upRightSpeed = orcShipInfo.upRight.speed;
	downLeftSpeed = orcShipInfo.downLeft.speed;
	downRightSpeed = orcShipInfo.downRight.speed;
}

void OrcShip::UpdateAnimationsSpeed(float dt)
{
	orcShipInfo.up.speed = upSpeed * dt;
	orcShipInfo.down.speed = downSpeed * dt;
	orcShipInfo.left.speed = leftSpeed * dt;
	orcShipInfo.right.speed = rightSpeed * dt;
	orcShipInfo.upLeft.speed = upLeftSpeed * dt;
	orcShipInfo.upRight.speed = upRightSpeed * dt;
	orcShipInfo.downLeft.speed = downLeftSpeed * dt;
	orcShipInfo.downRight.speed = downRightSpeed * dt;
}

bool OrcShip::ChangeAnimation()
{
	bool ret = false;

	// The unit is either moving or still
	switch (GetUnitDirection()) {

	case UnitDirection_Up:

		if (isStill) {

			orcShipInfo.up.loop = false;
			orcShipInfo.up.Reset();
			orcShipInfo.up.speed = 0.0f;
		}
		else
			orcShipInfo.up.loop = true;

		animation = &orcShipInfo.up;

		ret = true;
		break;

	case UnitDirection_NoDirection:
	case UnitDirection_Down:

		if (isStill) {

			orcShipInfo.down.loop = false;
			orcShipInfo.down.Reset();
			orcShipInfo.down.speed = 0.0f;
		}
		else
			orcShipInfo.down.loop = true;

		animation = &orcShipInfo.down;

		ret = true;
		break;

	case UnitDirection_Left:

		if (isStill) {

			orcShipInfo.left.loop = false;
			orcShipInfo.left.Reset();
			orcShipInfo.left.speed = 0.0f;
		}
		else
			orcShipInfo.left.loop = true;

		animation = &orcShipInfo.left;

		ret = true;
		break;

	case UnitDirection_Right:

		if (isStill) {

			orcShipInfo.right.loop = false;
			orcShipInfo.right.Reset();
			orcShipInfo.right.speed = 0.0f;
		}
		else
			orcShipInfo.right.loop = true;

		animation = &orcShipInfo.right;

		ret = true;
		break;

	case UnitDirection_UpLeft:

		if (isStill) {

			orcShipInfo.upLeft.loop = false;
			orcShipInfo.upLeft.Reset();
			orcShipInfo.upLeft.speed = 0.0f;
		}
		else
			orcShipInfo.upLeft.loop = true;

		animation = &orcShipInfo.upLeft;

		ret = true;
		break;

	case UnitDirection_UpRight:

		if (isStill) {

			orcShipInfo.upRight.loop = false;
			orcShipInfo.upRight.Reset();
			orcShipInfo.upRight.speed = 0.0f;
		}
		else
			orcShipInfo.upRight.loop = true;

		animation = &orcShipInfo.upRight;

		ret = true;
		break;

	case UnitDirection_DownLeft:

		if (isStill) {

			orcShipInfo.downLeft.loop = false;
			orcShipInfo.downLeft.Reset();
			orcShipInfo.downLeft.speed = 0.0f;
		}
		else
			orcShipInfo.downLeft.loop = true;

		animation = &orcShipInfo.downLeft;

		ret = true;
		break;

	case UnitDirection_DownRight:

		if (isStill) {

			orcShipInfo.downRight.loop = false;
			orcShipInfo.downRight.Reset();
			orcShipInfo.downRight.speed = 0.0f;
		}
		else
			orcShipInfo.downRight.loop = true;

		animation = &orcShipInfo.downRight;

		ret = true;
		break;
	}
	return ret;
}