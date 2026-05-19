#include <timing.h>

// Get Time
Time getTime() {
    return Time(SDL_GetTicksNS());
}

// Get Time (in seconds)
float getTimeInSeconds() {
    return getTime().seconds;
}

// Sleep
void sleep(Time time) {
    SDL_DelayPrecise(time.nanoseconds);
}

// Sleep (in seconds)
void sleep(float seconds) {
    Time time(seconds);
    SDL_DelayPrecise(time.nanoseconds);
}

// Some constants
const Time Time::second = Time((Uint64)1e9);
const Time Time::millisecond = Time((Uint64)1e6);
const Time Time::microsecond = Time((Uint64)1e3);
const Time Time::nanosecond = Time((Uint64)1);

// Constructors
Time::Time() {
    nanoseconds = 0;
    seconds = 0;
}

Time::Time(Uint64 ns) {
    nanoseconds = ns;
    seconds = nanoseconds / 1.e9f;
}

Time::Time(float s) {
    seconds = s;
    nanoseconds = (Uint64)(seconds * 1.e9f);
}

// Operators
Time operator+(const Time &a, const Time &b) {
    return Time(a.nanoseconds + b.nanoseconds);
}

Time& operator+=(Time &a, const Time &b) {
    a.nanoseconds += b.nanoseconds;
    a.seconds = a.nanoseconds / 1.e9f;
    return a;
}

Time operator-(const Time &a, const Time &b) {
    return Time(a.nanoseconds - b.nanoseconds);
}

Time& operator-=(Time &a, const Time &b) {
    a.nanoseconds -= b.nanoseconds;
    a.seconds = a.nanoseconds / 1.e9f;
    return a;
}

Time operator*(const Time &a, const float &b) {
    return Time((Uint64)(a.nanoseconds * b));
}
Time& operator*=(Time &a, const float &b) {
    a.nanoseconds *= b;
    a.seconds = a.nanoseconds / 1.e9f;
    return a;
}

Time operator/(const Time &a, const float &b) {
    return Time((Uint64)(a.nanoseconds / b));
}
Time& operator/=(Time &a, const float &b) {
    a.nanoseconds /= b;
    a.seconds = a.nanoseconds / 1.e9f;
    return a;
}

// Comparison Operators
bool operator>(const Time &a, const Time &b) {
    return a.nanoseconds > b.nanoseconds;
}

bool operator>=(const Time &a, const Time &b) {
    return a.nanoseconds >= b.nanoseconds;
}

bool operator<(const Time &a, const Time &b) {
    return a.nanoseconds < b.nanoseconds;
}
bool operator<=(const Time &a, const Time &b) {
    return a.nanoseconds <= b.nanoseconds;
}

bool operator==(const Time &a, const Time &b) {
    return a.nanoseconds == b.nanoseconds;
}
bool operator!=(const Time &a, const Time &b) {
    return a.nanoseconds != b.nanoseconds;
}