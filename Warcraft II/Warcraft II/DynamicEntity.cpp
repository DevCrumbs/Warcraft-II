#include "Defs.h"
#include "p2Log.h"

#include "DynamicEntity.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1EntityFactory.h"
#include "j1Input.h"
#include "j1Movement.h"
#include "j1PathManager.h"
#include "Goal.h"
#include "j1Player.h"
#include "j1Particles.h"
#include "j1Printer.h"

#include "UILifeBar.h"

#include "Brofiler\Brofiler.h"

DynamicEntity::DynamicEntity(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, j1Module* listener) : Entity(pos, size, currLife, maxLife, listener), unitInfo(unitInfo)
{
	this->entityType = EntityCategory_DYNAMIC_ENTITY;

	// Movement
	/// UnitInfo
	if (this->unitInfo.currSpeed == 0.0f)
		this->unitInfo.currSpeed = this->unitInfo.maxSpeed;

	/// SingleUnit
	singleUnit = new SingleUnit(this, nullptr);
	App->movement->CreateGroupFromUnit(this);

	/// PathPlanner
	pathPlanner = new PathPlanner(this);

	// Goals
	brain = new Goal_Think(this);

	// LifeBar
	lifeBarMarginX = 8;
	lifeBarMarginY = 32;
}

DynamicEntity::~DynamicEntity()
{
	// Remove Goals
	brain->RemoveAllSubgoals();

	if (brain != nullptr)
		delete brain;
	brain = nullptr;

	// Remove Movement
	if (pathPlanner != nullptr)
		delete pathPlanner;
	pathPlanner = nullptr;

	if (singleUnit != nullptr)
		delete singleUnit;
	singleUnit = nullptr;

	if (!App->gui->isGuiCleanUp) {

		if (lifeBar != nullptr) {
			lifeBar->toRemove = true;
			lifeBar = nullptr;
		}
	}

	animation = nullptr;

	isDead = true;
	isSpawned = true;

	// Remove Attack
	App->entities->InvalidateTargetInfo(this);
	currTarget = nullptr;

	// Remove Colliders
	if (sightRadiusCollider != nullptr)
		sightRadiusCollider->isRemove = true;
	sightRadiusCollider = nullptr;

	if (attackRadiusCollider != nullptr)
		attackRadiusCollider->isRemove = true;
	attackRadiusCollider = nullptr;

	color = ColorWhite;
	colorName = "White";

	// ----

	// Attack
	isStill = true;

	if (currTarget != nullptr)
		currTarget = nullptr;

	if (newTarget != nullptr)
		newTarget = nullptr;

	list<TargetInfo*>::const_iterator it = targets.begin();

	while (it != targets.end()) {

		delete *it;
		it++;
	}
	targets.clear();
}

void DynamicEntity::Move(float dt) {}

void DynamicEntity::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr) {
		//App->render->Blit(sprites, pos.x, pos.y, &(animation->GetCurrentFrame()));
		App->printer->PrintSprite({ (int)pos.x, (int)pos.y }, sprites, animation->GetCurrentFrame(), Layers_Entities);
	}
	if (isSelected)
		DebugDrawSelected();
}

void DynamicEntity::DebugDrawSelected()
{
	SDL_Rect entitySize = { pos.x, pos.y, size.x, size.y };
	//App->render->DrawQuad(entitySize, 255, 255, 255, 255, false);
	App->printer->PrintQuad(entitySize, ColorWhite);
}

void DynamicEntity::OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState) {}

void DynamicEntity::HandleInput(EntitiesEvent &EntityEvent)
{
	iPoint mouse_pos;
	App->input->GetMousePosition(mouse_pos.x, mouse_pos.y);

	switch (EntityEvent) {

	case EntitiesEvent_NONE:
		if (MouseHover()) {
			EntityEvent = EntitiesEvent_HOVER;
			listener->OnDynamicEntitiesEvent((DynamicEntity*)this, EntityEvent);
			break;
		}
		break;
	case EntitiesEvent_HOVER:

		if (!MouseHover()) {

			EntityEvent = EntitiesEvent_LEAVE;
			break;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_PRESSED && !App->gui->IsMouseOnUI()) {

			EntityEvent = EntitiesEvent_LEFT_CLICK;
			listener->OnDynamicEntitiesEvent((DynamicEntity*)this, EntityEvent);
			EntityEvent = EntitiesEvent_HOVER;
			break;

		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_PRESSED && !App->gui->IsMouseOnUI()) {

			EntityEvent = EntitiesEvent_RIGHT_CLICK;
			listener->OnDynamicEntitiesEvent((DynamicEntity*)this, EntityEvent);
			EntityEvent = EntitiesEvent_HOVER;
			break;

		}
		break;
	case EntitiesEvent_CREATED:

		if (MouseHover()) {
			EntityEvent = EntitiesEvent_HOVER;
			listener->OnDynamicEntitiesEvent((DynamicEntity*)this, EntityEvent);
			break;
		}
		break;

	case EntitiesEvent_LEAVE:
		listener->OnDynamicEntitiesEvent((DynamicEntity*)this, EntityEvent);
		EntityEvent = EntitiesEvent_NONE;

		break;
	}
}


bool DynamicEntity::MouseHover() const
{
	int x, y;
	App->input->GetMousePosition(x, y);
	uint scale = App->win->GetScale();

	iPoint screen_pos;
	screen_pos.x = pos.x + App->render->camera.x;
	screen_pos.y = pos.y + App->render->camera.y;

	return x > screen_pos.x / scale && x < screen_pos.x / scale + size.x && y > screen_pos.y / scale && y < screen_pos.y / scale + size.y;
}

// -------------------------------------------------------------

Animation* DynamicEntity::GetAnimation() const
{
	return animation;
}

Goal_Think* DynamicEntity::GetBrain() const
{
	return brain;
}

// UnitInfo
float DynamicEntity::GetSpeed() const
{
	return unitInfo.currSpeed;
}

uint DynamicEntity::GetPriority() const
{
	return unitInfo.priority;
}

uint DynamicEntity::GetDamage(Entity* target) const
{
	if (target == nullptr)
		return 0;

	if (target->entityType == EntityCategory_DYNAMIC_ENTITY) {

		DynamicEntity* dynEnt = (DynamicEntity*)target;

		switch (dynEnt->dynamicEntityType) {

		case EntityType_FOOTMAN:
		case EntityType_GRUNT:

			return unitInfo.heavyDamage;
			break;

		case EntityType_ELVEN_ARCHER:
		case EntityType_TROLL_AXETHROWER:

			return unitInfo.lightDamage;
			break;

		case EntityType_GRYPHON_RIDER:
		case EntityType_DRAGON:

			return unitInfo.airDamage;
			break;

			// Critters
		case EntityType_SHEEP:

		{
			int damage = dynEnt->GetMaxLife();
			damage /= 3;
			return damage;
		}
		break;

		case EntityType_BOAR:

		{
			int damage = dynEnt->GetMaxLife();
			damage /= 4;
			return damage;
		}
		break;
		}
	}
	else if (target->entityType == EntityCategory_STATIC_ENTITY)

		return unitInfo.towerDamage;

	return 0;
}

UILifeBar* DynamicEntity::GetLifeBar() const
{
	return lifeBar;
}

// State machine
void DynamicEntity::UnitStateMachine(float dt) {}

void DynamicEntity::SetUnitState(UnitState unitState)
{
	this->unitState = unitState;
}

UnitState DynamicEntity::GetUnitState() const
{
	return unitState;
}

// Movement
SingleUnit* DynamicEntity::GetSingleUnit() const
{
	return singleUnit;
}

PathPlanner* DynamicEntity::GetPathPlanner() const
{
	return pathPlanner;
}

void DynamicEntity::SetIsStill(bool isStill)
{
	this->isStill = isStill;
}

bool DynamicEntity::IsStill() const
{
	return isStill;
}

// Animations
void DynamicEntity::LoadAnimationsSpeed() {}

void DynamicEntity::UpdateAnimationsSpeed(float dt) {}

bool DynamicEntity::ChangeAnimation() { return true; }

// Direction
void DynamicEntity::SetUnitDirection(UnitDirection unitDirection)
{
	switch (unitDirection) {

	case UnitDirection_NoDirection:

		direction.x = 0.0f;
		direction.y = 0.0f;
		break;

	case UnitDirection_Up:

		direction.x = 0.0f;
		direction.y = -1.0f;
		break;

	case UnitDirection_Down:

		direction.x = 0.0f;
		direction.y = 1.0f;
		break;

	case UnitDirection_Left:

		direction.x = -1.0f;
		direction.y = 0.0f;
		break;

	case UnitDirection_Right:

		direction.x = 1.0f;
		direction.y = 0.0f;
		break;

	case UnitDirection_UpLeft:

		direction.x = -1.0f;
		direction.y = -1.0f;
		break;

	case UnitDirection_UpRight:

		direction.x = 1.0f;
		direction.y = -1.0f;
		break;

	case UnitDirection_DownLeft:

		direction.x = -1.0f;
		direction.y = 1.0f;
		break;

	case UnitDirection_DownRight:

		direction.x = 1.0f;
		direction.y = 1.0f;
		break;
	}
}

UnitDirection DynamicEntity::GetUnitDirection() const
{
	if (direction.x > 0.0f) {

		if (direction.y > 0.0f)
			return UnitDirection_DownRight;

		else if (direction.y < 0.0f)
			return UnitDirection_UpRight;

		else
			return UnitDirection_Right;
	}
	else if (direction.x < 0.0f) {

		if (direction.y > 0.0f)
			return UnitDirection_DownLeft;

		else if (direction.y < 0.0f)
			return UnitDirection_UpLeft;

		else
			return UnitDirection_Left;
	}
	else {

		if (direction.y > 0.0f)
			return UnitDirection_Down;

		else if (direction.y < 0.0f)
			return UnitDirection_Up;

		else
			return UnitDirection_NoDirection;
	}

	return UnitDirection_NoDirection;
}

UnitDirection DynamicEntity::GetDirection(fPoint direction) const
{
	if (direction.x > 0.0f) {

		if (direction.y > 0.0f)
			return UnitDirection_DownRight;

		else if (direction.y < 0.0f)
			return UnitDirection_UpRight;

		else
			return UnitDirection_Right;
	}
	else if (direction.x < 0.0f) {

		if (direction.y > 0.0f)
			return UnitDirection_DownLeft;

		else if (direction.y < 0.0f)
			return UnitDirection_UpLeft;

		else
			return UnitDirection_Left;
	}
	else {

		if (direction.y > 0.0f)
			return UnitDirection_Down;

		else if (direction.y < 0.0f)
			return UnitDirection_Up;

		else
			return UnitDirection_NoDirection;
	}

	return UnitDirection_NoDirection;
}

void DynamicEntity::SetUnitDirectionByValue(fPoint unitDirection)
{
	direction = unitDirection;
}

fPoint DynamicEntity::GetUnitDirectionByValue() const
{
	return direction;
}

// Collision
ColliderGroup* DynamicEntity::GetSightRadiusCollider() const
{
	return sightRadiusCollider;
}

ColliderGroup* DynamicEntity::GetAttackRadiusCollider() const
{
	return attackRadiusCollider;
}

ColliderGroup* DynamicEntity::CreateRhombusCollider(ColliderType colliderType, uint radius, DistanceHeuristic distanceHeuristic)
{
	vector<Collider*> colliders;

	// Perform a BFS
	queue<iPoint> queue;
	list<iPoint> visited;

	iPoint curr = singleUnit->currTile;
	queue.push(curr);

	while (queue.size() > 0) {

		curr = queue.front();
		queue.pop();

		iPoint neighbors[4];
		neighbors[0].create(curr.x + 1, curr.y + 0);
		neighbors[1].create(curr.x + 0, curr.y + 1);
		neighbors[2].create(curr.x - 1, curr.y + 0);
		neighbors[3].create(curr.x + 0, curr.y - 1);

		/*
		neighbors[4].create(curr.x + 1, curr.y + 1);
		neighbors[5].create(curr.x + 1, curr.y - 1);
		neighbors[6].create(curr.x - 1, curr.y + 1);
		neighbors[7].create(curr.x - 1, curr.y - 1);
		*/

		for (uint i = 0; i < 4; ++i)
		{
			if (App->pathfinding->IsWalkable(neighbors[i]) && CalculateDistance(neighbors[i], singleUnit->currTile, distanceHeuristic) < radius) {

				if (find(visited.begin(), visited.end(), neighbors[i]) == visited.end()) {

					queue.push(neighbors[i]);
					visited.push_back(neighbors[i]);

					iPoint collPos = App->map->MapToWorld(neighbors[i].x, neighbors[i].y);
					SDL_Rect rect = { collPos.x, collPos.y, App->map->data.tileWidth, App->map->data.tileHeight };

					Collider* coll = App->collision->CreateCollider(rect);
					colliders.push_back(coll);
				}
			}
		}
	}

	/*
	vector<Collider*> colliders;
	iPoint currTilePos = App->map->MapToWorld(singleUnit->currTile.x, singleUnit->currTile.y);

	int sign = 1;
	for (int y = -(int)radius + 1; y < (int)radius; ++y) {

	if (y == 0)
	sign *= -1;

	for (int x = (-sign * y) - (int)radius + 1; x < (int)radius + (sign * y); ++x) {

	SDL_Rect rect = { currTilePos.x + x * App->map->data.tile_width, currTilePos.y + y * App->map->data.tile_height, App->map->data.tile_width, App->map->data.tile_height };
	colliders.push_back(App->collision->CreateCollider(rect));
	}
	}
	*/

	return App->collision->CreateAndAddColliderGroup(colliders, colliderType, App->entities, this);
}

void DynamicEntity::UpdateRhombusColliderPos(ColliderGroup* collider, uint radius, DistanceHeuristic distanceHeuristic)
{
	collider->RemoveAllColliders();

	// 1. Create the small colliders

	// Perform a BFS
	queue<iPoint> queue;
	list<iPoint> visited;

	iPoint curr = singleUnit->currTile;
	queue.push(curr);

	while (queue.size() > 0) {

		curr = queue.front();
		queue.pop();

		iPoint neighbors[4];
		neighbors[0].create(curr.x + 1, curr.y + 0);
		neighbors[1].create(curr.x + 0, curr.y + 1);
		neighbors[2].create(curr.x - 1, curr.y + 0);
		neighbors[3].create(curr.x + 0, curr.y - 1);

		/*
		neighbors[4].create(curr.x + 1, curr.y + 1);
		neighbors[5].create(curr.x + 1, curr.y - 1);
		neighbors[6].create(curr.x - 1, curr.y + 1);
		neighbors[7].create(curr.x - 1, curr.y - 1);
		*/

		for (uint i = 0; i < 4; ++i)
		{
			if (App->pathfinding->IsWalkable(neighbors[i]) && CalculateDistance(neighbors[i], singleUnit->currTile, distanceHeuristic) < radius) {

				if (find(visited.begin(), visited.end(), neighbors[i]) == visited.end()) {

					queue.push(neighbors[i]);
					visited.push_back(neighbors[i]);

					iPoint collPos = App->map->MapToWorld(neighbors[i].x, neighbors[i].y);
					SDL_Rect rect = { collPos.x, collPos.y, App->map->data.tileWidth, App->map->data.tileHeight };

					Collider* coll = App->collision->CreateCollider(rect);
					App->collision->AddColliderToAColliderGroup(collider, coll);
				}
			}
		}
	}

	// 2. Create/Update the offset collider
	collider->CreateOffsetCollider();

	/*
	vector<Collider*>::const_iterator it = collider->colliders.begin();

	while (it != collider->colliders.end()) {

	iPoint currTilePos = App->map->MapToWorld(singleUnit->currTile.x, singleUnit->currTile.y);

	int sign = 1;
	for (int y = -(int)radius + 1; y < (int)radius; ++y) {

	if (y == 0)
	sign *= -1;

	for (int x = (-sign * y) - (int)radius + 1; x < (int)radius + (sign * y); ++x) {

	iPoint definitivePos = { currTilePos.x + x * App->map->data.tile_width, currTilePos.y + y * App->map->data.tile_height };
	iPoint definitiveTile = App->map->WorldToMap(definitivePos.x, definitivePos.y);

	SDL_Rect rect = { definitivePos.x, definitivePos.y, App->map->data.tile_width, App->map->data.tile_height };
	(*it)->SetPos(rect.x, rect.y);

	it++;
	}
	}
	}
	*/
}

// Attack
/// Unit attacks a target
Entity* DynamicEntity::GetCurrTarget() const
{
	if (currTarget != nullptr)
		return currTarget->target;
	else
		return nullptr;
}

bool DynamicEntity::SetCurrTarget(Entity* target)
{
	bool ret = false;

	if (target == nullptr)
		return false;

	if (currTarget != nullptr) {

		if (target == currTarget->target)
			return true;
	}

	list<TargetInfo*>::const_iterator it = targets.begin();

	TargetInfo* targetInfo = nullptr;

	// Check if the target is already in the targets list (this means that the TargetInfo exists)
	while (it != targets.end()) {

		if ((*it)->target == target) {

			targetInfo = *it;
			break;
		}
		it++;
	}

	// If the target is not in the targets list, create a new TargetInfo
	if (targetInfo == nullptr) {

		targetInfo = new TargetInfo();
		targetInfo->target = target;

		targets.push_front(targetInfo);
	}

	if (targetInfo != nullptr) {

		// Only push it if it does not have to be removed
		if (!targetInfo->isRemoved) {

			currTarget = targetInfo;
			ret = true;
		}
	}

	return ret;
}

bool DynamicEntity::SetIsRemovedTargetInfo(Entity* target)
{
	if (target == nullptr)
		return false;

	// Set isRemoved to true
	list<TargetInfo*>::const_iterator it = targets.begin();

	while (it != targets.end()) {

		if ((*it)->target == target) {

			(*it)->isRemoved = true;
			return true;
		}
		it++;
	}

	return false;
}

bool DynamicEntity::RemoveTargetInfo(TargetInfo* targetInfo)
{
	if (targetInfo == nullptr)
		return false;

	// If the target is the currTarget, set currTarget to nullptr
	if (targetInfo == currTarget)
		currTarget = nullptr;

	// Remove the target from the targets list
	list<TargetInfo*>::const_iterator it = targets.begin();

	while (it != targets.end()) {

		if ((*it)->target == targetInfo->target) {

			delete *it;
			targets.remove(*it);
			return true;
		}
		it++;
	}

	return false;
}

TargetInfo* DynamicEntity::GetBestTargetInfo(ENTITY_CATEGORY entityType) const
{
	// If there are no targets, return null
	if (targets.size() == 0)
		return nullptr;

	// Else, check out the available targets
	TargetInfo* result = nullptr;

	list<TargetInfo*>::const_iterator it = targets.begin();
	bool isChecked = false;

	// Order the targets by their priority (the target closer to the unit has the max priority)
	priority_queue<TargetInfoPriority, vector<TargetInfoPriority>, TargetInfoPriorityComparator> queue;
	TargetInfoPriority priorityTargetInfo;

	while (it != targets.end()) {

		if (!(*it)->isRemoved) {

			// Only dynamic entities
			if ((*it)->target->entityType == EntityCategory_DYNAMIC_ENTITY) {

				DynamicEntity* dynEnt = (DynamicEntity*)(*it)->target;

				if (dynEnt->dynamicEntityType != EntityType_SHEEP && dynEnt->dynamicEntityType != EntityType_BOAR) {

					priorityTargetInfo.targetInfo = *it;
					priorityTargetInfo.priority = (*it)->target->GetPos().DistanceManhattan(pos);
					queue.push(priorityTargetInfo);
				}
			}
		}

		it++;
	}

	TargetInfoPriority curr;
	while (queue.size() > 0) {

		curr = queue.top();
		queue.pop();

		if (!isChecked) {

			result = curr.targetInfo;
			isChecked = true;
		}
		else {

			// Pick the target with the less units attacking them (except for the unit)
			if (result->target->GetAttackingUnitsSize((Entity*)this) > curr.targetInfo->target->GetAttackingUnitsSize((Entity*)this))

				result = curr.targetInfo;
		}
	}

	return result;
}

void DynamicEntity::SetHitting(bool isHitting)
{
	this->isHitting = isHitting;
}

bool DynamicEntity::IsHitting() const
{
	return isHitting;
}

// Interact with the map
void DynamicEntity::SetGoldMine(GoldMine* goldMine)
{
	this->goldMine = goldMine;
}

GoldMine* DynamicEntity::GetGoldMine() const
{
	return goldMine;
}

void DynamicEntity::SetUnitGatheringGold(bool isGatheringGold)
{
	this->isGatheringGold = isGatheringGold;
}

bool DynamicEntity::IsUnitGatheringGold() const
{
	return isGatheringGold;
}

void DynamicEntity::SetRunestone(Runestone* runestone)
{
	this->runestone = runestone;
}

Runestone* DynamicEntity::GetRunestone() const
{
	return runestone;
}

void DynamicEntity::SetUnitHealingRunestone(bool isHealingRunestone)
{
	this->isHealingRunestone = isHealingRunestone;
}

bool DynamicEntity::IsUnitHealingRunestone() const
{
	return isHealingRunestone;
}

void DynamicEntity::SetPrisoner(DynamicEntity* prisoner) 
{
	this->prisoner = prisoner;
}

DynamicEntity* DynamicEntity::GetPrisoner() const 
{
	return prisoner;
}

void DynamicEntity::SetUnitRescuePrisoner(bool isRescuingPrisoner) 
{
	this->isRescuingPrisoner = isRescuingPrisoner;
}

bool DynamicEntity::IsUnitRescuingPrisoner() const 
{
	return isRescuingPrisoner;
}

// Player commands
bool DynamicEntity::SetUnitCommand(UnitCommand unitCommand)
{
	bool ret = false;

	if (this->unitCommand == UnitCommand_NoCommand) {

		this->unitCommand = unitCommand;
		ret = true;
	}

	return ret;
}

UnitCommand DynamicEntity::GetUnitCommand() const
{
	return unitCommand;
}

// TargetInfo struct ---------------------------------------------------------------------------------

bool TargetInfo::IsTargetPresent() const
{
	if (target == nullptr)
		return false;

	// The target is dead
	if (target->GetCurrLife() <= 0)
		return false;

	return true;
}

// Blit
void DynamicEntity::SetBlitState(bool isBlit)
{
	this->isBlit = isBlit;
}

bool DynamicEntity::GetBlitState() const
{
	return isBlit;
}