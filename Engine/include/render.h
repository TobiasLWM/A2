#ifndef Engine_render_h_
#define Engine_render_h_

// Includes
#include <SDL3/SDL.h>
#include "SDL3_gfx/SDL3_gfxPrimitives.h"

#include <vec2.h>

// Colour
class Color {
    public:
    Uint8 r, g, b, a; 

    // Constructor
    Color();
    Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a=255);

    // Constants
    static const Color black;
    static const Color red;
    static const Color green;
    static const Color blue;
    static const Color cyan;
    static const Color magenta;
    static const Color yellow;
    static const Color white;
};

// Rectangle
class Rect {
    public:
    float x, y, width, height;
};

// Texture
class Texture {
    public:
    Rect src;
    SDL_Texture *texture;
};

// Vertex
struct Vertex {
    public:
    float x,y;
    float r,g,b,a;
    float tx,ty;
};

// Clear
void clear(Uint8 r, Uint8 g, Uint8 b);
void clear(Color color);

// Draw Line
void drawLine(float x0, float y0, float x1, float y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255, float angle = 0.0f);
void drawLine(float x0, float y0, float x1, float y1, Color color, float angle = 0.0f);
void drawLine(Vec2 p0, Vec2 p1, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255, float angle = 0.0f);
void drawLine(Vec2 p0, Vec2 p1, Color color, float angle = 0.0f);

void drawLines(float *pts, int npoints, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255);
void drawLines(float *pts, int npoints, Color color);

// Draw Rectangle
void drawRect(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255, float angle = 0.0f);
void drawRect(float x, float y, float width, float height, Color color, float angle = 0.0f);
void drawRect(Vec2 pos, Vec2 size, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255, float angle = 0.0f);
void drawRect(Vec2 pos, Vec2 size, Color color, float angle = 0.0f);

// Fill Rectangle
void fillRect(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255, float angle = 0.0f);
void fillRect(float x, float y, float width, float height, Color color, float angle = 0.0f);
void fillRect(Vec2 pos, Vec2 size, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255, float angle = 0.0f);
void fillRect(Vec2 pos, Vec2 size, Color color, float angle = 0.0f);

// Draw Ellipse
void drawEllipse(float x, float y, float rx, float ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255, float angle = 0.0f);
void drawEllipse(float x, float y, float rx, float ry, Color color, float angle = 0.0f);
void drawEllipse(Vec2 pos, Vec2 radii, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255, float angle = 0.0f);
void drawEllipse(Vec2 pos, Vec2 radii, Color color, float angle = 0.0f);

// Fill Ellipse
void fillEllipse(float x, float y, float rx, float ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255, float angle = 0.0f);
void fillEllipse(float x, float y, float rx, float ry, Color color, float angle = 0.0f);
void fillEllipse(Vec2 pos, Vec2 radii, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255, float angle = 0.0f);
void fillEllipse(Vec2 pos, Vec2 radii, Color color, float angle = 0.0f);

// Draw Circle
void drawCircle(float x, float y, float radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255);
void drawCircle(float x, float y, float radius, Color color);
void drawCircle(Vec2 pos, float radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255);
void drawCircle(Vec2 pos, float radius, Color color);

// Fill Circle
void fillCircle(float x, float y, float radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255, float angle = 0.0f);
void fillCircle(float x, float y, float radius, Color color, float angle = 0.0f);
void fillCircle(Vec2 pos, float radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255, float angle = 0.0f);
void fillCircle(Vec2 pos, float radius, Color color, float angle = 0.0f);

// Draw Triangle
void drawTriangle(float x0, float y0, float x1, float y1, float x2, float y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255, float angle = 0.0f);
void drawTriangle(float x0, float y0, float x1, float y1, float x2, float y2, Color color, float angle = 0.0f);
void drawTriangle(Vec2 v0, Vec2 v1, Vec2 v2, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255, float angle = 0.0f);
void drawTriangle(Vec2 v0, Vec2 v1, Vec2 v2, Color color, float angle = 0.0f);


// Fill Triangle
void fillTriangle(float x0, float y0, float x1, float y1, float x2, float y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255, float angle = 0.0f);
void fillTriangle(float x0, float y0, float x1, float y1, float x2, float y2, Color color, float angle = 0.0f);
void fillTriangle(Vec2 v0, Vec2 v1, Vec2 v2, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255, float angle = 0.0f);
void fillTriangle(Vec2 v0, Vec2 v1, Vec2 v2, Color color, float angle = 0.0f);

void fillTriangle(float *pts, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

void fillTriangles(Vertex *vertices, int nvertices);

// Load Textures
Texture loadTexture(const char *path, bool clear = true);
Texture loadTexture(const char *path, float x, float y, float w, float h, bool clear = true);
Texture loadTexture(const char *path, Vec2 pos, Vec2 size, bool clear = true);
Texture loadTexture(const char *path, Rect src, bool clear = true);

Texture subTexture(Texture texture, float x, float y, float w, float h);
Texture subTexture(Texture texture, Vec2 pos, Vec2 size);
Texture subTexture(Texture texture, Rect src);

// Draw Texture
void drawTexture(Texture texture, float dst_x, float dst_y, float dst_w, float dst_h, float angle=0.0f, Uint8 alpha=255);
void drawTexture(Texture texture, Vec2 dst_pos, Vec2 dst_size, float angle=0.0f, Uint8 alpha=255);
void drawTexture(Texture texture, Rect dst, float angle=0.0f, Uint8 alpha=255);

void drawTexture(Texture texture, float src_x, float src_y, float src_w, float src_h, float dst_x, float dst_y, float dst_w, float dst_h, float angle=0.0f, Uint8 alpha=255);
void drawTexture(Texture texture, Vec2 src_pos, Vec2 src_size, Vec2 dst_pos, Vec2 dst_size, float angle=0.0f, Uint8 alpha=255);
void drawTexture(Texture texture, Rect src, Rect dst, float angle=0.0f, Uint8 alpha=255);

void drawText(float x, float y, char *msg, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

#endif