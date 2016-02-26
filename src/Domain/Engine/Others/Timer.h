#ifndef TIMER_H
#define TIMER_H

#include "Bang.h"

#include <functional>
#include <thread>
#include <chrono>

class Timer
{
private:
    std::function<void()> func;
    float secDelay;
    int repeatCount;
    bool waitOneDelayTime;

    mutable bool stop;

    std::thread timerThread;

    void TimerLoop() const;

public:
    Timer(std::function<void()> func, float secDelay, int repeatCount = -1, bool waitOneDelayTime = false);

    void Start() const;
    void Stop() const;
};

#endif // TIMER_H