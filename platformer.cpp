#include <engine.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// Simple structure to store an animation
struct Animation {
    std::vector<Texture> frames; // A vector (sequence) of individual frames
    int no_frames;               // Number of frames (just the length of the vector)
    float duration;              // Duration of animation
    bool loop;                   // Loop (does the animation loop or just play once)
    float start;
};

// List of Animation
std::vector<Animation> animations;

// Game state
enum GameState {PLAYING, LEVEL_FINISHING, LEVEL_FINISHED, MENU};
GameState game_state;

// Animation State
enum AnimState {IDLE, RUNNING, JUMPING, FALLING};

// Texture spritesheet
Texture spritesheet;

// Player
class Player {
    public:
    Vec2 pos;
    Vec2 vel;
    Vec2 size;
    bool isStanding;
    AnimState state;
};

int player_max_vel_x;
int player_min_vel_x;
int player_min_max_vel_x;

int player_max_vel_y;
int player_min_vel_y;
int player_min_max_vel_y;

bool injured;
float injured_timer;

// Platform
class Platform {
    public:
    Vec2 pos;
    Vec2 size;
};

// Blocks and tiles
struct Block {
    public:
    Vec2 pos;
    Vec2 size;
    Texture texture;
    bool isCollidable;
    int type; // 0 = to delete, 1 = normal block, 2 = column block, 3 = oxygen block, 4 = collectible, 5 = teleporter
};

struct Tile {
    public:
    Vec2 pos;
    bool isOccupied;
    Block* occupyingBlock;
};

// Enemies
class Enemy {
    public:
    Vec2 pos;
    Vec2 vel;
    Vec2 size;
    Vec2 farLeft;
    Vec2 farRight;
    int status; // 0 = alive, 1 = dead
};

std::vector<Enemy> enemies;

// Grid system for block and tile placement; each cell in the grid is 32x32 pixels
std::vector<std::vector<Tile>> grid(125, std::vector<Tile>(25));

// Vector of background tile textures
std::vector<Texture> backgroundTileTex;

// Vector of block tile textures
std::vector<Texture> blockTex;

// Rock texture
Texture rockTex;

// Vector of column tile textures
std::vector<Texture> columnTex;

// Vector of blocks
std::vector<Block> blocks;

// Vector of background tiles
std::vector<Tile> backgroundTiles;

// Player
Player player;

// Platforms
std::vector<Platform> platforms;

// Teleporter
Block teleporter;
int teleporter_state = 0; // 0 = idle, 1 = activating, 2 = beep, 3 = active

// Gravity
float gravity_time;     // Timer for gravity cycle
float gravity_period;   // Full cycle of hi>>low>>hi gravity
float gravity_theta;    // Angle for cosine wave
float gravity_phase;    // Phase of gravity (0 to 1)
float gravity_max;      // Maximum gravity
float gravity_min;      // Minimum gravity
float gravity;          // Current gravity

int gravity_background_colour = 0; // Background colour based on gravity
int gravity_background_overlay = 0; // Overlay colour based on gravity

// Scrolling
float screen_scroll_offset = 0.0f;
float screen_center = WINDOW_WIDTH / 2.0f;
Vec2 screen_pos = Vec2::zero;

// Starfield
int star_count;
struct Star {float x; float y; float size; float star_twinkle_timer;};
std::vector<Star> stars;
int level_width;
float star_off_time;

//Oxygen
float oxygen_level;
float oxygen_max;
float oxygen_min;
float oxygen_rate; // Rate at which oxygen depletes
Texture cannister_tex;
Texture highlight_tex;
Texture oxygen_fill_tex;
Texture oxygen_block_tex;
Vec2 oxygen_pos;

// Health
int health_max;
int health_min;
int health;
Texture health_tex;
Texture health_background;
Vec2 health_pos;

// Score
float score;
std::string scoreText; // String to hold the text that tells the player their final score
char* scoreTextPtr; //pointer for rendering 
std::string gameResultText; // String to hold the text that tells the playeer the total points they've accumulated
char* gameResultTextPtr; //pointer for rendering 

// Level finish time
float level_finish_time;

// Stars
Vec2 screen_star_pos;

// Earth
Texture earth_texture;
Vec2 earth_size;

Vec2 earth_pos;
Vec2 earth_screen_pos = earth_pos;

Vec2 earth_orbit_center = Vec2::zero;
int earth_orbit_radius_x;
int earth_orbit_radius_y;

float earth_time = 0.0f;                    // Timer for gravity cycle
float earth_period = gravity_period*2;      // Full cycle of hi>>low>>hi gravity
float earth_theta = 0.0f;
float earth_phase_shift = 0.0f;              // Phase shift to correct earth orbit position

// Create platforms of block structures
void createPlatform(int length, int xStart, int y, std::vector<Block> &blocks, std::vector<std::vector<Tile>> &grid) {
    for(int i = 0; i < length; i++) {
        int randomIndex = rand() % blockTex.size();
        blocks.push_back({ {grid[xStart + i][y].pos.x, grid[xStart + i][y].pos.y}, {32, 32}, blockTex[randomIndex], true, 1 });
        grid[xStart + i][y].isOccupied = true;
        grid[xStart + i][y].occupyingBlock = &blocks.back(); 
    }
}

void createRock(int x, int y, std::vector<Block> &blocks, std::vector<std::vector<Tile>> &grid) {
    blocks.push_back({ grid[x][y].pos, {32, 32}, rockTex, true, 1 });
    grid[x][y].isOccupied = true;
    grid[x][y].occupyingBlock = &blocks.back();
}

void createRockPlatform(int length, int xStart, int y, std::vector<Block> &blocks, std::vector<std::vector<Tile>> &grid) {
    for(int i = 0; i < length; i++) {
        createRock(xStart + i, y, blocks, grid);
    }
}

// Create oxygen block
void createOxygenBlock(int x, int y, std::vector<Block> &blocks, std::vector<std::vector<Tile>> &grid) {
    blocks.push_back({ grid[x][y].pos, {32, 32}, oxygen_block_tex, false, 3 });
    grid[x][y].isOccupied = true;
    grid[x][y].occupyingBlock = &blocks.back();
}

// Create column blocks
void createColumn(int height, int x, int yBottom, std::vector<Block> &blocks, std::vector<std::vector<Tile>> &grid) {
    for(int i = 0; i < height; i++) {
        if(i == height - 1) {
            blocks.push_back({ grid[x][yBottom - i].pos, {32, 32}, columnTex[0], true, 2 });
        } else if(i == 0 && height > 1){
            blocks.push_back({ grid[x][yBottom - i].pos, {32, 32}, columnTex[2], true, 2 });
        } else {
            blocks.push_back({ grid[x][yBottom - i].pos, {32, 32}, columnTex[1], true, 2 });
        }
        grid[x][yBottom - i].isOccupied = true;
        grid[x][yBottom - i].occupyingBlock = &blocks.back(); 
    }
}

void gameOver() {
    score = 0.0f;
    scoreText = "Final Score: 0";
    scoreTextPtr = const_cast<char*>(scoreText.c_str());
    gameResultText = "Game Over!";
    gameResultTextPtr = const_cast<char*>(gameResultText.c_str());
    game_state = MENU;
}

// Check for collisions between player and tile
void collisionsBlock(Player &player, Block &block) {
    // Bounds of player & platform
    //Block platform = *grid[x][y].occupyingBlock;
    //std::cout << "Block Collided X: " << platform.pos.x << std::endl;

    float playerLeft   = player.pos.x - player.size.x/2;
    float playerRight  = player.pos.x + player.size.x/2;
    float playerTop    = player.pos.y - player.size.y/2;
    float playerBottom = player.pos.y + player.size.y/2;

    float platformLeft   = block.pos.x; 
    float platformRight  = block.pos.x + block.size.x;
    float platformTop    = block.pos.y;
    float platformBottom = block.pos.y + block.size.y;

    // Check if player and tile are colliding
    if((playerLeft < platformRight) && (playerRight > platformLeft) &&
        (playerTop < platformBottom) && (playerBottom > platformTop)) {
            if(block.isCollidable) {
                // Process Collision
                double tx = 1000000;
                double ty = 1000000;

                // If player is going left
                if(player.vel.x < 0) {
                    // Calculate time since collision in X
                    tx = (playerLeft - platformRight) / player.vel.x;
                } else if(player.vel.x > 0) {
                    // Calculate time since collision in X
                    tx = (playerRight - platformLeft) / player.vel.x;
                }

                // If player is going up
                if(player.vel.y < 0) {
                    // Calculate time since collision in Y
                    ty = (playerTop - platformBottom) / player.vel.y;
                } else if(player.vel.y > 0) {
                    // Calculate time since collision in Y
                    ty = (playerBottom - platformTop) / player.vel.y;
                }

                // Work out which collision to apply
                if(tx < ty) {
                    // Collided horizontally first
                    if(player.vel.x < 0) {
                        // Stop player moving
                        player.vel.x = 0;

                        // Correct position
                        player.pos.x = platformRight + player.size.x/2;
                    } else {
                        // Stop player moving
                        player.vel.x = 0;

                        // Correct position
                        player.pos.x = platformLeft - player.size.x/2;
                    }
                } else {
                    // Collided vertically first
                    if(player.vel.y < 0) {
                        // Stop player moving
                        player.vel.y = 0;

                        // Correct position
                        player.pos.y = platformBottom + player.size.y/2;
                    } else {
                        // Stop player moving
                        player.vel.y = 0;
                
                        // Correct position
                        player.pos.y = platformTop - player.size.y/2;

                        // Player is standing
                        player.isStanding = true;
                    }
                }

            } else if(!block.isCollidable) {
                // Process non-collidable block (e.g. oxygen)
                if(block.type == 3) {
                    // Collect Oxygen, increase oxygen level, remove block from game
                    oxygen_level +=20;
                    if(oxygen_level > oxygen_max) {
                        oxygen_level = oxygen_max;
                    }
                    // Remove block from vector
                    block.type = 0;
                    grid[block.pos.x/32][block.pos.y/32].isOccupied = false;
                    blocks.erase(std::remove_if(blocks.begin(), blocks.end(), [](Block &b){ return b.type == 0; }), blocks.end());
                } else if (block.type == 5) {
                    // Teleporter block
                    if(teleporter_state == 0) {
                        block.type = 4;
                        //teleporter_state = 1; // Start activation
                        game_state = LEVEL_FINISHING; // Level finishing
                        // Calculate score
                        level_finish_time = getTimeInSeconds();
                        score += oxygen_level;
                        float time_bonus = 200 - level_finish_time;
                        if(time_bonus > 0) {
                            score += time_bonus;
                        }
                        scoreText = "Final Score: " + std::to_string((int)score);
                        scoreTextPtr = const_cast<char*>(scoreText.c_str());
                        gameResultText = "Level Complete!";
                        gameResultTextPtr = const_cast<char*>(gameResultText.c_str());
                        //std::cout << "Teleporter Activated! State now: " << teleporter_state << std::endl;
                    }
                }
            }
        }
}

void collisionsEnemy(Player &player, Enemy &enemy) {
    float playerLeft   = player.pos.x - player.size.x/2;
    float playerRight  = player.pos.x + player.size.x/2;
    float playerTop    = player.pos.y - player.size.y/2;
    float playerBottom = player.pos.y + player.size.y/2;

    float enemyLeft   = enemy.pos.x - enemy.size.x/2;
    float enemyRight  = enemy.pos.x + enemy.size.x/2;
    float enemyTop    = enemy.pos.y - enemy.size.y/2;
    float enemyBottom = enemy.pos.y + enemy.size.y/2;

    // Check if player and tile are colliding
    if((playerLeft < enemyRight) && (playerRight > enemyLeft) &&
        (playerTop < enemyBottom) && (playerBottom > enemyTop)) {
            // Collision with enemy; reverse velocity; if uninjured reduce health, set injured status
            player.vel.x = -player.vel.x * 1.5;
            player.vel.y = -player.vel.y * 1.5;
            
            if(!injured) {
                if(health > 1) {
                    health -= 1;
                    injured = true;
                    injured_timer = 0.0f;
                } else {
                    // If health was on 1, game over
                    gameOver();
                }
            }
        }
}

// Check for collisions between player and block
void collisionsBroad(Player &player, std::vector<Block> &blocks, std::vector<std::vector<Tile>> &grid) {
    //TODO: first broad check for closest blocks to player via grid, then only check collisions with those blocks
}

// Load Animation (from series of files)
Animation loadAnimation(const char *file_base, int no_frames, float duration, bool loop) {
    // File name
    char file[1024];

    // Animation
    Animation result;

    // Load frames
    for(int i = 0; i < no_frames; i++) {
        snprintf(file, 1024, "%s%03d.png", file_base, i);
        result.frames.push_back(loadTexture(file));
    }

    // Set parameters
    result.no_frames = result.frames.size();
    result.duration = duration;
    result.loop = loop;

    // Return
    return result;
}

// Create platforms, columns, and pickups for level 1
void populateLevel1() {
    // Ground platforms
    createPlatform(34, 0, 22, blocks, grid);
    createPlatform(7, 36, 22, blocks, grid);
    createPlatform(38, 45, 22, blocks, grid);
    createPlatform(34, 85, 22, blocks, grid);

    // Smaller platforms 
    createPlatform(3, 10, 19, blocks, grid);
    createPlatform(2, 38, 19, blocks, grid);
    createPlatform(4, 40, 16, blocks, grid);
    createPlatform(3, 45, 16, blocks, grid);
    createPlatform(1, 47, 19, blocks, grid);
    createPlatform(1, 50, 19, blocks, grid);
    createPlatform(3, 53, 19, blocks, grid);
    createPlatform(1, 54, 16, blocks, grid);
    createPlatform(1, 59, 19, blocks, grid);
    createPlatform(2, 60, 16, blocks, grid);
    createPlatform(3, 64, 16, blocks, grid);
    createPlatform(1, 65, 19, blocks, grid);
    createPlatform(2, 93, 16, blocks, grid);

    // Create enemies
    Enemy enemy1 = {grid[11][18].pos, {10, 0}, {32, 32}, grid[10][18].pos, grid[12][18].pos, 0};
    enemies.push_back(enemy1);

    // Create rocks
    createRockPlatform(4, 67, 21, blocks, grid);
    createRockPlatform(3, 68, 20, blocks, grid);
    createRockPlatform(2, 69, 19, blocks, grid);
    createRockPlatform(1, 70, 18, blocks, grid);

    createRockPlatform(4, 73, 21, blocks, grid);
    createRockPlatform(3, 73, 20, blocks, grid);
    createRockPlatform(2, 73, 19, blocks, grid);
    createRockPlatform(1, 73, 18, blocks, grid);

    createRockPlatform(4, 79, 21, blocks, grid);
    createRockPlatform(3, 80, 20, blocks, grid);
    createRockPlatform(2, 81, 19, blocks, grid);
    createRockPlatform(1, 82, 18, blocks, grid);

    createRockPlatform(4, 85, 21, blocks, grid);
    createRockPlatform(3, 85, 20, blocks, grid);
    createRockPlatform(2, 85, 19, blocks, grid);
    createRockPlatform(1, 85, 18, blocks, grid);

    for(int i = 2; i < 10; i++) {
        createRockPlatform(i, 108-i, 12+i, blocks, grid);
    }

    createRock(111, 21, blocks, grid);

    // Create columns
    createColumn(2, 15, 21, blocks, grid);
    createColumn(2, 20, 21, blocks, grid);
    createColumn(3, 24, 21, blocks, grid);
    createColumn(3, 30, 21, blocks, grid);
    createColumn(2, 91, 21, blocks, grid);
    createColumn(2, 98, 21, blocks, grid);

    // Create oxygen blocks
    createOxygenBlock(20, 18, blocks, grid);
    createOxygenBlock(17, 15, blocks, grid);
    teleporter = { grid[115][21].pos, {32, 16}, subTexture(spritesheet, 128, 32, 32, 16), false, 5 };
    blocks.push_back(teleporter);
    teleporter_state = 0;
}

void player_movement(float dt, float gravity, Player &player, std::vector<Block> &blocks, std::vector<std::vector<Tile>> &grid, std::vector<Animation> &animations)
{
    // Right Key
    if (keyIsPressed(KEY_RIGHT) || keyIsPressed(KEY_D))
    {
        // player.vel.x += 250;
        player.vel.x += player_min_vel_x + player_min_max_vel_x * gravity_phase; // Player speed increases with gravity
    }

    // Left Key
    if (keyIsPressed(KEY_LEFT) || keyIsPressed(KEY_A))
    {
        // player.vel.x -= 250;
        player.vel.x -= player_min_vel_x + player_min_max_vel_x * gravity_phase;
    }

    // Jump
    if (keyPressedThisFrame(KEY_SPACE) || keyPressedThisFrame(KEY_UP) || keyPressedThisFrame(KEY_W))
    {
        // player.vel.y = -750; // Initial setting
        if (player.isStanding)
        {
            player.vel.y = player_max_vel_y;
            player.state = JUMPING;
            animations[2].start = getTimeInSeconds();
        }
    }

    // Apply gravity
    // player.vel.y += 981 * dt; // Earth Gravity
    player.vel.y += gravity * dt; // Gravity changes

    // Move Player
    player.pos += player.vel * dt;
    if (player.pos.x < player.size.x / 2)
    {
        player.pos.x = player.size.x / 2;
    }
    else if (player.pos.x > level_width - player.size.x / 2)
    {
        player.pos.x = level_width - player.size.x / 2;
    }

    // Clear standing flag
    player.isStanding = false;

    // Process collisions
    for (int i = 0; i < blocks.size(); i++)
    {
        collisionsBlock(player, blocks[i]);
    }
    
    // Only process enemy collision if it has been long enough since last injury
    if(injured == true) {
        injured_timer += dt;
        if(injured_timer > 1.0f) {
            injured = false;
            injured_timer = 0.0f;
        }
    } else {
        if(enemies.size() > 0) {
            for(int i = 0; i < enemies.size(); i++) {
                collisionsEnemy(player, enemies[i]);
            }
        }
    }

    // Update Animation
    if (player.state == IDLE)
    {
        // Idle Transitions
        if (player.isStanding && player.vel.x != 0)
        {
            player.state = RUNNING;
        }
        else if (player.isStanding == false)
        {
            player.state = FALLING;
        }
    }
    else if (player.state == RUNNING)
    {
        // Running Transitions
        if (player.isStanding && player.vel.x == 0)
        {
            player.state = IDLE;
            animations[0].start = getTimeInSeconds();
        }
        else if (player.isStanding == false)
        {
            player.state = FALLING;
        }
    }
    else if (player.state == JUMPING)
    {
        // Jumping Transitions
        if (player.isStanding && player.vel.x == 0)
        {
            player.state = IDLE;
        }
        else if (player.isStanding && player.vel.x != 0)
        {
            player.state = RUNNING;
        }
        else if (getTimeInSeconds() > animations[2].start + animations[2].duration)
        {
            player.state = FALLING;
        }
    }
    else if (player.state == FALLING)
    {
        // Falling Transitions
        if (player.isStanding && player.vel.x == 0)
        {
            player.state = IDLE;
        }
        else if (player.isStanding && player.vel.x != 0)
        {
            player.state = RUNNING;
        }
    }
}

void enemy_movement(std::vector<Enemy> &enemies, std::vector<Block> &blocks, std::vector<std::vector<Tile>> &grid, float dt) {
    //if enemies isn't empty, move enemies
    if(enemies.size() > 0) {
        for(int i = 0; i < enemies.size(); i++) {
            // Move enemy; if enemy reaches edge of it's patrol area, turn around
            enemies[i].pos += enemies[i].vel * dt;
            if(enemies[i].pos.x < enemies[i].farLeft.x + 16 || enemies[i].pos.x > enemies[i].farRight.x + 16) {
                enemies[i].vel.x = -enemies[i].vel.x;
            }
        }
    }
}

// Initialise (called once at start)
void init() {
    setWindowTitle("Platformer");
    game_state = PLAYING; // Set game state to playing

    // Load spritesheet
    spritesheet = loadTexture("./assets/images/spritesheet.png");

    // Load animations
    // 0 = Idle, 1 = Run, 2 = Jump, 3 = Jump Peak, 4 = Teleporter, 5 = Enemy one
    animations.push_back(loadAnimation("./assets/images/astronautIdle", 3, 1.0f, true));
    animations.push_back(loadAnimation("./assets/images/aRun", 6, 1.0f, true));
    animations.push_back(loadAnimation("./assets/images/Jump__", 10, 0.5f, false));
    //loadAnimation("assest/images/Jump__", 
    animations.push_back(Animation {std::vector<Texture>({loadTexture("./assets/images/Jump__009.png")}), 1, 1.0f, true});
    animations.push_back(loadAnimation("./assets/images/teleporter__", 17, 8.0f, true));
    animations.push_back(loadAnimation("./assets/images/alien1__", 2, 0.5f, true));

    // Oxygen
    // Load oxygen textures
    cannister_tex = subTexture(spritesheet, 128, 0, 64, 16);
    highlight_tex = subTexture(spritesheet, 192, 0, 64, 16);
    oxygen_fill_tex = subTexture(spritesheet, 128, 16, 64, 16);
    oxygen_block_tex = subTexture(spritesheet,256, 0, 32, 32);

    // Load health textures
    health_tex = subTexture(spritesheet, 160, 32, 64, 16);
    health_background = subTexture(spritesheet, 160, 48, 64, 16);

    // Set oxygen levels
    oxygen_max = 50;
    oxygen_min = 0;
    oxygen_level = 25;
    oxygen_rate = 1;

    // Score
    score = 0.0f;

    // Create Player
    player.pos = Vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
    player.vel = Vec2::zero;
    player.size = Vec2(32, 64);

    player_max_vel_x = 200;
    player_min_vel_x = 100;
    player_min_max_vel_x = player_max_vel_x - player_min_vel_x;

    player_max_vel_y = -300;
    player_min_vel_y = -100;
    player_min_max_vel_y = player_max_vel_y - player_min_vel_y;

    // Health
    health_max = 3;
    health_min = 0;
    health = 3;

    // Create a vector of tiles to act as a grid system for block and tile placement; each cell in the grid is 32x32 pixels
    for (int x = 0; x < 125; x++) {
        for (int y = 0; y < 25; y++) {
            grid[x][y] = {{ x * 32, y * 32 }, false, nullptr};
        }
    }

    // Load block tile textures into blockTex vector
    blockTex.push_back(subTexture(spritesheet, 0, 32, 32, 32));
    blockTex.push_back(subTexture(spritesheet, 32, 32, 32, 32));
    blockTex.push_back(subTexture(spritesheet, 64, 32, 32, 32));

    // Load rock texture
    rockTex = subTexture(spritesheet, 96, 32, 32, 32);

    // Load column tile textures into columnTex vector; first top of column, then middle, then bottom
    columnTex.push_back(subTexture(spritesheet, 288, 0, 32, 32));
    columnTex.push_back(subTexture(spritesheet, 288, 32, 32, 32));
    columnTex.push_back(subTexture(spritesheet, 288, 64, 32, 32));


    // Create platforms
    level_width = 4000;
    populateLevel1();

    // Create starfield
    // level_width = 4000;
    screen_star_pos = Vec2::zero;
    star_off_time = 0.2f; // Time for star to be off during twinkle

    star_count = 200;
    for(int i = 0; i < star_count; i++) {
        Star star;
        star.x = uniform(0, level_width + WINDOW_WIDTH/2); // Screen scrolls after 1/2 window_width
        star.y = uniform(0, WINDOW_HEIGHT);
        star.size = uniform(1, 3);
        star.star_twinkle_timer = 0; // Randomly set star on or off for twinkling effect
        stars.push_back(star);
    }   

    // Earth
    earth_texture = subTexture(spritesheet, 0, 64, 64, 64);
    earth_size = Vec2(64, 64);

    earth_pos = Vec2::zero;
    earth_screen_pos = earth_pos;

    earth_orbit_center = Vec2(level_width/2 - earth_size.x/2, WINDOW_HEIGHT);
    earth_orbit_radius_x = level_width/2 + earth_size.x;
    earth_orbit_radius_y = WINDOW_HEIGHT;
    earth_period = gravity_period * 2;      // gravity_period is light-dark // earth_period is light-dark-light

    earth_time = 0.0f;
    earth_period = gravity_period*2;
    earth_theta = 0.0f;
    earth_phase_shift = 0.0f;

    // Gravity
    gravity_time = 0.0f;          // Timer for gravity cycle
    gravity_period = 20.0f;      // Full cycle of hi>>low>>hi gravity
    gravity_theta = 0.0f;    // Angle for cosine wave
    gravity_phase = 0.0f;                    // Phase of gravity (0 to 1)
    gravity_max = 981;            // Maximum gravity
    gravity_min = 160;            // Minimum gravity
    gravity = 0.0f; // Current gravity

    gravity_background_colour = 0; // Background colour based on gravity
}

// Update Game
void update(float dt) {
    if(game_state == PLAYING || game_state == LEVEL_FINISHING || game_state == LEVEL_FINISHED) {
        player.vel.x = 0;

        // Scrolling
        screen_scroll_offset = player.pos.x - screen_center;
        if (screen_scroll_offset < 0) {
            screen_scroll_offset = 0; // Prevent walking past the left edge
        }
        
        if (screen_scroll_offset > level_width - WINDOW_WIDTH) {
            screen_scroll_offset = level_width - WINDOW_WIDTH; // Prevent walking past the right edge
        }

        // Gravity
        gravity_time += dt; // Timer for gravity cycle
        if (gravity_time > 2 * gravity_period)
        {
            gravity_time = 0; // Reset Timer
        }
        gravity_theta = (2.0f * M_PI * gravity_time) / gravity_period;       // Angle for cosine wave
        gravity_phase = (cosf(gravity_theta) + 1) / 2;                       // Phase of gravity (0 to 1)
        gravity = gravity_min + gravity_phase * (gravity_max - gravity_min); // Current gravity
        gravity_background_colour = (int)(255.0f * (gravity_phase));         // Background colour based on gravity
        if (gravity_background_colour < 55)
        {
            gravity_background_overlay = 200;
        }
        else
        {
            gravity_background_overlay = 255 - gravity_background_colour;
        }

        // Earth ellipctical orbit
        earth_time = gravity_time;         // Timer for earth orbit
        earth_theta = (2.0f * M_PI * earth_time) / gravity_period;    // Angle for cosine wave
        earth_phase_shift = 2* M_PI * 90/360;              // Phase shift to start at top of screen

        earth_pos.x = earth_orbit_center.x + cosf(earth_theta + earth_phase_shift) * earth_orbit_radius_x;
        earth_pos.y = earth_orbit_center.y - sinf(earth_theta + earth_phase_shift) * earth_orbit_radius_y;
    
        if(game_state == PLAYING) {
            player_movement(dt, gravity, player, blocks, grid, animations);
            enemy_movement(enemies, blocks, grid, dt);
            // Oxygen depletion
            oxygen_level -= oxygen_rate * dt;

            // Check for game over
            if(oxygen_level <= oxygen_min) {
                gameOver();
            }
            if(player.pos.y > WINDOW_HEIGHT + player.size.y) {
                gameOver();
            }
        } else if(game_state == LEVEL_FINISHED) {
            player.vel = Vec2::zero; // Stop player movement
            player.state = IDLE; // Set player animation to idle
            std::cout << "Level Finished, level finish time: " << level_finish_time << std::endl;
            if(teleporter_state == 1) {
                if(getTimeInSeconds() > animations[4].start + animations[4].duration - 4.0f) {
                    player.pos.y = -80; // Move player off screen at level end
                }
            }
            if(getTimeInSeconds() > level_finish_time + 8.2f) {
                game_state = MENU;
            }
        } else if(game_state == LEVEL_FINISHING) {
            // Once the player has touched teleporter, move them to center of teleporter and then halt
            // If player is to the right of teleporter edge, move left
            if(player.pos.x > teleporter.pos.x + teleporter.size.x/2 && player.pos.x < teleporter.pos.x + teleporter.size.x/2 + 50) {
                player.vel.x -= player_min_vel_x + player_min_max_vel_x * gravity_phase;
                player.pos += player.vel * dt;
                if (player.pos.x < player.size.x / 2)
                {
                    player.pos.x = player.size.x / 2;
                }
                else if (player.pos.x > level_width - player.size.x / 2)
                {
                    player.pos.x = level_width - player.size.x / 2;
                }
                if(player.pos.x <= teleporter.pos.x + teleporter.size.x/2) {
                    game_state = LEVEL_FINISHED;
                    teleporter_state = 1;
                }
            } else if (player.pos.x + player.size.x > teleporter.pos.x && player.pos.x + player.size.x < teleporter.pos.x + teleporter.size.x) {
                std::cout << "Player in teleporter zone" << std::endl;
                // If player is to the left of teleporter, move right
                player.vel.x += player_min_vel_x + player_min_max_vel_x * gravity_phase;
                player.pos += player.vel * dt;
                if (player.pos.x < player.size.x / 2)
                {
                    player.pos.x = player.size.x / 2;
                }
                else if (player.pos.x > level_width - player.size.x / 2)
                {
                    player.pos.x = level_width - player.size.x / 2;
                }
                if(player.pos.x + player.size.x/2 >= teleporter.pos.x + teleporter.size.x/2) {
                    std::cout << "Player centered on teleporter, level finished!" << std::endl;
                    game_state = LEVEL_FINISHED;
                    teleporter_state = 1;
                }
            }
        }
    } else if(game_state == MENU) {
        //TODO: create menu and implement
        // If player presses r, restart game
        if(keyPressedThisFrame(KEY_R)) {
            // Reset game state and variables
            blocks.clear();
            enemies.clear();
            player.pos = Vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
            player.vel = Vec2::zero;
            health = health_max;
            platforms.clear();
            oxygen_level = 25;
            score = 0.0f;
            populateLevel1();
            game_state = PLAYING;
        } else if (keyPressedThisFrame(KEY_X)) {
            close();
        }
    }

    // Starfield /// WORK ON THIS
    for(int i = 0; i < star_count; i++) {
        stars[i].star_twinkle_timer -= dt;
        if (stars[i].star_twinkle_timer > 0) {
            continue;
        }

        if (stars[i].star_twinkle_timer < 0) {
            stars[i].star_twinkle_timer = 0;
        }

        if (uniform(0,1) < 0.001f) {
                stars[i].star_twinkle_timer = star_off_time;
            }
    }
}

// Draw Animation
void drawAnimation(Animation anim, Vec2 pos, Vec2 size) {
    float elapsed = getTimeInSeconds() - anim.start;
    int index = (elapsed / anim.duration) * anim.no_frames;

    if(anim.loop) {
        index = index % anim.no_frames;
    } else {
        index = min(index, anim.no_frames-1);
    }
    drawTexture(anim.frames[index], pos, size);
}

// Render Game
void render(float lag) {
    // White background
    clear(Color(24, 20, gravity_background_colour));
    //clear(gravity_background_colour,gravity_background_colour,gravity_background_colour);

    if(game_state == PLAYING || game_state == LEVEL_FINISHING || game_state == LEVEL_FINISHED) {
        // Scrolling screen
        Vec2 screen_pos = player.pos;
        screen_pos.x -= screen_scroll_offset;

        // Draw starfield
        for(int i = 0; i < star_count; i++) {
            Vec2 star_pos = Vec2(stars[i].x, stars[i].y);
            // star_pos.x -= screen_scroll_offset * (stars[i].x / level_width); // Stretching starfield 
            
            screen_star_pos = star_pos;
            screen_star_pos.x -= screen_scroll_offset/ (3*(4-stars[i].size));
            
            if (stars[i].star_twinkle_timer == 0) {
                fillEllipse(screen_star_pos, Vec2(stars[i].size, stars[i].size), Color::white);
            }
        }

        //Draw blocks
        for(int i = 0; i < blocks.size(); i++) {
            Vec2 screen_block_pos = blocks[i].pos;
            screen_block_pos.x -= screen_scroll_offset;
            if(blocks[i].type != 5 && blocks[i].type != 4) { // Don't draw teleporter block (type 5) as it is drawn separately with animation, and don't draw blocks marked for deletion (type 0)
                drawTexture(blocks[i].texture, screen_block_pos, {32, 32});
            } 
        }

        // Draw teleporter (one tile below actual position so it is collided with by walking on top of)
        Vec2 screen_teleporter_pos = teleporter.pos;
        screen_teleporter_pos.x -= screen_scroll_offset;
        screen_teleporter_pos.y += 32;
        if(teleporter_state == 0) {
            drawTexture(teleporter.texture, screen_teleporter_pos, teleporter.size);
            //std::cout << "Teleporter State: " << teleporter_state << std::endl;
        } else if(teleporter_state == 1) {
            //std::cout << "Playing Teleporter Animation, State: " << teleporter_state << std::endl;
            drawAnimation(animations[4], {screen_teleporter_pos.x, screen_teleporter_pos.y - 60}, {32, 76});
        }

        // Draw Earth / Sun??
        earth_screen_pos = earth_pos;
        earth_screen_pos.x -= screen_scroll_offset;
        drawTexture(earth_texture, earth_screen_pos, earth_size, 0);

        if(player.state == IDLE) {
            // Play Idle Animation
            Vec2 size = Vec2(player.size.x, player.size.y);
            int pos_y = size.y/2;
            drawAnimation(animations[0], Vec2(screen_pos.x-size.x/2, screen_pos.y - pos_y+6), size);
        } else if(player.state == RUNNING) {
            // Play Running Animation
            if(player.vel.x < 0) {
                Vec2 size = Vec2(-44, player.size.y);
                int pos_y = size.y/2;
                //size.x *= 376.f/290.f;
                //size.y *= 520.f/500.f;
                drawAnimation(animations[1], Vec2(screen_pos.x-size.x/2, screen_pos.y - pos_y+6), size);
                //drawAnimation()
            } else {
                Vec2 size = Vec2(44, player.size.y);
                int pos_y = size.y/2;
                //size.x *= 376.f/290.f;
                //size.y *= 520.f/500.f;
                drawAnimation(animations[1], Vec2(screen_pos.x-size.x/2, screen_pos.y - pos_y+6), size);
            }
        } else if(player.state == JUMPING) {
            // Play Jumping Animation
            if(player.vel.x < 0) {
                Vec2 size = Vec2(-player.size.x, player.size.y);
                size.x *= 399.f/290.f;
                size.y *= 543.f/500.f;
                drawAnimation(animations[2], screen_pos-size/2, size);
            } else {
                Vec2 size = player.size;
                size.x *= 399.f/290.f;
                size.y *= 543.f/500.f;
                drawAnimation(animations[2], screen_pos-size/2, size);
            }
        } else if(player.state == FALLING) {
            // Play Falling Animation
            if(player.vel.x < 0) {
                Vec2 size = Vec2(-player.size.x, player.size.y);
                size.x *= 399.f/290.f;
                size.y *= 543.f/500.f;
                drawAnimation(animations[3], screen_pos-size/2, size);
            } else {
                Vec2 size = player.size;
                size.x *= 399.f/290.f;
                size.y *= 543.f/500.f;
                drawAnimation(animations[3], screen_pos-size/2, size);
            }
        }

        // Draw oxygen UI
        drawTexture(oxygen_fill_tex, grid[1][1].pos, {oxygen_level + 8, 16});
        drawTexture(cannister_tex, grid[1][1].pos, {64, 16});
        drawTexture(highlight_tex, grid[1][1].pos, {64, 16});
        drawTexture(subTexture(spritesheet, 192, 16, 64, 16), grid[1][1].pos, {64, 16});

        // Draw health UI
        drawTexture(health_background, grid[1][2].pos, {64, 16});
        drawTexture(cannister_tex, grid[1][2].pos, {64, 16});
        health_tex = subTexture(spritesheet, 160, 32, 16 * health + 8, 16);
        drawTexture(health_tex, grid[1][2].pos, {16 * health + 8, 16});
        drawTexture(highlight_tex, grid[1][2].pos, {64, 16});

        // Draw enemies
        if(enemies.size() > 0) {
            for(int i = 0; i < enemies.size(); i++) {
                if(enemies[i].vel.x < 0) {
                    Vec2 screen_enemy_pos = enemies[i].pos;
                    screen_enemy_pos.x -= screen_scroll_offset;
                    Vec2 size = Vec2(32, enemies[i].size.y);
                    drawAnimation(animations[5], Vec2(screen_enemy_pos.x-size.x/2, screen_enemy_pos.y + 6), size);
                } else {
                    Vec2 screen_enemy_pos = enemies[i].pos;
                    screen_enemy_pos.x -= screen_scroll_offset;
                    Vec2 size = Vec2(-32, enemies[i].size.y);
                    drawAnimation(animations[5], Vec2(screen_enemy_pos.x-size.x/2, screen_enemy_pos.y + 6), size);
                }
            }
        }
        //drawTexture(highlight_tex, grid[1][3].pos, {64, 16});
    } else if(game_state == MENU) {
        fillRect(Vec2::zero, Vec2(WINDOW_WIDTH, WINDOW_HEIGHT), Color::black);
        drawText(grid[13][15].pos.x, grid[13][15].pos.y, gameResultTextPtr, 247, 118, 34, 255);
        drawText(grid[13][18].pos.x, grid[13][18].pos.y, scoreTextPtr, 247, 118, 34, 255);
        drawText(grid[13][16].pos.x, grid[13][16].pos.y, "Press X to quit, press R to restart", 247, 118, 34, 255);
    }
}

// Close the Game
void close() {
    // TODO: implement save file?
    exit(0);
}