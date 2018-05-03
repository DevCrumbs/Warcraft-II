#include "Turalyon.h"
#include "j1Printer.h"
#include "j1EntityFactory.h"

Turalyon::Turalyon(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const TuralyonInfo& turalyonInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), turalyonInfo(turalyonInfo)
{
	animation = &this->turalyonInfo.idle;

	TuralyonInfo info = (TuralyonInfo&)App->entities->GetUnitInfo(EntityType_TURALYON);
	this->unitInfo = this->turalyonInfo.unitInfo;
	offsetSize = this->unitInfo.offsetSize;
}

void Turalyon::Move(float dt)
{
	HandleInput(entityEvent);
}

void Turalyon::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr) {

		fPoint offset = { 0.0f,0.0f };
		offset = { animation->GetCurrentFrame().w / 3.8f, animation->GetCurrentFrame().h / 3.5f };

		App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_Entities);
	}

	if (isSelected)
		DebugDrawSelected();
}

void Turalyon::DebugDrawSelected()
{
	const SDL_Rect entitySize = { pos.x + offsetSize.x, pos.y + offsetSize.y, size.x, size.y };
	App->printer->PrintQuad(entitySize, color);
}

// Animations
void Turalyon::LoadAnimationsSpeed()
{

}
void Turalyon::UpdateAnimations(float dt)
{

}

// Prisoner rescue
bool Turalyon::IsUnitRescuingPrisoner() const
{
	return isUnitRescuingPrisoner;
}

void Turalyon::SetUnitRescuePrisoner(bool isUnitRescuingPrisoner)
{
	this->isUnitRescuingPrisoner = isUnitRescuingPrisoner;
}


bool Turalyon::IsRescued() const
{
	return isRescued;
}

void Turalyon::SetRescued(bool isRescued)
{
	this->isRescued = isRescued;
}