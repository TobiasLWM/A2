#ifndef Engine_vec2_h_
#define Engine_vec2_h_

#include <cmath>

// Class Representing a 2D Vector
class Vec2 {
    public:
    // Data Members
    float x, y;

    // Constructor
    Vec2();
    Vec2(float x, float y);

    // Directions
    static const Vec2 zero;
    static const Vec2 up;
    static const Vec2 down;
    static const Vec2 left;
    static const Vec2 right;
};

// Methods
float length(const Vec2 &v);
Vec2 unit(const Vec2 &v);
Vec2 rotate(const Vec2 &v, float angle);
float dot(const Vec2 &a, const Vec2 &b);
float distance(const Vec2 &a, const Vec2 &b);

// Overloaded Operators
Vec2 operator+(const Vec2 &a, const Vec2 &b);
Vec2& operator+=(Vec2 &a, const Vec2 &b);

Vec2 operator-(const Vec2 &a, const Vec2 &b);
Vec2& operator-=(Vec2 &a, const Vec2 &b);

Vec2 operator*(const Vec2 &a, const float b);
Vec2 operator*(const float a, const Vec2 &b);
Vec2& operator*=(Vec2 &a, const float b);
Vec2 operator/(const Vec2 &a, const float b);
Vec2& operator/=(Vec2 &a, const float b);

#endif