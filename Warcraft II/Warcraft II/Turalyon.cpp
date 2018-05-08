#include "Turalyon.h"
#include "j1Printer.h"
#include "j1EntityFactory.h"
#include "Goal.h"

Turalyon::Turalyon(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const TuralyonInfo& turalyonInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener, false), turalyonInfo(turalyonInfo)
{
	TuralyonInfo info = (TuralyonInfo&)App->entities->GetUnitInfo(EntityType_TURALYON);
	this->unitInfo = this->turalyonInfo.unitInfo;
	this->turalyonInfo.idle = info.idle;
	this->turalyonInfo.rescue = info.rescue;

	offsetSize = this->unitInfo.offsetSize;

	LoadAnimationsSpeed();

	animation = &this->turalyonInfo.idle;
	isStill = true;
}

void Turalyon::Move(float dt)
{
	HandleInput(entityEvent);
	UpdateAnimations(dt);

	if (isRescued) {

		if (animation->Finished())
			isRemove = true;
	}
}

void Turalyon::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr) {

		fPoint offset = { 0.0f,0.0f };

		if (App->fow->IsOnSight(pos))
		{
			if (animation == &turalyonInfo.rescue) {

				offset = { animation->GetCurrentFrame().w / 5.0f, animation->GetCurrentFrame().h / 3.5f };
				App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_Entities);
			}
			else {

				offset = { animation->GetCurrentFrame().w / 5.0f, animation->GetCurrentFrame().h / 3.5f };
				App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_Entities);
			}
		}
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
	idleSpeed = turalyonInfo.idle.speed;
	rescueSpeed = turalyonInfo.rescue.speed;
}

void Turalyon::UpdateAnimations(float dt)
{
	turalyonInfo.idle.speed = idleSpeed * dt;
	turalyonInfo.rescue.speed = rescueSpeed * dt;
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

	if (this->isRescued)
	
		animation = &this->turalyonInfo.rescue;
}