#ifndef _game_constants_h
#define _game_constants_h

#include <stdint.h>

// Timing
#define FRAME_TIME          66.666666   // Desired time per frame in ms (66.666666 is ~15 fps)

// Rendering resolution/memory tradeoffs
#define RES_DIVIDER         2           // Higher values result in lower horizontal resolution when rasterize and lower process and memory usage
                                        // Lower will require more process and memory, but looks nicer
#define Z_RES_DIVIDER       2           // Zbuffer resolution divider. We sacrifice resolution to save memory
#define DISTANCE_MULTIPLIER 20          // Distances are stored as uint8_t, multiplying the distance we can obtain more precision taking care
                                        // of keep numbers inside the type range. Max is 256 / MAX_RENDER_DEPTH
#define MAX_RENDER_DEPTH    12
#define MAX_SPRITE_DEPTH    8

// Level layout
#define LEVEL_WIDTH_BASE    6
#define LEVEL_WIDTH         (1 << LEVEL_WIDTH_BASE)
#define LEVEL_HEIGHT        57
#define LEVEL_SIZE          LEVEL_WIDTH / 2 * LEVEL_HEIGHT

// Scene IDs
#define INTRO                 0
#define GAME_PLAY             1
#define DIFF                  2
#define MUS                   3
#define MID                   4
#define SCORE                 5

// Gameplay
#define GUN_TARGET_POS        18
#define GUN_SHOT_POS          GUN_TARGET_POS + 8

#define ROT_SPEED             .12
#define MOV_SPEED             .1
#define MOV_SPEED_INV         2.5           // 1 / MOV_SPEED

#define JOGGING_SPEED         .005
#define ENEMY_SPEED           .04
#define FIREBALL_SPEED        .2
#define FIREBALL_ANGLES       45          // Num of angles per PI

#define MAX_ENTITIES          12           // Max num of active entities
#define MAX_STATIC_ENTITIES   24          // Max num of entities in sleep mode

#define MAX_ENTITY_DISTANCE   200         // * DISTANCE_MULTIPLIER
#define MAX_ENEMY_VIEW        90          // * DISTANCE_MULTIPLIER
#define ITEM_COLLIDER_DIST    6           // * DISTANCE_MULTIPLIER
#define ENEMY_COLLIDER_DIST   4           // * DISTANCE_MULTIPLIER
#define FIREBALL_COLLIDER_DIST 3          // * DISTANCE_MULTIPLIER
#define ENEMY_MELEE_DIST      9           // * DISTANCE_MULTIPLIER
#define WALL_COLLIDER_DIST    .2

#define ENEMY_MELEE_DAMAGE    18
#define ENEMY_FIREBALL_DAMAGE 25
#define GUN_MAX_DAMAGE        80

#endif
