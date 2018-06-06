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

DynamicEntity::DynamicEntity(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, j1Module* listener, bool isSingleUnit) : Entity(pos, size, currLife, maxLife, listener), unitInfo(unitInfo)
{
	// Movement
	/// UnitInfo
	if (this->unitInfo.currSpeed == 0.0f)
		this->unitInfo.currSpeed = this->unitInfo.maxSpeed;

	/// SingleUnit
	if (isSingleUnit) {

		singleUnit = new SingleUnit(this, nullptr);
		App->movement->CreateGroupFromUnit(this);

		/// PathPlanner
		pathPlanner = new PathPlanner(this);

		// Goals
		brain = new Goal_Think(this);
	}

	// LifeBar
	lifeBarMarginX = 8;
	lifeBarMarginY = 32;
}

DynamicEntity::~DynamicEntity()
{
	isBlitSavedGroupSelection = false;
	isBlitSelectedGroupSelection = false;

	// Remove Goals
	if (brain != nullptr) {

		brain->RemoveAllSubgoals();
		delete brain;
	}
	brain = nullptr;

	// Remove Movement
	if (pathPlanner != nullptr)
		delete pathPlanner;
	pathPlanner = nullptr;

	if (singleUnit != nullptr)
		delete singleUnit;
	singleUnit = nullptr;

	// Remove Attack
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

	it = targetsToRemove.begin();

	while (it != targetsToRemove.end()) {

		delete *it;
		it++;
	}
	targetsToRemove.clear();

	// Remove Colliders
	if (sightRadiusCollider != nullptr)
		sightRadiusCollider->isRemove = true;
	sightRadiusCollider = nullptr;

	if (attackRadiusCollider != nullptr)
		attackRadiusCollider->isRemove = true;
	attackRadiusCollider = nullptr;

	// Other
	if (!App->gui->isGuiCleanUp) {

		if (lifeBar != nullptr)
			App->gui->RemoveElem((UIElement**)&lifeBar);
		lifeBar = nullptr;
	}

	animation = nullptr;

	isDead = true;
	isSpawned = true;

	color = ColorWhite;
	colorName = "White";
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
	iPoint mousePos = App->render->ScreenToWorld(x, y);
	uint scale = App->win->GetScale();

	/*
	iPoint screen_pos;
	screen_pos.x = pos.x + App->render->camera.x;
	screen_pos.y = pos.y + App->render->camera.y;
	*/

	// An offset value is applied ONLY to the units selection
	iPoint offsetValue = { 15,15 };

	iPoint entityPos = { (int)pos.x + offsetSize.x - offsetValue.x / 2, (int)pos.y + offsetSize.y - offsetValue.y / 2 };
	iPoint entitySize = { size.x + offsetValue.x, size.y + offsetValue.y };

	return mousePos.x > entityPos.x / scale && mousePos.x < entityPos.x / scale + entitySize.x && mousePos.y > entityPos.y / scale && mousePos.y < entityPos.y / scale + entitySize.y;
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
	bool isWalkabilityChecked = true;

	if (dynamicEntityType == EntityType_GRYPHON_RIDER || dynamicEntityType == EntityType_DRAGON)
		isWalkabilityChecked = false;

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
			if (CalculateDistance(neighbors[i], singleUnit->currTile, distanceHeuristic) < radius) {

				if (isWalkabilityChecked) {

					if (App->pathfinding->IsWalkable(neighbors[i])) {

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
				else {

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
	bool isWalkabilityChecked = true;

	if (dynamicEntityType == EntityType_GRYPHON_RIDER || dynamicEntityType == EntityType_DRAGON)
		isWalkabilityChecked = false;

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
			if (CalculateDistance(neighbors[i], singleUnit->currTile, distanceHeuristic) < radius) {

				if (isWalkabilityChecked) {

					if (App->pathfinding->IsWalkable(neighbors[i])) {

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
				else {
				
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
list<TargetInfo*> DynamicEntity::GetTargets() const 
{
	return targets;
}

list<TargetInfo*> DynamicEntity::GetTargetsToRemove() const 
{
	return targetsToRemove;
}

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

		list<TargetInfo*>::iterator it = find(targets.begin(), targets.end(), targetInfo);

		if (*it != nullptr) {

			currTarget = *it;
			newTarget = currTarget;
			ret = true;
		}
	}

	return ret;
}

void DynamicEntity::InvalidateCurrTarget() 
{
	currTarget = nullptr;
}

bool DynamicEntity::UpdateTargetsToRemove() 
{
	list<TargetInfo*>::const_iterator it = targets.begin();

	while (it != targets.end()) {

		bool isRemove = false;

		if ((*it)->target == nullptr)

			isRemove = true;

		else if ((*it)->target->isRemove)

			isRemove = true;

		if (isRemove) {

			// Removing target process --
			if (currTarget == *it)

				InvalidateCurrTarget();

			if ((*it)->isInGoals > 0 && !(*it)->isRemoveNeeded) {

				(*it)->isRemoveNeeded = true;
				targetsToRemove.splice(targetsToRemove.begin(), targets, it);

				it = targets.begin();
				continue;
			}
			else if (!(*it)->isRemoveNeeded) {

				delete *it;
				targets.remove(*it);

				it = targets.begin();
				continue;
			}
			// -- Removing target process
		}
		it++;
	}

	it = targetsToRemove.begin();

	while (it != targetsToRemove.end()) {

		// Removing target process --
		if ((*it)->isInGoals == 0 && (*it)->isRemoveNeeded) {
		
			if (currTarget == *it)

				InvalidateCurrTarget();

			delete *it;
			targetsToRemove.remove(*it);

			it = targetsToRemove.begin();
			continue;
		}
		// -- Removing target process

		it++;
	}

	return false;
}

TargetInfo* DynamicEntity::GetBestTargetInfo(ENTITY_CATEGORY entityCategory, ENTITY_TYPE entityType, bool isCrittersCheck, bool isOnlyCritters) const
{
	// If there are no targets, return null
	if (targets.size() == 0)
		return nullptr;

	if (isCrittersCheck && isOnlyCritters)
		return nullptr;

	// Else, check out the available targets
	TargetInfo* result = nullptr;

	list<TargetInfo*>::const_iterator it = targets.begin();
	bool isChecked = false;

	// Order the targets by their priority (the target closer to the unit has the max priority)
	priority_queue<TargetInfoPriority, vector<TargetInfoPriority>, TargetInfoPriorityComparator> queue;
	TargetInfoPriority priorityTargetInfo;

	while (it != targets.end()) {

		if ((*it)->target->GetIsValid() && !(*it)->target->isRemove && !(*it)->isRemoveNeeded) {

			if ((*it)->target->entityType == entityCategory && entityCategory == EntityCategory_DYNAMIC_ENTITY) {

				DynamicEntity* dynEnt = (DynamicEntity*)(*it)->target;

				bool isCheckDone = false;

				if (entityType == EntityType_NONE)
					isCheckDone = true;
				else if (dynEnt->dynamicEntityType == entityType)			
					isCheckDone = true;
				
				if (isCheckDone) {

					bool isCrittersValid = true;

					if (isCrittersCheck) {

						if (dynEnt->dynamicEntityType == EntityType_SHEEP || dynEnt->dynamicEntityType == EntityType_BOAR)
							isCrittersValid = false;
					}
					else if (isOnlyCritters) {

						if (dynEnt->dynamicEntityType != EntityType_SHEEP && dynEnt->dynamicEntityType != EntityType_BOAR)
							isCrittersValid = false;
					}

					if (isCrittersValid) {

						priorityTargetInfo.targetInfo = *it;
						priorityTargetInfo.priority = (*it)->target->GetPos().DistanceManhattan(pos);
						queue.push(priorityTargetInfo);
					}
				}
			}
			else if ((*it)->target->entityType == entityCategory && entityCategory == EntityCategory_STATIC_ENTITY) {
			
				StaticEntity* statEnt = (StaticEntity*)(*it)->target;

				if (statEnt->staticEntityType != EntityType_TOWN_HALL) {

					priorityTargetInfo.targetInfo = *it;
					priorityTargetInfo.priority = (*it)->target->GetPos().DistanceManhattan(pos);
					queue.push(priorityTargetInfo);
				}
			}
			else if (entityCategory == EntityCategory_NONE) {
			
				priorityTargetInfo.targetInfo = *it;
				priorityTargetInfo.priority = (*it)->target->GetPos().DistanceManhattan(pos);
				queue.push(priorityTargetInfo);		
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

	if (result != nullptr)

		result = *find(targets.begin(), targets.end(), result);

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

	//if (this->unitCommand == UnitCommand_NoCommand) {

		this->unitCommand = unitCommand;
		ret = true;
	//}

	return ret;
}

UnitCommand DynamicEntity::GetUnitCommand() const
{
	return unitCommand;
}

// UnitInfo struct ---------------------------------------------------------------------------------

UnitInfo::UnitInfo() {}

UnitInfo::UnitInfo(const UnitInfo& u) :
	priority(u.priority), sightRadius(u.sightRadius), attackRadius(u.attackRadius), heavyDamage(u.heavyDamage), lightDamage(u.lightDamage),
	airDamage(u.airDamage), towerDamage(u.towerDamage), maxSpeed(u.maxSpeed), currSpeed(u.currSpeed), currLife(u.currLife), maxLife(u.maxLife),
	size(u.size), offsetSize(u.offsetSize), isWanderSpawnTile(u.isWanderSpawnTile) {}

UnitInfo::~UnitInfo() {}

// TargetInfo struct ---------------------------------------------------------------------------------

TargetInfo::TargetInfo() {}

TargetInfo::TargetInfo(const TargetInfo& t) :
	isSightSatisfied(t.isSightSatisfied), isAttackSatisfied(t.isAttackSatisfied), target(t.target),
	isInGoals(t.isInGoals), isRemoveNeeded(t.isRemoveNeeded), attackingTile(t.attackingTile) {}

TargetInfo::~TargetInfo()
{
	isSightSatisfied = false;
	isAttackSatisfied = false;

	isInGoals = 0;
	isRemoveNeeded = false;

	target = nullptr;

	attackingTile = { -1,-1 };
}

bool TargetInfo::IsTargetDead() const 
{
	// The target doesn't exist (just in case)
	if (target == nullptr)
		return true;

	// -----

	if (target->GetCurrLife() <= 0 || target->isRemove)
		return true;

	return false;
}

bool TargetInfo::IsTargetValid() const 
{
	// The target doesn't exist (just in case)
	if (target == nullptr)
		return false;

	// -----

	if (!target->GetIsValid())
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

iPoint DynamicEntity::GetLastTile()
{
	return lastColliderUpdateTile;
}

// Fow
void DynamicEntity::SetLastSeenTile(iPoint lastSeenTile) 
{
	this->lastSeenTile = lastSeenTile;
}

iPoint DynamicEntity::GetLastSeenTile() const
{
	return lastSeenTile;
}

// Group selection
void DynamicEntity::BlitSavedGroupSelection()
{
	isBlitSelectedGroupSelection = false;
	isBlitSavedGroupSelection = true;
	alphaSavedGroupSelection = 255;
}

void DynamicEntity::BlitSelectedGroupSelection()
{
	isBlitSavedGroupSelection = false;
	isBlitSelectedGroupSelection = true;
	alphaSelectedGroupSelection = 255;
}