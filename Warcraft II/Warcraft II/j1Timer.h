#ifndef __j1TIMER_H__
#define __j1TIMER_H__

#include "Defs.h"

class j1Timer
{
public:

	// Constructor
	j1Timer();

	void Start();
	uint32 Read() const;
	float ReadSec() const;

	void Stop();
	void Resume(int time);
	bool IsStarted();

private:
	uint32	startedAt = 0;

	bool started = false;
};

#endif //__j1TIMER_H__