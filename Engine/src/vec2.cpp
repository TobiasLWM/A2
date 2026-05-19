#include "vec2.h"

#define USE_MATH_DEFINES
#include <cmath>
#include <SDL3/SDL.h>

// Make sure M_PI is defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Static variables
const Vec2 Vec2::zero  = Vec2{ 0,  0};
const Vec2 Vec2::up    = Vec2{ 0, -1};
const Vec2 Vec2::down  = Vec2{ 0,  1};
const Vec2 Vec2::left  = Vec2{-1,  0};
const Vec2 Vec2::right = Vec2{ 1,  0};

// Constructors
Vec2::Vec2() {
    x = 0;
    y = 0;
}
Vec2::Vec2(float x, float y) {
    this->x = x;
    this->y = y;
}

// Methods

// Length of the vector
float length(const Vec2 &v) {
    return sqrt(v.x*v.x + v.y*v.y);
}

// Returns a unit vector (length 1)
Vec2 unit(const Vec2 &v) {
    float len = length(v);
    return Vec2{v.x/len, v.y/len};
}

// Dot product of two vectors
float dot(const Vec2 &a, const Vec2 &b) {
    return (a.x * b.x + a.y * b.y);
}

// Rotate a vector by an angle (in degrees, clockwise)
Vec2 rotate(const Vec2 &v, float angle) {
    float radians = angle / 180 * M_PI;
    return Vec2{(float)(v.x * cos(radians) - v.y * sin(radians)), (float)(v.x*sin(radians) + v.y*cos(radians))};
}

// Distance between two vectors
float distance(const Vec2 &a, const Vec2 &b) {
    return length(a - b);
}

// Operators

// Addition
Vec2 operator+(const Vec2 &a, const Vec2 &b) {
    return Vec2 {a.x + b.x, a.y + b.y};
}

Vec2& operator+=(Vec2 &a, const Vec2 &b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

// Subtraction
Vec2 operator-(const Vec2 &a) {
    return Vec2 {-a.x, -a.y};
}

Vec2 operator-(const Vec2 &a, const Vec2 &b) {
    return Vec2 {a.x - b.x, a.y - b.y};
}

Vec2& operator-=(Vec2 &a, const Vec2 &b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

// Scalar Multiplication
Vec2 operator*(const Vec2 &a, const float b) {
    return Vec2 {a.x * b, a.y * b};
}

Vec2 operator*(const float a, const Vec2 &b) {
    return Vec2 {a * b.x, a * b.y};
}

Vec2& operator*=(Vec2 &a, const float b) {
    a.x *= b;
    a.y *= b;
    return a;
}

// Scalar Division
Vec2 operator/(const Vec2 &a, const float b) {
    return Vec2 {a.x / b, a.y / b};
}

Vec2& operator/=(Vec2 &a, const float b) {
    a.x /= b;
    a.y /= b;
    return a;
}
