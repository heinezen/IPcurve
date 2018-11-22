#ifndef TIMER_H
#define TIMER_H

#include <SDL/SDL.h>

typedef void (*CTimerCallback)(void*);

class CTimer {
	public:
		CTimer(int del, CTimerCallback cb, void* dat = 0, bool sshot = 0, bool suspended = 0);
		~CTimer();
		bool shot();
		void start();
		void stop();
	private:
		CTimerCallback callback;
		void* data;
		int	delay;
		bool single_shot;
		SDL_TimerID timer_id;
};

#endif
