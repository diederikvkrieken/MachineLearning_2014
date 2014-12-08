#include "timer.h"

Timer::Timer()
{
  new_timer = true;
  running = false;
  pause_time = 0;
}

Uint32 Timer::getTime()
{
  return SDL_GetTicks() - start_time - pause_time;
}

void Timer::start()
{
  if(running)
  { return; }

  if(new_timer)
  {
    start_time = SDL_GetTicks();
    running = true;
    new_timer = false;
  }
  else
  {
    pause_time += SDL_GetTicks() - stop_time;
    running = true;
  }
}

void Timer::stop()
{
  stop_time = SDL_GetTicks();
  running = false;
}

void Timer::reset()
{
  start_time = SDL_GetTicks();
  pause_time = 0;
  if(!running)
  { new_timer = true; }
}

























