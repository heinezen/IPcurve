#include "timer.h"

/*typedef (void* callback)(void*) CCallback;

class  CTimer {
	public:
		CTimer(int del, CCallback cb, bool sshot = 0);
		~CTimer();
	private:
		CCallback callback;
		int	delay;
		bool singe_shot;
};*/

Uint32 TimerCB(Uint32 interval, void* obj)
{
	CTimer* timer = (CTimer*)obj;
	if(timer->shot())
		return interval;
	return 0;
}

CTimer::CTimer(int del, CTimerCallback cb, void* dat, bool sshot, bool suspended)
{
	delay = del;
	callback = cb;
	single_shot = sshot;
	data = dat;
	if(!suspended)
		timer_id = SDL_AddTimer(delay,TimerCB,this);
}

CTimer::~CTimer()
{
	stop();
}

bool CTimer::shot()
{
	if(callback)
		callback(data);
	if(!single_shot)
		timer_id = 0;
	return !single_shot;
}

void CTimer::start()
{
	if(timer_id)
		stop();
	timer_id = SDL_AddTimer(delay,TimerCB,this);
}

void CTimer::stop()
{
	if(!timer_id)
		return;
	SDL_RemoveTimer(timer_id);
}
