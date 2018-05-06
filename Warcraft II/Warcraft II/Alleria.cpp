#include "Alleria.h"
#include "j1Printer.h"
#include "j1EntityFactory.h"

Alleria::Alleria(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const AlleriaInfo& alleriaInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), alleriaInfo(alleriaInfo)
{
	animation = &this->alleriaInfo.idle;
	idleSpeed = alleriaInfo.idle.speed;

	AlleriaInfo info = (AlleriaInfo&)App->entities->GetUnitInfo(EntityType_ALLERIA);
	this->unitInfo = this->alleriaInfo.unitInfo;
	offsetSize = this->unitInfo.offsetSize;
}

void Alleria::Move(float dt)
{
	HandleInput(entityEvent);
	UpdateAnimations(dt);
}

void Alleria::Draw(SDL_Texture* sprites) 
{
	if (animation != nullptr) {

		fPoint offset = { 0.0f,0.0f };
		offset = { animation->GetCurrentFrame().w / 3.5f, animation->GetCurrentFrame().h / 3.0f };

		App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_Entities);
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

}
void Alleria::UpdateAnimations(float dt)
{
	alleriaInfo.idle.speed = idleSpeed * dt;
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
}