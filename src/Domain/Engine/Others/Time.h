#ifndef TIME_H
#define TIME_H

#include "Bang.h"

#include <chrono>

class Time
{
friend class Canvas;

private:
    static float deltaTime;

    Time() {}

public:
    /**
     * @brief Returns the number of seconds that have passed between
     *        the last frame and the current one (the delta time)
     * @return The delta time in seconds.
     */
    static float GetDeltaTime();

    /**
     * @brief GetNow
     * @return Returns the epoch time.
     */
    static unsigned long long GetNow();
};

#endif // TIME_H
