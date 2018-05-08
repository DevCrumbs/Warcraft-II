#include "Defs.h"
#include "p2Log.h"

#include "CritterSheep.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Particles.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1EntityFactory.h"
#include "j1Movement.h"
#include "j1PathManager.h"
#include "Goal.h"
#include "j1Audio.h"
#include "j1Printer.h"

#include "j1Scene.h" // isFrameByFrame
#include "j1Input.h" // isFrameByFrame

#include "Brofiler\Brofiler.h"

CritterSheep::CritterSheep(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const CritterSheepInfo& critterSheepInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), critterSheepInfo(critterSheepInfo)
{
	// XML loading
	/// Animations
	CritterSheepInfo info = (CritterSheepInfo&)App->entities->GetUnitInfo(EntityType_SHEEP);
	this->unitInfo = this->critterSheepInfo.unitInfo;
	this->critterSheepInfo.up = info.up;
	this->critterSheepInfo.down = info.down;
	this->critterSheepInfo.left = info.left;
	this->critterSheepInfo.right = info.right;
	this->critterSheepInfo.upLeft = info.upLeft;
	this->critterSheepInfo.upRight = info.upRight;
	this->critterSheepInfo.downLeft = info.downLeft;
	this->critterSheepInfo.downRight = info.downRight;
	this->critterSheepInfo.deathUpLeft = info.deathUpLeft;
	this->critterSheepInfo.deathUpRight = info.deathUpRight;
	this->critterSheepInfo.deathDownLeft = info.deathDownLeft;
	this->critterSheepInfo.deathDownRight = info.deathDownRight;

	size = this->unitInfo.size;
	offsetSize = this->unitInfo.offsetSize;

	LoadAnimationsSpeed();

	// Initialize the goals
	brain->RemoveAllSubgoals();

	brain->AddGoal_Wander(5, singleUnit->currTile, true, 2, 4, 2, 6, 3);

	// Collisions
	CreateEntityCollider(EntitySide_NoSide);
	entityCollider->isTrigger = true;
}

CritterSheep::~CritterSheep()
{
	if (lastPaw != nullptr)
		lastPaw->isDelete = true;
	lastPaw = nullptr;
}

void CritterSheep::Move(float dt)
{
	// Save mouse position (world and map coords)
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint mousePos = App->render->ScreenToWorld(x, y);
	iPoint mouseTile = App->map->WorldToMap(mousePos.x, mousePos.y);
	iPoint mouseTilePos = App->map->MapToWorld(mouseTile.x, mouseTile.y);

	// ---------------------------------------------------------------------

	// Is the unit dead?
	/// The unit must fit the tile (it is more attractive for the player)
	if (currLife <= 0
		&& unitState != UnitState_Die
		&& singleUnit->IsFittingTile()
		&& !isDead) {

		App->audio->PlayFx(App->audio->GetFX().sheepDeath, 0);

		isDead = true;

		if (lastPaw != nullptr)
			lastPaw->isDelete = true;

		// Remove the entity from the unitsSelected list
		App->entities->RemoveUnitFromUnitsSelected(this);

		brain->RemoveAllSubgoals();

		unitState = UnitState_Idle;

		// Remove Movement (so other units can walk above them)
		App->entities->InvalidateMovementEntity(this);

		// Remove any path request
		pathPlanner->SetSearchRequested(false);
		pathPlanner->SetSearchCompleted(false);
		App->pathmanager->UnRegister(pathPlanner);

		if (singleUnit != nullptr)
			delete singleUnit;
		singleUnit = nullptr;

		// Invalidate colliders
		entityCollider->isValid = false;
	}

	if (!isDead)

		UpdatePaws();

	// PROCESS THE CURRENTLY ACTIVE GOAL
	brain->Process(dt);

	UnitStateMachine(dt);

	// Update animations
	ChangeAnimation();

	if (!isDead)

		// Update colliders
		UpdateEntityColliderPos();
}

void CritterSheep::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr)
		// Not draw if not on fow sight
		if (App->fow->IsOnSight(pos))
			App->printer->PrintSprite({ (int)pos.x, (int)pos.y }, sprites, animation->GetCurrentFrame(), Layers_Entities);

	if (isSelected)
		DebugDrawSelected();
}

void CritterSheep::DebugDrawSelected()
{
	const SDL_Rect entitySize = { pos.x + offsetSize.x, pos.y + offsetSize.y, size.x, size.y };
	App->printer->PrintQuad(entitySize, color);
}

void CritterSheep::OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState)
{
}

// State machine
void CritterSheep::UnitStateMachine(float dt)
{
	switch (unitState) {

	case UnitState_Wander:

		break;

	case UnitState_Die:

		// Remove the corpse when a certain time is reached
		if (deadTimer.ReadSec() >= TIME_REMOVE_CORPSE)
			isRemove = true;

		break;

	case UnitState_Idle:
	case UnitState_NoState:
	default:

		break;
	}
}

// -------------------------------------------------------------

// Animations
bool CritterSheep::ChangeAnimation()
{
	bool ret = false;

	// The unit is dead
	if (isDead) {

		if (unitState != UnitState_Die) {
			unitState = UnitState_Die;
			deadTimer.Start();
		}

		animation = &critterSheepInfo.deathDownLeft;
		ret = true;

		return ret;
	}

	// The unit is moving
	else {

		switch (GetUnitDirection()) {

		case UnitDirection_Up:

			animation = &critterSheepInfo.up;
			ret = true;
			break;

		case UnitDirection_NoDirection:
		case UnitDirection_Down:

			animation = &critterSheepInfo.down;
			ret = true;
			break;

		case UnitDirection_Left:

			animation = &critterSheepInfo.left;
			ret = true;
			break;

		case UnitDirection_Right:

			animation = &critterSheepInfo.right;
			ret = true;
			break;

		case UnitDirection_UpLeft:

			animation = &critterSheepInfo.upLeft;
			ret = true;
			break;

		case UnitDirection_UpRight:

			animation = &critterSheepInfo.upRight;
			ret = true;
			break;

		case UnitDirection_DownLeft:

			animation = &critterSheepInfo.downLeft;
			ret = true;
			break;

		case UnitDirection_DownRight:

			animation = &critterSheepInfo.downRight;
			ret = true;
			break;
		}
		return ret;
	}
	return ret;
}

// Paws
void CritterSheep::UpdatePaws()
{
	// Add footprints along the critter's path
	if (singleUnit->currTile.x != -1 && singleUnit->currTile.y != -1
		&& !isStill) {

		if (lastPawTile != singleUnit->currTile) {

			// Remove last paw
			if (lastPaw != nullptr) {

				lastPaw->isRemove = true;
				lastPaw = nullptr;
			}

			// Create a new paw
			iPoint currTilePos = App->map->MapToWorld(singleUnit->currTile.x, singleUnit->currTile.y);

			fPoint orientation = { (float)singleUnit->currTile.x - (float)lastPawTile.x, (float)singleUnit->currTile.y - (float)lastPawTile.y };

			float m = sqrtf(pow(orientation.x, 2.0f) + pow(orientation.y, 2.0f));

			if (m > 0.0f) {
				orientation.x /= m;
				orientation.y /= m;
			}

			double angle = (atan2(orientation.y, orientation.x) * 180.0f / (float)M_PI);

			if (angle >= 360.0f)
				angle = 0.0f;

			lastPaw = App->particles->AddParticle(App->particles->sheepPaws, currTilePos, { 0.0f,0.0f }, 0.0f, 0, 0, angle);

			lastPawTile = singleUnit->currTile;
		}
	}
}