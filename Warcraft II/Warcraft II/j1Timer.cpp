// ----------------------------------------------------
// j1Timer.cpp
// Fast timer with milisecons precision
// ----------------------------------------------------

#include "SDL\include\SDL_timer.h"

#include "j1Timer.h"
#include "p2Log.h"

// ---------------------------------------------
j1Timer::j1Timer()
{
	Start();
}

// ---------------------------------------------
void j1Timer::Start()
{
	started = true;
	startedAt = SDL_GetTicks();
	LOG("Stareted at %i", startedAt);
}

// ---------------------------------------------
uint32 j1Timer::Read() const
{
	return SDL_GetTicks() - startedAt;
}

// ---------------------------------------------
float j1Timer::ReadSec() const
{
	return float(SDL_GetTicks() - startedAt) / 1000.0f;
}

void j1Timer::Stop()
{
	startedAt = 0;
	started = false;
}

bool j1Timer::IsStarted()
{
	return started;
}
