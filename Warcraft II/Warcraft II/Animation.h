#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "SDL/include/SDL_rect.h"
#define MAX_FRAMES 45

class Animation
{
private:
	int loops = 0;
	float currentFrame = 0.0f;
	int lastFrame = 0;

	SDL_Rect frames[MAX_FRAMES];

public:
	float speed = 0.0f;
	bool loop = true;

public:

	void PushBack(const SDL_Rect& rect)
	{
		frames[lastFrame++] = rect;
	}

	SDL_Rect& GetCurrentFrame()
	{
		currentFrame += speed;
		if (currentFrame >= lastFrame)
		{
			loops++;
			currentFrame = (loop) ? 0.0f : lastFrame - 1;
		}

		return frames[(int)currentFrame];
	}

	bool Finished() const
	{
		return loops > 0;
	}

	void Reset()
	{
		loops = 0;
		currentFrame = 0;
	}

	void Start()
	{
		speed = 0.08f;
	}

	void Stop()
	{
		speed = 0.0f;
	}
};

#endif //__ANIMATION_H__