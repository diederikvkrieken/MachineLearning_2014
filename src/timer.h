#ifndef TIMER_H
#define TIMER_H

#include <SDL.h>

class Timer
{
  public:
    Timer();
    Uint32 getTime();
    void start();
    void stop();
    void reset();

    bool isStarted() { return !new_timer; }
  private:
    bool new_timer, running;
    Uint32 start_time, stop_time, pause_time;
};

#endif // TIMER_H


