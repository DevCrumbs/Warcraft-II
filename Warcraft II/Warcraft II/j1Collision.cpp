#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"

#include "j1Collision.h"
#include "j1Input.h"
#include "j1Render.h"

j1Collision::j1Collision()
{
	name.assign("collision");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	/*
	matrix[COLLIDER_PLAYER][COLLIDER_ARROW] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_CATPEASANT] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_CATPEASANT_SHOT] = true;
	*/
}

// Destructor
j1Collision::~j1Collision()
{}

bool j1Collision::PreUpdate()
{
	// Remove all colliders scheduled for deletion
<<<<<<< HEAD
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->toDelete)
		{
			delete colliders[i];
			colliders[i] = nullptr;
=======
	list<ColliderGroup*>::iterator it = colliderGroups.begin();

	while (it != colliderGroups.end()) {

		if ((*it)->isRemove) {

			EraseColliderGroup(*it);

			it = colliderGroups.begin();
			continue;
>>>>>>> Develompent
		}
	}

	return true;
}

// Called before render is available
bool j1Collision::Update(float dt)
{
<<<<<<< HEAD
	Collider* c1;
	Collider* c2;
=======
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
>>>>>>> Develompent

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

<<<<<<< HEAD
			if (c1->CheckCollision(c2->rect))
			{
				if (matrix[c1->type][c2->type] && c1->callback)
					c1->callback->OnCollision(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback)
					c2->callback->OnCollision(c2, c1);
=======
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
>>>>>>> Develompent
			}
			J++;
		}
	}

<<<<<<< HEAD
	DebugDraw();

	return true;
=======
	HandleTriggers();

	return ret;
>>>>>>> Develompent
}

void j1Collision::DebugDraw()
{
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		debug = !debug;

	if (debug == false)
		return;

	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case COLLIDER_NONE: // white
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER_PLAYER: // green
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break;
		case COLLIDER_ARROW: // blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
		case COLLIDER_IMP_BOMB_EXPLOSION: // red
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;
		}
	}
}

// Called before quitting
bool j1Collision::CleanUp()
{
	LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

Collider* j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
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
	}

	return ret;
}
>>>>>>> Develompent

	return ret;
}

bool j1Collision::EraseCollider(Collider* collider)
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == collider)
		{
			delete colliders[i];
			colliders[i] = nullptr;
			return true;
		}
	}

	return false;
}

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
>>>>>>> Develompent
}
