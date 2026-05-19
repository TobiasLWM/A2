#include <render.h>
#include <algorithm>

#include <SDL3_image/SDL_image.h>

// Renderer (in engine.cpp)
extern SDL_Renderer *renderer;

// Color Constructor
Color::Color() {
    r = 0;
    g = 0;
    b = 0;
    a = 255;
}

Color::Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

// Color constants
const Color Color::black   = {   0,   0,   0, 255};
const Color Color::red     = { 255,   0,   0, 255};
const Color Color::green   = {   0, 255,   0, 255};
const Color Color::blue    = {   0,   0, 255, 255};
const Color Color::cyan    = {   0, 255, 255, 255};
const Color Color::magenta = { 255,   0, 255, 255};
const Color Color::yellow  = { 255, 255,   0, 255};
const Color Color::white   = { 255, 255, 255, 255};

// Target Texture (used for rotations)
SDL_Texture *targetTexture = nullptr;
int targetWidth = 0, targetHeight = 0;

// Get Target Texture
SDL_Texture* getTargetTexture(float width, float height) {
    if(targetWidth < width || targetHeight < height) {
        SDL_DestroyTexture(targetTexture);
        targetTexture = nullptr;
    }

    // Create target texture if it doesn't exist
    if(targetTexture == nullptr) {
        targetWidth = ceil(width);
        targetHeight = ceil(height);
        targetTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, targetWidth, targetHeight);
        SDL_SetTextureScaleMode(targetTexture, SDL_SCALEMODE_LINEAR);
    }

    return targetTexture;
}

// Clear Screen
void clear(Color color) {
    // Call RGB clear
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

void clear(Uint8 r, Uint8 g, Uint8 b) {
    clear({r,g,b});
}

// ----------------------------------------
// Draw Primitives
// ----------------------------------------

// Draw Rectangle
void drawRect(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float angle) {
    // Check angle
    if(angle == 0.0f) {
        // Set Color
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        
        // Draw Rect
        SDL_FRect rect = {x, y, width, height};
        SDL_RenderRect(renderer, &rect);
    } else {
        // Get Target Texture (temporary texture we can draw the Rectangle into)
        SDL_Texture *target = getTargetTexture(width, height);

        // Set render target to temporary texture (render calls will draw to the texture now)
        SDL_SetRenderTarget(renderer, target);

        // Clear background to transparent
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
        SDL_RenderClear(renderer);

        // Set the SDL Renderer Colour
        SDL_SetRenderDrawColor(renderer, r, g, b, a);

        // Create a Rectangle struct for SDL
        SDL_FRect rect = {0, 0, width, height};

        // Render the Rectangle (axis-aligned)
        SDL_RenderRect(renderer, &rect);

        // Set render target back to screen framebuffer (draw calls will go to the screen)
        SDL_SetRenderTarget(renderer, nullptr);

        // Create source and destination rectangles
        SDL_FRect src_rect = {0, 0, width, height};
        SDL_FRect dst_rect = {x, y, width, height};

        // Set centre for rotation
        SDL_FPoint centre = {width/2.0f, height/2.0f};
        
        // Render texture (rotated) onto the screen
        SDL_RenderTextureRotated(renderer, target, &src_rect, &dst_rect, angle, &centre, SDL_FLIP_NONE);
    }
}

void drawRect(float x, float y, float width, float height, Color color, float angle) {
    // Call RGBA drawRect
    drawRect(x, y, width, height, color.r, color.g, color.b, color.a, angle);
}

void drawRect(Vec2 pos, Vec2 size, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float angle) {
    // Call RGBA drawRect
    drawRect(pos.x, pos.y, size.x, size.y, r, g, b, a, angle);
}

void drawRect(Vec2 pos, Vec2 size, Color color, float angle) {
    // Call RGBA drawRect
    drawRect(pos.x, pos.y, size.x, size.y, color.r, color.g, color.b, color.a, angle);
}


// Fill Rectangle
void fillRect(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float angle) {
    if(angle == 0.0) {
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        
        SDL_FRect rect = {x, y, width, height};
        SDL_RenderFillRect(renderer, &rect);
    } else {
        // Get Target Texture
        SDL_Texture *target = getTargetTexture(width, height);

        // Set render target to temporary texture
        SDL_SetRenderTarget(renderer, target);

        // Clear background to transparent
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
        SDL_RenderClear(renderer);

        // Set the SDL Renderer Colour
        SDL_SetRenderDrawColor(renderer, r, g, b, a);

         // Create a Rectangle struct for SDL
        SDL_FRect rect = {0, 0, width, height};

        // Render the Rectangle (axis-aligned)
        SDL_RenderFillRect(renderer, &rect);

        // Set render target back to screen
        SDL_SetRenderTarget(renderer, nullptr);

        // Create source and destination rectangles
        SDL_FRect src_rect = {0, 0, width, height};
        SDL_FRect dst_rect = {x, y, width, height};

        // Set centre for rotation
        SDL_FPoint centre = {width/2.0f, height/2.0f};
        
        // Render texture (rotated)
        SDL_RenderTextureRotated(renderer, target, &src_rect, &dst_rect, angle, &centre, SDL_FLIP_NONE);
    }
}

void fillRect(float x, float y, float width, float height, Color color, float angle) {
    // Call RGBA fillRect
    fillRect(x, y, width, height, color.r, color.g, color.b, color.a, angle);
}

void fillRect(Vec2 pos, Vec2 size, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float angle) {
    // Call RGBA fillRect
    fillRect(pos.x, pos.y, size.x, size.y, r, g, b, a, angle);
}

void fillRect(Vec2 pos, Vec2 size, Color color, float angle) {
    // Call RGBA fillRect
    fillRect(pos.x, pos.y, size.x, size.y, color.r, color.g, color.b, color.a, angle);
}


// Draw Ellipse
void drawEllipse(float x, float y, float rx, float ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float angle) {
    // Check angle
    if(angle == 0.0f) {
        // Render simple (axis-aligned) ellips
        ellipseRGBA(renderer, x, y, rx, ry, r, g, b, a);
    } else {
        float width = 2.0f * rx + 1;
        float height = 2.0f * ry + 1;

        // Get Target Texture
        SDL_Texture *target = getTargetTexture(width, height);

        // Set render target to temporary texture
        SDL_SetRenderTarget(renderer, target);

        // Clear background to transparent
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
        SDL_RenderClear(renderer);

        // Set the SDL Renderer Colour
        SDL_SetRenderDrawColor(renderer, r, g, b, a);

        // Render simple (axis-aligned) ellipse
        ellipseRGBA(renderer, rx, ry, rx, ry, r, g, b, a);

        // Set render target back to screen
        SDL_SetRenderTarget(renderer, nullptr);

        // Create source and destination rectangles
        SDL_FRect src_rect = {0, 0, width, height};
        SDL_FRect dst_rect = {x-rx, y-ry, width, height};

        // Set centre for rotation
        SDL_FPoint centre = {width/2.0f, height/2.0f};
        
        // Render texture (rotated)
        SDL_RenderTextureRotated(renderer, target, &src_rect, &dst_rect, angle, &centre, SDL_FLIP_NONE);
    }
}

void drawEllipse(float x, float y, float rx, float ry, Color color, float angle) {
    drawEllipse(x, y, rx, ry, color.r, color.g, color.b, color.a, angle);
}

void drawEllipse(Vec2 pos, Vec2 radii, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float angle) {
    drawEllipse(pos.x, pos.y, radii.x, radii.y, r, g, b, a, angle);
}

void drawEllipse(Vec2 pos, Vec2 radii, Color color, float angle) {
    drawEllipse(pos.x, pos.y, radii.x, radii.y, color.r, color.g, color.b, color.a, angle);
}

// Fill Ellipse
void fillEllipse(float x, float y, float rx, float ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float angle) {
    // Check angle
    if(angle == 0.0f) {
        // Fill simple (axis-aligned) ellipse
        filledEllipseRGBA(renderer, x, y, rx, ry, r, g, b, a);
    } else {
        float width = 2.0f * rx + 1;
        float height = 2.0f * ry + 1;

        // Get Target Texture
        SDL_Texture *target = getTargetTexture(width, height);

        // Set render target to temporary texture
        SDL_SetRenderTarget(renderer, target);

        // Clear background to transparent
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
        SDL_RenderClear(renderer);

        // Set the SDL Renderer Colour
        SDL_SetRenderDrawColor(renderer, r, g, b, a);

        // Fill simple (axis-aligned) ellipse
        filledEllipseRGBA(renderer, rx, ry, rx, ry, r, g, b, a);

        // Set render target back to screen
        SDL_SetRenderTarget(renderer, nullptr);

        // Create source and destination rectangles
        SDL_FRect src_rect = {0, 0, width, height};
        SDL_FRect dst_rect = {x - rx, y - ry, width, height};

        // Set centre for rotation
        SDL_FPoint centre = {rx, ry};
        
        // Render texture (rotated)
        SDL_RenderTextureRotated(renderer, target, &src_rect, &dst_rect, angle, &centre, SDL_FLIP_NONE);
    }
}

void fillEllipse(float x, float y, float rx, float ry, Color color, float angle) {
    fillEllipse(x, y, rx, ry, color.r, color.g, color.b, color.a, angle);
}

void fillEllipse(Vec2 pos, Vec2 radii, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float angle) {
    fillEllipse(pos.x, pos.y, radii.x, radii.y, r, g, b, a, angle);
}

void fillEllipse(Vec2 pos, Vec2 radii, Color color, float angle) {
    fillEllipse(pos.x, pos.y, radii.x, radii.y, color.r, color.g, color.b, color.a, angle);
}


// Draw Circle
void drawCircle(float x, float y, float radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    circleRGBA(renderer, x, y, radius, r, g, b, a);
}

void drawCircle(float x, float y, float radius, Color color) {
    drawCircle(x, y, radius, color.r, color.g, color.b, color.a);
}

void drawCircle(Vec2 pos, float radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    drawCircle(pos.x, pos.y, radius, r, g, b, a);
}

void drawCircle(Vec2 pos, float radius, Color color) {
    drawCircle(pos.x, pos.y, radius, color.r, color.g, color.b, color.a);
}

// Fill Circle
void fillCircle(float x, float y, float radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float angle) {
    filledCircleRGBA(renderer, x, y, radius, r, g, b, a);
}

void fillCircle(float x, float y, float radius, Color color, float angle) {
    fillCircle(x, y, radius, color.r, color.g, color.b, color.a);
}

void fillCircle(Vec2 pos, float radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float angle) {
    fillCircle(pos.x, pos.y, radius, r, g, b, a);
}

void fillCircle(Vec2 pos, float radius, Color color, float angle) {
    fillCircle(pos.x, pos.y, radius, color.r, color.g, color.b, color.a);
}

// Draw Triangle
void drawTriangle(float x0, float y0, float x1, float y1, float x2, float y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float angle) {
    drawTriangle({x0, y0}, {x1, y1}, {x2, y2}, {r, g, b, a}, angle);
}

void drawTriangle(float x0, float y0, float x1, float y1, float x2, float y2, Color color, float angle) {
    drawTriangle({x0, y0}, {x1, y1}, {x2, y2}, color, angle);
}

void drawTriangle(Vec2 v0, Vec2 v1, Vec2 v2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float angle) {
    drawTriangle(v0, v1, v2, {r, g, b, a}, angle);
}

void drawTriangle(Vec2 v0, Vec2 v1, Vec2 v2, Color color, float angle) {
    // Check angle
    if(angle == 0.0f) {
        trigonRGBA(renderer, v0.x, v0.y, v1.x, v1.y, v2.x, v2.y, color.r, color.g, color.b, color.a);
    } else {
        float left = std::min({v0.x, v1.x, v2.x}) - 1;
        float top = std::min({v0.y, v1.y, v2.y}) - 1;
        float width = std::max({v0.x, v1.x, v2.x}) - std::min({v0.x, v1.x, v2.x}) + 1;
        float height = std::max({v0.y, v1.y, v2.y}) - std::min({v0.y, v1.y, v2.y}) + 1;

        // Get Target Texture
        SDL_Texture *target = getTargetTexture(width, height);

        // Set render target to temporary texture
        SDL_SetRenderTarget(renderer, target);

        // Clear background to transparent
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
        SDL_RenderClear(renderer);

        // Draw Triangle
        trigonRGBA(renderer, v0.x - left, v0.y - top, v1.x - left, v1.y - top, v2.x - left, v2.y - top, color.r, color.g, color.b, color.a);

        // Set render target back to screen
        SDL_SetRenderTarget(renderer, nullptr);

        // Create source and destination rectangles
        SDL_FRect src_rect = {0, 0, width, height};
        SDL_FRect dst_rect = {left, top, width, height};

        // Set centre for rotation
        SDL_FPoint centre = {width/2, height/2};
        
        // Render texture (rotated)
        SDL_RenderTextureRotated(renderer, target, &src_rect, &dst_rect, angle, &centre, SDL_FLIP_NONE);
    }
}

// Fill Triangle
void fillTriangle(float x0, float y0, float x1, float y1, float x2, float y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float angle) {
    fillTriangle({x0, y0}, {x1, y1}, {x2, y2}, {r, g, b, a}, angle);
}

void fillTriangle(float x0, float y0, float x1, float y1, float x2, float y2, Color color, float angle) {
    fillTriangle({x0, y0}, {x1, y1}, {x2, y2}, color);
}

void fillTriangle(Vec2 v0, Vec2 v1, Vec2 v2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float angle) {
    fillTriangle(v0, v1, v2, {r, g, b, a});
}

void fillTriangle(Vec2 v0, Vec2 v1, Vec2 v2, Color color, float angle) {
    // Check angle
    if(angle == 0.0f) {
        // Fill Triangle
        filledTrigonRGBA(renderer, v0.x, v0.y, v1.x, v1.y, v2.x, v2.y, color.r, color.g, color.b, color.a);
    } else {
        float left = std::min({v0.x, v1.x, v2.x});
        float top = std::min({v0.y, v1.y, v2.y});
        float width = std::max({v0.x, v1.x, v2.x}) - std::min({v0.x, v1.x, v2.x});
        float height = std::max({v0.y, v1.y, v2.y}) - std::min({v0.y, v1.y, v2.y});

        // Get Target Texture
        SDL_Texture *target = getTargetTexture(width, height);

        // Set render target to temporary texture
        SDL_SetRenderTarget(renderer, target);

        // Clear background to transparent
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
        SDL_RenderClear(renderer);

        // Fill Triangle
        filledTrigonRGBA(renderer, v0.x - left, v0.y - top, v1.x - left, v1.y - top, v2.x - left, v2.y - top, color.r, color.g, color.b, color.a);

        // Set render target back to screen
        SDL_SetRenderTarget(renderer, nullptr);

        // Create source and destination rectangles
        SDL_FRect src_rect = {0, 0, width, height};
        SDL_FRect dst_rect = {left, top, width, height};

        // Set centre for rotation
        SDL_FPoint centre = {width/2, height/2};
        
        // Render texture (rotated)
        SDL_RenderTextureRotated(renderer, target, &src_rect, &dst_rect, angle, &centre, SDL_FLIP_NONE);
    }
}

// Draw Lines
void drawLine(float x0, float y0, float x1, float y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float angle) {
    drawLine({x0, y0}, {x1, y1}, {r, g, b, a}, angle);
}

void drawLine(float x0, float y0, float x1, float y1, Color color, float angle) {
    drawLine({x0, y0}, {x1, y1}, color, angle);
}

void drawLine(Vec2 p0, Vec2 p1, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float angle) {
    drawLine(p0, p1, {r, g, b, a}, angle);
}

void drawLine(Vec2 p0, Vec2 p1, Color color, float angle) {
    // Check angle
    if(angle == 0.0f) {
        // Draw Line
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderLine(renderer, p0.x, p0.y, p1.x, p1.y);
    } else {
        Vec2 centre = (p0 + p1) / 2.0f;
        Vec2 r0 = p0 - centre;
        Vec2 r1 = p1 - centre;

        r0 = centre + rotate(r0, angle);
        r1 = centre + rotate(r1, angle);

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderLine(renderer, r0.x, r0.y, r1.x, r1.y);
    }
}


// Draw Lines
void drawLines(float *pts, int npoints, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer,r,g,b,a);
	SDL_RenderLines(renderer, (const SDL_FPoint*) pts, npoints);
}

void drawLines(float *pts, int npoints, Color color) {
    drawLines(pts, npoints, color.r, color.g, color.b, color.a);
}


void fillTriangle(float *pts, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_Vertex vert[3];
    SDL_FColor col={r/255.0f,g/255.0f,b/255.0f};
    vert[0].position.x=pts[0];
    vert[0].position.y=pts[1];
    vert[0].color=col;
    vert[1].position.x=pts[2];
    vert[1].position.y=pts[3];
    vert[1].color=col;
    vert[2].position.x=pts[4];
    vert[2].position.y=pts[5];
    vert[2].color=col;
    SDL_RenderGeometry(renderer, NULL, vert, 3, NULL, 0);
}

// nvertices must be a multiple of 3
void fillTriangles(Vertex *vertices, int nvertices) {
    SDL_RenderGeometry(renderer, NULL, (const SDL_Vertex *)vertices, nvertices, NULL, 0);
}

// ----------------------------------------
// Draw Textures
// ----------------------------------------

void drawTexture(Texture texture, Rect src, Rect dst, float angle, Uint8 alpha) {
    SDL_FRect src_rect = {src.x, src.y, src.width, src.height};
    SDL_FRect dst_rect = {dst.x, dst.y, dst.width, dst.height};

    if(angle == 0.0f) {
        // Render Texture
        SDL_SetTextureAlphaMod(texture.texture, alpha);
        SDL_RenderTexture(renderer, texture.texture, &src_rect, &dst_rect);
    } else {
        // Calculate center
        SDL_FPoint centre = {dst.width / 2.0f, dst.height / 2.0f};

        // Render Rotated Texture
        SDL_SetTextureAlphaMod(texture.texture, alpha);
        SDL_RenderTextureRotated(renderer, texture.texture, &src_rect, &dst_rect, angle, &centre, SDL_FLIP_NONE);
    }
}

void drawTexture(Texture texture, float src_x, float src_y, float src_width, float src_height, float dst_x, float dst_y, float dst_w, float dst_h, float angle, Uint8 alpha) {
    drawTexture(texture, Rect{src_x, src_y, src_width, src_height}, Rect{dst_x, dst_y, dst_w, dst_h}, angle, alpha);
}

void drawTexture(Texture texture, Vec2 src_pos, Vec2 src_size, Vec2 dst_pos, Vec2 dst_size, float angle, Uint8 alpha) {
    drawTexture(texture, Rect{src_pos.x, src_pos.y, src_size.x, src_size.y}, Rect{dst_pos.x, dst_pos.y, dst_size.x, dst_size.y}, angle, alpha);
}

void drawTexture(Texture texture, float dst_x, float dst_y, float dst_w, float dst_h, float angle, Uint8 alpha) {
    drawTexture(texture, texture.src, Rect{dst_x, dst_y, dst_w, dst_h}, angle, alpha);
}

void drawTexture(Texture texture, Vec2 dst_pos, Vec2 dst_size, float angle, Uint8 alpha) {
    drawTexture(texture, texture.src, Rect{dst_pos.x, dst_pos.y, dst_size.x, dst_size.y}, angle, alpha);
}

void drawTexture(Texture texture, Rect dst, float angle, Uint8 alpha) {
    drawTexture(texture, texture.src, dst, angle, alpha);
}

// Draw Text
void drawText(float x, float y, char *msg, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    stringRGBA (renderer, x, y, msg, r, g, b, a);
}

// Load Texture from file
Texture loadTexture(const char *path, bool clear) {
    // Surface used to load image
    SDL_Surface *surface = NULL;

    // Try to load image from file using SDL_image
    surface = IMG_Load(path);

    if (!surface) {
        // Error
        SDL_Log("Couldn't load image: %s (%s)", path, SDL_GetError());
        exit(1);
    }

    // Force all pixels that are fully transparent (alpha = 0) to rgb (0,0,0)
    // Windows invents rgb values for transparent pixels
    if(clear) {
        // Lock Surface
        SDL_LockSurface(surface);

        // Get Pixel Pointer
        Uint32 *pixels = (Uint32 *)surface->pixels;

        // For all pixels
        for(int y = 0; y < surface->h; y++) {
            for(int x = 0; x < surface->w; x++) {
                // Read current pixel
                Uint8 r, g, b, a;
                SDL_ReadSurfacePixel(surface, x, y, &r, &g, &b, &a);

                // If pixel is completely transparent (alpha = 0)
                if(a == 0) {
                    // Write black transparent pixel
                    SDL_WriteSurfacePixel(surface, x, y, 0, 0, 0, 0);
                }
            }
        }
        // Unlock Surface
        SDL_UnlockSurface(surface);
    }
    
    // Create a texture
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (!texture) {
        // Error
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        exit(1);
    }

    // Clean up surface
    SDL_DestroySurface(surface);

    // Return texture
    return Texture{Rect{0, 0, (float)texture->w, (float)texture->h}, texture};
}

Texture loadTexture(const char *path, Rect src, bool clear) {
    // Surface used to load image
    SDL_Surface *surface = NULL;

    // Try to load image from file using SDL_image
    surface = IMG_Load(path);

    if (!surface) {
        // Error
        SDL_Log("Couldn't load image: %s (%s)", path, SDL_GetError());
        exit(1);
    }

    // Force all pixels that are fully transparent (alpha = 0) to rgb (0,0,0)
    // Windows invents rgb values for transparent pixels
    if(clear) {
        // Lock Surface
        SDL_LockSurface(surface);

        // Get Pixel Pointer
        Uint32 *pixels = (Uint32 *)surface->pixels;

        // For all pixels
        for(int y = 0; y < surface->h; y++) {
            for(int x = 0; x < surface->w; x++) {
                // Read current pixel
                Uint8 r, g, b, a;
                SDL_ReadSurfacePixel(surface, x, y, &r, &g, &b, &a);

                // If pixel is completely transparent (alpha = 0)
                if(a == 0) {
                    // Write black transparent pixel
                    SDL_WriteSurfacePixel(surface, x, y, 0, 0, 0, 0);
                }
            }
        }
        // Unlock Surface
        SDL_UnlockSurface(surface);
    }
    
    // Create a texture
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (!texture) {
        // Error
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        exit(1);
    }

    // Clean up surface
    SDL_DestroySurface(surface);

    // Return texture
    return Texture{src, texture};
}

// Load Textures
Texture loadTexture(const char *path, Vec2 pos, Vec2 size, bool clear) {
    return loadTexture(path, Rect{pos.x, pos.y, size.x, size.y}, clear); 
}


Texture loadTexture(const char *path, float x, float y, float w, float h, bool clear) {
    return loadTexture(path, Rect{x, y, w, h}, clear);
}

// SubTexture
Texture subTexture(Texture texture, float x, float y, float w, float h) {
    return Texture{Rect{x, y, w, h}, texture.texture};
}
Texture subTexture(Texture texture, Vec2 pos, Vec2 size) {
    return Texture{Rect{pos.x, pos.y, size.x, size.y}, texture.texture};
}
Texture subTexture(Texture texture, Rect src) {
    return Texture{src, texture.texture};
}