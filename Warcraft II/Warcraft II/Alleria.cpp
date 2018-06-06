#include "Alleria.h"
#include "j1Printer.h"
#include "j1EntityFactory.h"
#include "Goal.h"

Alleria::Alleria(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const AlleriaInfo& alleriaInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener, false), alleriaInfo(alleriaInfo)
{
	*(ENTITY_CATEGORY*)&entityType = EntityCategory_DYNAMIC_ENTITY;
	*(ENTITY_TYPE*)&dynamicEntityType = EntityType_ALLERIA;
	*(EntitySide*)&entitySide = EntitySide_NoSide;

	AlleriaInfo info = (AlleriaInfo&)App->entities->GetUnitInfo(EntityType_ALLERIA);
	this->unitInfo = this->alleriaInfo.unitInfo;
	this->alleriaInfo.idle = info.idle;
	this->alleriaInfo.rescue = info.rescue;

	offsetSize = this->unitInfo.offsetSize;

	LoadAnimationsSpeed();

	animation = &this->alleriaInfo.idle;
	isStill = true;
}

void Alleria::Move(float dt)
{
	HandleInput(entityEvent);
	UpdateAnimations(dt);

	if (isRescued) {
	
		if (animation->Finished())
			isRemove = true;
	}
}

void Alleria::Draw(SDL_Texture* sprites) 
{
	if (animation != nullptr) {

		fPoint offset = { 0.0f,0.0f };
		if (App->fow->IsOnSight(pos))
		{
			if (animation == &alleriaInfo.rescue) {

				offset = { animation->GetCurrentFrame().w / 1.5f, animation->GetCurrentFrame().h / 3.0f };
				App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_Entities);
			}
			else {

				offset = { animation->GetCurrentFrame().w / 8.0f, animation->GetCurrentFrame().h / 3.0f };
				App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_Entities);
			}
		}
	}
	if (isSelected)
		DebugDrawSelected();
}

void Alleria::DebugDrawSelected() 
{
	const SDL_Rect entitySize = { pos.x + offsetSize.x, pos.y + offsetSize.y, size.x, size.y };
	App->printer->PrintQuad(entitySize, color);
}

// Animations
void Alleria::LoadAnimationsSpeed()
{
	idleSpeed = alleriaInfo.idle.speed;
	rescueSpeed = alleriaInfo.rescue.speed;
}

void Alleria::UpdateAnimations(float dt)
{
	alleriaInfo.idle.speed = idleSpeed * dt;
	alleriaInfo.rescue.speed = rescueSpeed * dt;
}

// Prisoner rescue
bool Alleria::IsUnitRescuingPrisoner() const 
{
	return isUnitRescuingPrisoner;
}

void Alleria::SetUnitRescuePrisoner(bool isUnitRescuingPrisoner) 
{
	this->isUnitRescuingPrisoner = isUnitRescuingPrisoner;
}

bool Alleria::IsRescued() const 
{
	return isRescued;
}

void Alleria::SetRescued(bool isRescued) 
{
	this->isRescued = isRescued;

	if (this->isRescued)

		animation = &this->alleriaInfo.rescue;
}