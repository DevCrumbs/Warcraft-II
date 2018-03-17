#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"
#include "j1Render.h"
#include "DynamicEntity.h"

DynamicEntity::DynamicEntity(fPoint pos, iPoint size, int maxLife, float speed, j1Module* listener) : Entity(pos, size, maxLife, listener), speed(speed) {}

void DynamicEntity::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr)
		App->render->Blit(sprites, pos.x, pos.y, &(animation->GetCurrentFrame()));
}