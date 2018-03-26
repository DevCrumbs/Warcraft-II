#include "j1App.h"
#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"
#include "j1Render.h"
#include "StaticEntity.h"
#include "j1Particles.h"

StaticEntity::StaticEntity(fPoint pos, iPoint size, int maxLife, j1Module* listener) :Entity(pos, size, maxLife, listener) {
	constructionTime = 10;
}

StaticEntity::~StaticEntity() {}

void StaticEntity::Draw(SDL_Texture* sprites)
{
	App->render->Blit(sprites, pos.x, pos.y, texArea);
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
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_PRESSED && !(StaticEntity*)this->isBuilt) {
			EntityEvent = EntitiesEvent_CREATED;
			listener->OnStaticEntitiesEvent((StaticEntity*)this, EntityEvent);
			EntityEvent = EntitiesEvent_HOVER;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_PRESSED) {

			EntityEvent = EntitiesEvent_LEFT_CLICK;
			listener->OnStaticEntitiesEvent((StaticEntity*)this, EntityEvent);
			EntityEvent = EntitiesEvent_HOVER;
			break;

		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_PRESSED) {

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


bool StaticEntity::CheckBuildingState() {
	bool ret = true;
	BuildingState bs = buildingState;

	if (this->GetCurrLife() <= 0)
		buildingState = BuildingState_Destroyed;
	else if (this->GetCurrLife() <= this->GetMaxLife() / 4) {// less than 1/4 HP
			buildingState = BuildingState_HardFire;
	}
	else if (this->GetCurrLife() <= 3 * this->GetMaxLife() / 4)// less than 3/4 HP
		buildingState = BuildingState_LowFire;
	else {
		buildingState = BuildingState_Normal;
	}
		

	if(bs != buildingState)
		switch (buildingState)
		{
		case BuildingState_Normal:
			fire->isDeleted = true;
			break;
		case BuildingState_LowFire:
			fire = App->particles->AddParticle(App->particles->lowFire, this->GetPosition().x + this->GetSize().x / 3, this->GetPosition().y + this->GetSize().y / 3);
			break;

		case BuildingState_HardFire:
			fire->isDeleted = true;
			fire = App->particles->AddParticle(App->particles->hardFire, this->GetPosition().x + this->GetSize().x / 5, this->GetPosition().y + this->GetSize().y / 5);

			break;
		case BuildingState_Destroyed:
			fire->isDeleted = true;
			ret = false;
			break;
		default:
			break;
		}
	return ret;
}

uint StaticEntity::GetConstructionTimer() const
{
	return constructionTimer.ReadSec();
}

uint StaticEntity::GetConstructionTime() const
{
	return constructionTime;
}

bool StaticEntity::GetIsFinishedBuilt() const
{
	return isBuilt;
}