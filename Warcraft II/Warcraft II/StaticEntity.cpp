#include "j1App.h"
#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"
#include "j1Render.h"
#include "StaticEntity.h"
#include "j1Particles.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1EntityFactory.h"
#include "j1Pathfinding.h"
#include "j1Printer.h"
#include "j1Player.h"
#include "j1Gui.h"

StaticEntity::StaticEntity(fPoint pos, iPoint size, int currLife, uint maxLife, j1Module* listener) :Entity(pos, size, currLife, maxLife, listener) 
{
	constructionTime = 10.0;
}

StaticEntity::~StaticEntity()
{
	// Remove Colliders
	if (sightRadiusCollider != nullptr)
		sightRadiusCollider->isRemove = true;
	sightRadiusCollider = nullptr;

	iPoint buildingTile;

	if (App->scene->active) {

		switch (buildingSize) {

		case StaticEntitySize_Small:
			buildingTile = App->map->WorldToMap(pos.x, pos.y);
			App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 1u;
			App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 1u;
			break;

		case StaticEntitySize_Medium:
			buildingTile = App->map->WorldToMap(pos.x, pos.y);
			App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 1u;
			App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y) + (buildingTile.x + 2)] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 2) + buildingTile.x] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 1)] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 2)] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 2)] = 1u;
			break;

		case StaticEntitySize_Big:
			buildingTile = App->map->WorldToMap(pos.x, pos.y);
			App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 1u;
			App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y) + (buildingTile.x + 2)] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 2) + buildingTile.x] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 1)] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 2)] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 2)] = 1u;

			App->scene->data[App->scene->w * (buildingTile.y) + (buildingTile.x + 3)] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 3)] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 3)] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 3) + (buildingTile.x + 3)] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 3) + buildingTile.x] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 3) + (buildingTile.x + 1)] = 1u;
			App->scene->data[App->scene->w * (buildingTile.y + 3) + (buildingTile.x + 2)] = 1u;
			break;

		case StaticEntitySize_None:
		default:
			break;
		}
	}
}

void StaticEntity::Draw(SDL_Texture* sprites)
{
	//App->render->Blit(sprites, pos.x, pos.y, texArea);
	App->printer->PrintSprite({ (int)pos.x, (int)pos.y }, sprites, (SDL_Rect)*texArea, Layers_Entities);

	if (isSelected)
		DebugDrawSelected();
}

void StaticEntity::DebugDrawSelected() 
{
	const SDL_Rect entitySize = { pos.x, pos.y, size.x, size.y };
	App->printer->PrintQuad(entitySize, color);
}

void StaticEntity::HandleInput(EntitiesEvent &EntityEvent)
{
	iPoint mouse_pos;
	App->input->GetMousePosition(mouse_pos.x, mouse_pos.y);

	switch (EntityEvent) {

	case EntitiesEvent_NONE:
		if (MouseHover()) {
			EntityEvent = EntitiesEvent_HOVER;
			listener->OnStaticEntitiesEvent((StaticEntity*)this, EntityEvent);
			break;
		}
		break;
	case EntitiesEvent_HOVER:

		if (!MouseHover()) {

			EntityEvent = EntitiesEvent_LEAVE;
			break;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_PRESSED && !(StaticEntity*)this->isBuilt && !App->gui->IsMouseOnUI()) {
			EntityEvent = EntitiesEvent_CREATED;
			listener->OnStaticEntitiesEvent((StaticEntity*)this, EntityEvent);
			EntityEvent = EntitiesEvent_HOVER;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_PRESSED && !App->gui->IsMouseOnUI()) {

			EntityEvent = EntitiesEvent_LEFT_CLICK;
			listener->OnStaticEntitiesEvent((StaticEntity*)this, EntityEvent);
			EntityEvent = EntitiesEvent_HOVER;
			break;

		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_PRESSED && !App->gui->IsMouseOnUI()) {

			EntityEvent = EntitiesEvent_RIGHT_CLICK;
			listener->OnStaticEntitiesEvent((StaticEntity*)this, EntityEvent);
			EntityEvent = EntitiesEvent_HOVER;
			break;

		}
		break;
	case EntitiesEvent_CREATED:

		if (MouseHover()) {
			EntityEvent = EntitiesEvent_HOVER;
			listener->OnStaticEntitiesEvent((StaticEntity*)this, EntityEvent);
			break;
		}
		break;

	case EntitiesEvent_LEAVE:
		listener->OnStaticEntitiesEvent((StaticEntity*)this, EntityEvent);
		EntityEvent = EntitiesEvent_NONE;

		break;
	}
}

bool StaticEntity::MouseHover() const
{
	int x, y;
	App->input->GetMousePosition(x, y);
	uint scale = App->win->GetScale();

	iPoint screen_pos;
	screen_pos.x = pos.x + App->render->camera.x;
	screen_pos.y = pos.y + App->render->camera.y;

	return x > screen_pos.x / scale && x < screen_pos.x / scale + size.x && y > screen_pos.y / scale && y < screen_pos.y / scale + size.y;
}

bool StaticEntity::CheckBuildingState()
{
	bool ret = true;

	BuildingState bs = buildingState;

	if (this->GetCurrLife() <= 0) {

		buildingState = BuildingState_Destroyed;
		isValid = false;
	}
	else if (this->GetCurrLife() <= this->GetMaxLife() / 4) {// less than 1/4 HP
		buildingState = BuildingState_HardFire;
	}
	else if (this->GetCurrLife() <= 3 * this->GetMaxLife() / 4)// less than 3/4 HP
		buildingState = BuildingState_LowFire;
	else {
		buildingState = BuildingState_Normal;
	}

	if (bs != buildingState) {

		switch (buildingState)
		{
		case BuildingState_Normal:
			if(fire != nullptr)
				fire->isRemove = true;
			break;

		case BuildingState_LowFire:

			if (fire != nullptr)
				fire->isRemove = true;
			fire = App->particles->AddParticle(App->particles->lowFire, { (int)this->GetPos().x + this->GetSize().x / 3, (int)this->GetPos().y + this->GetSize().y / 3 });
			break;

		case BuildingState_HardFire:

			if (fire != nullptr)
				fire->isRemove = true;
			fire = App->particles->AddParticle(App->particles->hardFire, { (int)this->GetPos().x + this->GetSize().x / 5, (int)this->GetPos().y + this->GetSize().y / 5 });
			break;

		case BuildingState_Destroyed:

			if (fire != nullptr)
				fire->isRemove = true;
			isRemove = true;

			if (entitySide == EntitySide_Enemy) {

				// Give gold to the player
				if (entityType == EntityType_WATCH_TOWER || entityType == EntityType_ENEMY_GUARD_TOWER || entityType == EntityType_ENEMY_CANNON_TOWER)
					App->player->AddGold(App->entities->DetermineBuildingGold(staticEntityType, buildingSize));
				else
					App->player->AddGold(App->entities->DetermineBuildingGold(EntityType_NONE, buildingSize));

				App->audio->PlayFx(App->audio->GetFX().goldGetSound); // Gold sound

				/// Check if the room of this enemy has been cleared
				Room* room = App->map->GetEntityRoom(this);

				if (room != nullptr) {
					if (!room->isCleared) {

						if (App->map->GetEntitiesOnRoomByCategory(*room, EntityCategory_NONE, EntitySide_Enemy).size() == 0) {

							// ROOM CLEARED!
							if (room->roomRect.w != 40 * 32) {

								// Give gold to the player
								if (room->roomRect.w == 30 * 32)
									App->player->AddGold(300);
								else if (room->roomRect.w == 50 * 32)
									App->player->AddGold(800);

								room->isCleared = true;
								App->player->roomsCleared++;

								if (App->scene->adviceMessage != AdviceMessage_ROOM_CLEAR) {

									App->scene->adviceMessageTimer.Start();
									App->scene->adviceMessage = AdviceMessage_ROOM_CLEAR;
									App->scene->ShowAdviceMessage(App->scene->adviceMessage);
								}

								App->scene->alpha = 200;
								App->scene->isRoomCleared = true;
								App->scene->roomCleared = room->roomRect;

								/// TODO Valdivia: sonido sala limpiada
								App->audio->PlayFx(App->audio->GetFX().roomClear, 0);
							}
						}
					}
				}
			}

			ret = false;
			break;

		default:
			break;
		}
	}

	return ret;
}

float StaticEntity::GetConstructionTimer() const
{
	return constructionTimer;
}

float StaticEntity::GetConstructionTime() const
{
	return constructionTime;
}

bool StaticEntity::GetIsFinishedBuilt() const
{
	return isBuilt;
}

BuildingState StaticEntity::GetBuildingState() const 
{
	return buildingState;
}

ColliderGroup* StaticEntity::CreateRhombusCollider(ColliderType colliderType, uint radius, DistanceHeuristic distanceHeuristic)
{
	vector<Collider*> colliders;

	// Perform a BFS
	queue<iPoint> queue;
	list<iPoint> visited;

	iPoint curr = App->map->WorldToMap(pos.x, pos.y);
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
			if (App->pathfinding->IsWalkable(neighbors[i]) && CalculateDistance(neighbors[i], App->map->WorldToMap(pos.x, pos.y), distanceHeuristic) < radius) {

				if (find(visited.begin(), visited.end(), neighbors[i]) == visited.end()) {

					queue.push(neighbors[i]);
					visited.push_back(neighbors[i]);

					iPoint collPos = App->map->MapToWorld(neighbors[i].x, neighbors[i].y);
					SDL_Rect rect = { collPos.x, collPos.y, App->map->data.tileWidth, App->map->data.tileHeight };
					bool isRectInVector = false;

					//Up left
					for (uint i = 0; i < colliders.size(); ++i) {
						if (rect == colliders[i]->colliderRect) {
							isRectInVector = true;
							break;
						}
					}
					if (!isRectInVector) {
						Collider* coll = App->collision->CreateCollider(rect);
						colliders.push_back(coll);
					}
					isRectInVector = false;

					//Up right
					if (App->pathfinding->IsWalkable({ neighbors[i].x + 1, neighbors[i].y }) && CalculateDistance({ neighbors[i].x + 1, neighbors[i].y }, App->map->WorldToMap(pos.x + 32, pos.y), distanceHeuristic) < radius) {
						rect = { collPos.x + 32, collPos.y, App->map->data.tileWidth, App->map->data.tileHeight };
						for (uint i = 0; i < colliders.size(); ++i) {
							if (rect == colliders[i]->colliderRect) {
								isRectInVector = true;
								break;
							}
						}
						if (!isRectInVector) {
							Collider* coll = App->collision->CreateCollider(rect);
							colliders.push_back(coll);
						}
						isRectInVector = false;
					}
					//Down left
					if (App->pathfinding->IsWalkable({ neighbors[i].x, neighbors[i].y + 1 }) && CalculateDistance({ neighbors[i].x + 1, neighbors[i].y }, App->map->WorldToMap(pos.x + 32, pos.y), distanceHeuristic) < radius) {
						rect = { collPos.x, collPos.y + 32, App->map->data.tileWidth, App->map->data.tileHeight };
						for (uint i = 0; i < colliders.size(); ++i) {
							if (rect == colliders[i]->colliderRect) {
								isRectInVector = true;
								break;
							}
						}

						if (!isRectInVector){
							Collider* coll = App->collision->CreateCollider(rect);
							colliders.push_back(coll);
						}
						isRectInVector = false;
					}

					//Down right
					if (App->pathfinding->IsWalkable({ neighbors[i].x + 1, neighbors[i].y + 1 }) && CalculateDistance({ neighbors[i].x + 1, neighbors[i].y }, App->map->WorldToMap(pos.x + 32, pos.y), distanceHeuristic) < radius) {
						rect = { collPos.x + 32, collPos.y + 32, App->map->data.tileWidth, App->map->data.tileHeight };
						for (uint i = 0; i < colliders.size(); ++i) {
							if (rect == colliders[i]->colliderRect) {
								isRectInVector = true;
								break;
							}
						}
						if (!isRectInVector) {
							Collider* coll = App->collision->CreateCollider(rect);
							colliders.push_back(coll);
						}
					}
					
				}
			}
		}
	}

	// 2. Create/Update the offset collider
	ColliderGroup* colliderGroup = App->collision->CreateAndAddColliderGroup(colliders, colliderType, App->entities, this);

	if (colliderGroup != nullptr)

		colliderGroup->CreateOffsetCollider();

	return colliderGroup;
}

ColliderGroup* StaticEntity::GetSightRadiusCollider() const
{
	return sightRadiusCollider;
}

// Reconstruction
float StaticEntity::GetSecondsReconstruction(StaticEntitySize buildingSize) const 
{
	/// TODO Balancing (enemy buildings reconstruction)
	switch (buildingSize) {
	
	case StaticEntitySize_Small:
		return 5.0f;
		break;
	case StaticEntitySize_Medium:
		return 8.0f;
		break;
	case StaticEntitySize_Big:
		return 12.0f;
		break;
	default:
		return 5.0f;
		break;
	}

	return 5.0f;
}

// Respawn
float StaticEntity::GetRandomSecondsRespawn() const 
{
	/// TODO Balancing (respawn enemies)
	int minValue = 120.0f;
	int maxValue = 150.0f;

	/// rand() % (max - min + 1) + min
	int randomValue = rand() % (maxValue - minValue + 1) + minValue;

	return (float)randomValue;
}

uint StaticEntity::GetMaxEnemiesPerRoom(ROOM_TYPE roomType) const 
{
	/// TODO Balancing (max enemies per room)
	switch (roomType) {
	
	case roomType_LITTLE:
		return 10;
		break;
	case roomType_LARGE:
		return 25;
		break;
	default:
		return 15;
		break;
	}

	return 15;
}