#ifndef __j1PERFTIMER_H__
#define __j1PERFTIMER_H__

#include "Defs.h"

class j1PerfTimer
{
public:

	// Constructor
	j1PerfTimer();

	void Start();
	double ReadMs() const;
	uint64 ReadTicks() const;

private:
	uint64	startedAt;
	static uint64 frequency;
};

#endif //__j1PERFTIMER_H__