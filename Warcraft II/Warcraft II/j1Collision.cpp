#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"

#include "j1Collision.h"
#include "j1Input.h"
#include "j1Render.h"
#include "Entity.h"

#include "Brofiler\Brofiler.h"

j1Collision::j1Collision()
{
	name.assign("collision");

	// COLLIDERS
	/// PlayerUnit
	matrix[ColliderType_PlayerUnit][ColliderType_PlayerUnit] = false;
	matrix[ColliderType_PlayerUnit][ColliderType_EnemyUnit] = false;
	matrix[ColliderType_PlayerUnit][ColliderType_PlayerSightRadius] = false;
	matrix[ColliderType_PlayerUnit][ColliderType_EnemySightRadius] = true;
	matrix[ColliderType_PlayerUnit][ColliderType_PlayerAttackRadius] = false;
	matrix[ColliderType_PlayerUnit][ColliderType_EnemyAttackRadius] = true;

	/// EnemyUnit
	matrix[ColliderType_EnemyUnit][ColliderType_EnemyUnit] = false;
	matrix[ColliderType_EnemyUnit][ColliderType_PlayerUnit] = false;
	matrix[ColliderType_EnemyUnit][ColliderType_EnemySightRadius] = false;
	matrix[ColliderType_EnemyUnit][ColliderType_PlayerSightRadius] = true;
	matrix[ColliderType_EnemyUnit][ColliderType_EnemyAttackRadius] = false;
	matrix[ColliderType_EnemyUnit][ColliderType_PlayerAttackRadius] = true;

	/// PlayerSightRadius
	matrix[ColliderType_PlayerSightRadius][ColliderType_PlayerSightRadius] = false;
	matrix[ColliderType_PlayerSightRadius][ColliderType_EnemySightRadius] = false;
	matrix[ColliderType_PlayerSightRadius][ColliderType_PlayerUnit] = false;
	matrix[ColliderType_PlayerSightRadius][ColliderType_EnemyUnit] = true;
	matrix[ColliderType_PlayerSightRadius][ColliderType_PlayerAttackRadius] = false;
	matrix[ColliderType_PlayerSightRadius][ColliderType_EnemyAttackRadius] = false;

	/// EnemySightRadius
	matrix[ColliderType_EnemySightRadius][ColliderType_EnemySightRadius] = false;
	matrix[ColliderType_EnemySightRadius][ColliderType_PlayerSightRadius] = false;
	matrix[ColliderType_EnemySightRadius][ColliderType_EnemyUnit] = false;
	matrix[ColliderType_EnemySightRadius][ColliderType_PlayerUnit] = true;
	matrix[ColliderType_EnemySightRadius][ColliderType_EnemyAttackRadius] = false;
	matrix[ColliderType_EnemySightRadius][ColliderType_PlayerAttackRadius] = false;
<<<<<<< HEAD

	/// PlayerAttackRadius
	matrix[ColliderType_PlayerAttackRadius][ColliderType_PlayerAttackRadius] = false;
	matrix[ColliderType_PlayerAttackRadius][ColliderType_EnemyAttackRadius] = false;
	matrix[ColliderType_PlayerAttackRadius][ColliderType_PlayerSightRadius] = false;
	matrix[ColliderType_PlayerAttackRadius][ColliderType_EnemySightRadius] = false;
	matrix[ColliderType_PlayerAttackRadius][ColliderType_PlayerUnit] = false;
	matrix[ColliderType_PlayerAttackRadius][ColliderType_EnemyUnit] = true;

	/// EnemyAttackRadius
	matrix[ColliderType_EnemyAttackRadius][ColliderType_EnemyAttackRadius] = false;
	matrix[ColliderType_EnemyAttackRadius][ColliderType_PlayerAttackRadius] = false;
	matrix[ColliderType_EnemyAttackRadius][ColliderType_EnemySightRadius] = false;
	matrix[ColliderType_EnemyAttackRadius][ColliderType_PlayerSightRadius] = false;
	matrix[ColliderType_EnemyAttackRadius][ColliderType_EnemyUnit] = false;
	matrix[ColliderType_EnemyAttackRadius][ColliderType_PlayerUnit] = true;

=======

	/// PlayerAttackRadius
	matrix[ColliderType_PlayerAttackRadius][ColliderType_PlayerAttackRadius] = false;
	matrix[ColliderType_PlayerAttackRadius][ColliderType_EnemyAttackRadius] = false;
	matrix[ColliderType_PlayerAttackRadius][ColliderType_PlayerSightRadius] = false;
	matrix[ColliderType_PlayerAttackRadius][ColliderType_EnemySightRadius] = false;
	matrix[ColliderType_PlayerAttackRadius][ColliderType_PlayerUnit] = false;
	matrix[ColliderType_PlayerAttackRadius][ColliderType_EnemyUnit] = true;

	/// EnemyAttackRadius
	matrix[ColliderType_EnemyAttackRadius][ColliderType_EnemyAttackRadius] = false;
	matrix[ColliderType_EnemyAttackRadius][ColliderType_PlayerAttackRadius] = false;
	matrix[ColliderType_EnemyAttackRadius][ColliderType_EnemySightRadius] = false;
	matrix[ColliderType_EnemyAttackRadius][ColliderType_PlayerSightRadius] = false;
	matrix[ColliderType_EnemyAttackRadius][ColliderType_EnemyUnit] = false;
	matrix[ColliderType_EnemyAttackRadius][ColliderType_PlayerUnit] = true;

>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
	// DEBUG COLORS
	debugColors[ColliderType_PlayerUnit] = ColorDarkBlue;
	debugColors[ColliderType_EnemyUnit] = ColorDarkRed;
	debugColors[ColliderType_PlayerSightRadius] = ColorLightBlue;
	debugColors[ColliderType_EnemySightRadius] = ColorLightRed;
}

// Destructor
j1Collision::~j1Collision()
{}

bool j1Collision::PreUpdate()
{
	bool ret = true;

	// Remove all colliders scheduled for deletion
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->toDelete)
		{
			delete colliders[i];
			colliders[i] = nullptr;
=======
=======
>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
=======
>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
	list<ColliderGroup*>::iterator it = colliderGroups.begin();

	while (it != colliderGroups.end()) {

		if ((*it)->isRemove) {

			EraseColliderGroup(*it);

			it = colliderGroups.begin();
			continue;
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> Develompent
=======
>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
=======
>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
		}

		it++;
	}

	return ret;
}

// Called before render is available
bool j1Collision::Update(float dt)
{
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	Collider* c1;
	Collider* c2;
=======
=======
>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Orchid);

	bool ret = true;

	Collider* c1 = nullptr;
	Collider* c2 = nullptr;

	list<ColliderGroup*>::const_iterator I = colliderGroups.begin();

	while (I != colliderGroups.end()) {

		list<ColliderGroup*>::const_iterator J = I;
		J++;

		// Avoid checking collisions already checked
		while (J != colliderGroups.end()) {

			if (!matrix[(*I)->colliderType][(*J)->colliderType]
				&& !matrix[(*J)->colliderType][(*I)->colliderType]) {

				J++;
				continue;
			}

			for (uint i = 0; i < (*I)->colliders.size(); ++i) {

				c1 = (*I)->colliders[i];

				bool isCollision = false;

				for (uint j = 0; j < (*J)->colliders.size(); ++j) {

					c2 = (*J)->colliders[j];

					// Check for the collision
					if (c1->CheckCollision(c2->colliderRect)) {

						if (matrix[c1->colliderGroup->colliderType][c2->colliderGroup->colliderType] && c1->colliderGroup->callback != nullptr) {
<<<<<<< HEAD
>>>>>>> Develompent
=======
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Orchid);
>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings

	bool ret = true;

	Collider* c1 = nullptr;
	Collider* c2 = nullptr;

	list<ColliderGroup*>::const_iterator I = colliderGroups.begin();

	while (I != colliderGroups.end()) {

		list<ColliderGroup*>::const_iterator J = I;
		J++;

		// Avoid checking collisions already checked
		while (J != colliderGroups.end()) {

			if (!matrix[(*I)->colliderType][(*J)->colliderType]
				&& !matrix[(*J)->colliderType][(*I)->colliderType]) {

				J++;
				continue;
			}

			for (uint i = 0; i < (*I)->colliders.size(); ++i) {

				c1 = (*I)->colliders[i];

				bool isCollision = false;

				for (uint j = 0; j < (*J)->colliders.size(); ++j) {

					c2 = (*J)->colliders[j];

					// Check for the collision
					if (c1->CheckCollision(c2->colliderRect)) {

						if (matrix[c1->colliderGroup->colliderType][c2->colliderGroup->colliderType] && c1->colliderGroup->callback != nullptr) {

							if (c1->colliderGroup->isTrigger) {

								if (find(c1->colliderGroup->collidingGroups.begin(), c1->colliderGroup->collidingGroups.end(), c2->colliderGroup) == c1->colliderGroup->collidingGroups.end()) {

<<<<<<< HEAD
<<<<<<< HEAD
			if (c1->CheckCollision(c2->rect))
			{
				if (matrix[c1->type][c2->type] && c1->callback)
					c1->callback->OnCollision(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback)
					c2->callback->OnCollision(c2, c1);
=======
=======
									c1->colliderGroup->collidingGroups.push_back(c2->colliderGroup);
									c1->colliderGroup->lastCollidingGroups.push_back(c2->colliderGroup);

=======

							if (c1->colliderGroup->isTrigger) {

								if (find(c1->colliderGroup->collidingGroups.begin(), c1->colliderGroup->collidingGroups.end(), c2->colliderGroup) == c1->colliderGroup->collidingGroups.end()) {

									c1->colliderGroup->collidingGroups.push_back(c2->colliderGroup);
									c1->colliderGroup->lastCollidingGroups.push_back(c2->colliderGroup);

>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
									// Collision!
									c1->colliderGroup->callback->OnCollision(c1->colliderGroup, c2->colliderGroup, CollisionState_OnEnter);
									isCollision = true;
								}
								else {
									c1->colliderGroup->lastCollidingGroups.push_back(c2->colliderGroup);
									isCollision = true;
								}
							}
							else
								c1->colliderGroup->callback->OnCollision(c1->colliderGroup, c2->colliderGroup, CollisionState_OnEnter);
						}

						if (matrix[c2->colliderGroup->colliderType][c1->colliderGroup->colliderType] && c2->colliderGroup->callback != nullptr) {

							if (c2->colliderGroup->isTrigger) {

								if (find(c2->colliderGroup->collidingGroups.begin(), c2->colliderGroup->collidingGroups.end(), c1->colliderGroup) == c2->colliderGroup->collidingGroups.end()) {

									c2->colliderGroup->collidingGroups.push_back(c1->colliderGroup);
									c2->colliderGroup->lastCollidingGroups.push_back(c1->colliderGroup);

<<<<<<< HEAD
>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
=======
>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
									// Collision!
									c2->colliderGroup->callback->OnCollision(c2->colliderGroup, c1->colliderGroup, CollisionState_OnEnter);
									isCollision = true;
								}
								else {
									c2->colliderGroup->lastCollidingGroups.push_back(c1->colliderGroup);
									isCollision = true;
								}
							}
							else
								c2->colliderGroup->callback->OnCollision(c2->colliderGroup, c1->colliderGroup, CollisionState_OnEnter);
						}
					}

					if (isCollision)
						break;
				}

				if (isCollision)
					break;
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> Develompent
=======
>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
=======
>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
			}
			J++;
		}
		I++;
	}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	DebugDraw();

	return true;
=======
	HandleTriggers();

	return ret;
>>>>>>> Develompent
=======
	HandleTriggers();

	return ret;
>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
=======
	HandleTriggers();

	return ret;
>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
}

void j1Collision::HandleTriggers()
{
	list<ColliderGroup*>::const_iterator groups = colliderGroups.begin();

	while (groups != colliderGroups.end()) {

		list<ColliderGroup*>::const_iterator collisions = (*groups)->collidingGroups.begin();
<<<<<<< HEAD

		while (collisions != (*groups)->collidingGroups.end()) {

			if (find((*groups)->lastCollidingGroups.begin(), (*groups)->lastCollidingGroups.end(), *collisions) == (*groups)->lastCollidingGroups.end()) {

				// Not collision anymore...
				(*groups)->callback->OnCollision(*groups, *collisions, CollisionState_OnExit);

=======

		while (collisions != (*groups)->collidingGroups.end()) {

			if (find((*groups)->lastCollidingGroups.begin(), (*groups)->lastCollidingGroups.end(), *collisions) == (*groups)->lastCollidingGroups.end()) {

				// Not collision anymore...
				(*groups)->callback->OnCollision(*groups, *collisions, CollisionState_OnExit);

>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
				(*groups)->collidingGroups.remove(*collisions);
				collisions = (*groups)->collidingGroups.begin();
				continue;
			}
			else
				(*groups)->lastCollidingGroups.remove(*collisions);

			collisions++;
		}
		groups++;
	}
}

// Called before quitting
bool j1Collision::CleanUp()
{
	bool ret = true;
	LOG("Freeing all colliders");

	// Remove all colliderGroups
	list<ColliderGroup*>::iterator it = colliderGroups.begin();

	while (it != colliderGroups.end()) {

		delete *it;
		it++;
	}
	colliderGroups.clear();

	return ret;
}

void j1Collision::DebugDraw()
{
	Uint8 alpha = 80;
	SDL_Color color;

	list<ColliderGroup*>::const_iterator it = colliderGroups.begin();

	while (it != colliderGroups.end()) {

		color = debugColors[(*it)->colliderType];

		for (uint i = 0; i < (*it)->colliders.size(); ++i)

			App->render->DrawQuad((*it)->colliders[i]->colliderRect, color.r, color.g, color.b, alpha);

		it++;
	}
}

// ColliderGroups
ColliderGroup* j1Collision::CreateAndAddColliderGroup(vector<Collider*> colliders, ColliderType colliderType, j1Module* callback, Entity* entity)
{
	if (callback == nullptr)
		return nullptr;

	ColliderGroup* collGroup = new ColliderGroup(colliders, colliderType, callback, entity);

	for (uint i = 0; i < colliders.size(); ++i)
		colliders[i]->SetColliderGroup(collGroup);

	colliderGroups.push_back(collGroup);

	return collGroup;
}

bool j1Collision::EraseColliderGroup(ColliderGroup* colliderGroup)
{
	bool ret = false;

	if (colliderGroup != nullptr) {

		delete colliderGroup;

		if (colliderGroups.erase(remove(colliderGroups.begin(), colliderGroups.end(), colliderGroup), colliderGroups.end()) != colliderGroups.end())
			ret = true;

		colliderGroup = nullptr;
<<<<<<< HEAD
	}
<<<<<<< HEAD
=======
}

// ColliderGroups
ColliderGroup* j1Collision::CreateAndAddColliderGroup(vector<Collider*> colliders, ColliderType colliderType, j1Module* callback, Entity* entity)
{
	if (callback == nullptr)
		return nullptr;

	ColliderGroup* collGroup = new ColliderGroup(colliders, colliderType, callback, entity);

	for (uint i = 0; i < colliders.size(); ++i)
		colliders[i]->SetColliderGroup(collGroup);

	colliderGroups.push_back(collGroup);

	return collGroup;
}

bool j1Collision::EraseColliderGroup(ColliderGroup* colliderGroup)
{
	bool ret = false;

	if (colliderGroup != nullptr) {

		delete colliderGroup;

		if (colliderGroups.erase(remove(colliderGroups.begin(), colliderGroups.end(), colliderGroup), colliderGroups.end()) != colliderGroups.end())
			ret = true;

		colliderGroup = nullptr;
=======
>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
	}

	return ret;
}
>>>>>>> Develompent

	return ret;
}

// Colliders
Collider* j1Collision::CreateCollider(SDL_Rect colliderRect)
{
	Collider* coll = new Collider(colliderRect);
	return coll;
}

bool j1Collision::AddColliderToAColliderGroup(ColliderGroup* colliderGroup, Collider* collider)
{
	if (colliderGroup != nullptr && collider != nullptr)

		if (!colliderGroup->IsColliderInGroup(collider)) {

			collider->SetColliderGroup(colliderGroup);
			colliderGroup->colliders.push_back(collider);
			return true;
		}

	return false;
}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
// -----------------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return (rect.x < r.x + r.w && rect.x + rect.w > r.x && rect.y < r.y + r.h && rect.h + rect.y > r.y);
=======
bool j1Collision::EraseColliderFromAColliderGroup(ColliderGroup* colliderGroup, Collider* collider)
{
	bool ret = false;

	if (collider != nullptr) {

		delete collider;

		if (colliderGroup->colliders.erase(remove(colliderGroup->colliders.begin(), colliderGroup->colliders.end(), collider), colliderGroup->colliders.end()) != colliderGroup->colliders.end())
			ret = true;

		collider = nullptr;
	}
	return ret;
}

// Collider struct ---------------------------------------------------------------------------------

Collider::Collider(SDL_Rect colliderRect) :colliderRect(colliderRect) {}

Collider::~Collider()
{
	colliderGroup = nullptr;
}

void Collider::SetPos(int x, int y)
{
	colliderRect.x = x;
	colliderRect.y = y;
}

void Collider::SetColliderGroup(ColliderGroup* colliderGroup)
{
	this->colliderGroup = colliderGroup;
}

bool Collider::CheckCollision(const SDL_Rect& r) const
{
=======
bool j1Collision::EraseColliderFromAColliderGroup(ColliderGroup* colliderGroup, Collider* collider)
{
	bool ret = false;

	if (collider != nullptr) {

		delete collider;

		if (colliderGroup->colliders.erase(remove(colliderGroup->colliders.begin(), colliderGroup->colliders.end(), collider), colliderGroup->colliders.end()) != colliderGroup->colliders.end())
			ret = true;

		collider = nullptr;
	}
	return ret;
}

// Collider struct ---------------------------------------------------------------------------------

Collider::Collider(SDL_Rect colliderRect) :colliderRect(colliderRect) {}

Collider::~Collider()
{
	colliderGroup = nullptr;
}

void Collider::SetPos(int x, int y)
{
	colliderRect.x = x;
	colliderRect.y = y;
}

void Collider::SetColliderGroup(ColliderGroup* colliderGroup)
{
	this->colliderGroup = colliderGroup;
}

bool Collider::CheckCollision(const SDL_Rect& r) const
{
>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
	return (colliderRect.x < r.x + r.w && colliderRect.x + colliderRect.w > r.x && colliderRect.y < r.y + r.h && colliderRect.h + colliderRect.y > r.y);
}

// ColliderGroup struct ---------------------------------------------------------------------------------

ColliderGroup::ColliderGroup(vector<Collider*> colliders, ColliderType colliderType, j1Module* callback, Entity* entity) :colliders(colliders), colliderType(colliderType), callback(callback), entity(entity) {}

ColliderGroup::~ColliderGroup()
{
	callback = nullptr;
	entity = nullptr;

	// Remove all colliders
	for (uint i = 0; i < colliders.size(); ++i)

		colliders.erase(remove(colliders.begin(), colliders.end(), colliders[i]), colliders.end());

	collidingGroups.clear();
	lastCollidingGroups.clear();
<<<<<<< HEAD
>>>>>>> Develompent
=======
>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
}

=======
bool j1Collision::EraseColliderFromAColliderGroup(ColliderGroup* colliderGroup, Collider* collider)
{
	bool ret = false;

	if (collider != nullptr) {

		delete collider;

		if (colliderGroup->colliders.erase(remove(colliderGroup->colliders.begin(), colliderGroup->colliders.end(), collider), colliderGroup->colliders.end()) != colliderGroup->colliders.end())
			ret = true;

		collider = nullptr;
	}
	return ret;
}

// Collider struct ---------------------------------------------------------------------------------

Collider::Collider(SDL_Rect colliderRect) :colliderRect(colliderRect) {}

Collider::~Collider()
{
	colliderGroup = nullptr;
}

void Collider::SetPos(int x, int y)
{
	colliderRect.x = x;
	colliderRect.y = y;
}

void Collider::SetColliderGroup(ColliderGroup* colliderGroup)
{
	this->colliderGroup = colliderGroup;
}

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return (colliderRect.x < r.x + r.w && colliderRect.x + colliderRect.w > r.x && colliderRect.y < r.y + r.h && colliderRect.h + colliderRect.y > r.y);
}

// ColliderGroup struct ---------------------------------------------------------------------------------

ColliderGroup::ColliderGroup(vector<Collider*> colliders, ColliderType colliderType, j1Module* callback, Entity* entity) :colliders(colliders), colliderType(colliderType), callback(callback), entity(entity) {}

ColliderGroup::~ColliderGroup()
{
	callback = nullptr;
	entity = nullptr;

	// Remove all colliders
	for (uint i = 0; i < colliders.size(); ++i)

		colliders.erase(remove(colliders.begin(), colliders.end(), colliders[i]), colliders.end());

	collidingGroups.clear();
	lastCollidingGroups.clear();
}

>>>>>>> parent of d9438a7... Merge pull request #43 from DevCrumbs/Units-from-buildings
bool ColliderGroup::IsColliderInGroup(Collider* collider)
{
	for (uint i = 0; i < colliders.size(); ++i)

		if (colliders[i] == collider)
			return true;

	return false;
}