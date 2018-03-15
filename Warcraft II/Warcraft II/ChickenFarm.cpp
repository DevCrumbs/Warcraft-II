#include "ChickenFarm.h"

#include "j1Input.h"
#include "j1Window.h"

ChickenFarm::ChickenFarm(fPoint pos, iPoint size, int life, const ChickenFarmInfo& chickenFarmInfo, j1Module* listener) :StaticEntity(pos, size, life, listener), chickenFarmInfo(chickenFarmInfo)
{
	texArea = &chickenFarmInfo.completeTexArea;
}

void ChickenFarm::Move(float dt)
{
	if (listener != nullptr)
		HandleInput();
}

// Animations
void ChickenFarm::LoadAnimationsSpeed()
{

}
void ChickenFarm::UpdateAnimations(float dt)
{

}

void ChickenFarm::HandleInput()
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

			EntityEvent = EntitiesEvent_None;
			break;
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

		listener->OnStaticEntitiesEvent((StaticEntity*)this, EntityEvent);
		EntityEvent = EntitiesEvent_None;
		break;
	}
}


bool ChickenFarm::MouseHover() const
{
	int x, y;
	App->input->GetMousePosition(x, y);
	uint scale = App->win->GetScale();

	iPoint screen_pos;
	screen_pos.x = pos.x;
	screen_pos.y = pos.y;

	return x > screen_pos.x / scale && x < screen_pos.x / scale + size.x && y > screen_pos.y / scale && y < screen_pos.y / scale + size.y;
}