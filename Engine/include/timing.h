#ifndef Engine_timing_h_
#define Engine_timing_h_

#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>

// Time class
class Time {
    public:
    // Data Members
    Uint64 nanoseconds;
    float seconds;

    // Constructors
    Time();
    Time(Uint64 ns);
    Time(float s);

    // Constants
    static const Time second;
    static const Time millisecond;
    static const Time microsecond;
    static const Time nanosecond;
};

// Functions
Time getTime();
float getTimeInSeconds();
void sleep(Time);

// Operators
Time operator+(const Time &a, const Time &b);
Time& operator+=(Time &a, const Time &b);

Time operator-(const Time &a, const Time &b);
Time& operator-=(Time &a, const Time &b);

Time operator*(const Time &a, const float &b);
Time operator/(const Time &a, const float &b);


bool operator>(const Time &a, const Time &b);
bool operator>=(const Time &a, const Time &b);

bool operator<(const Time &a, const Time &b);
bool operator<=(const Time &a, const Time &b);

bool operator==(const Time &a, const Time &b);
bool operator!=(const Time &a, const Time &b);

#endif