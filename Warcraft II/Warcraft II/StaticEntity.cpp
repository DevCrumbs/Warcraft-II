#include "j1App.h"
#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"
#include "j1Render.h"
#include "StaticEntity.h"
#include "j1Particles.h"

StaticEntity::StaticEntity(fPoint pos, iPoint size, int maxLife, j1Module* listener) :Entity(pos, size, maxLife, listener) {
	constructionTime = 15;
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

	case EntitiesEvent_None:
		if (MouseHover()) {
			EntityEvent = EntitiesEvent_Hover;
			listener->OnStaticEntitiesEvent((StaticEntity*)this, EntityEvent);
			break;
		}
		break;
	case EntitiesEvent_Hover:

		if (!MouseHover()) {

			EntityEvent = EntitiesEvent_Leave;
			break;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_PRESSED && !(StaticEntity*)this->isBuilt) {
			EntityEvent = EntitiesEvent_Created;
			listener->OnStaticEntitiesEvent((StaticEntity*)this, EntityEvent);
			EntityEvent = EntitiesEvent_Hover;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_PRESSED) {

			EntityEvent = EntitiesEvent_LeftClick;
			listener->OnStaticEntitiesEvent((StaticEntity*)this, EntityEvent);
			EntityEvent = EntitiesEvent_Hover;
			break;

		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_PRESSED) {

			EntityEvent = EntitiesEvent_RightClick;
			listener->OnStaticEntitiesEvent((StaticEntity*)this, EntityEvent);
			EntityEvent = EntitiesEvent_Hover;
			break;

		}
		break;
	case EntitiesEvent_Created:

		if (MouseHover()) {
			EntityEvent = EntitiesEvent_Hover;
			listener->OnStaticEntitiesEvent((StaticEntity*)this, EntityEvent);
			break;
		}
		break;

	case EntitiesEvent_Leave:
		listener->OnStaticEntitiesEvent((StaticEntity*)this, EntityEvent);
		EntityEvent = EntitiesEvent_None;

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


void StaticEntity::CheckBuildingState() {

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
			break;
		default:
			break;
		}
}