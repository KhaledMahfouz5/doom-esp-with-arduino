#ifndef _esp32_doom_h
#define _esp32_doom_h

#include <stdint.h>
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ===============================
 * Engine entrypoints
 * =============================== */

void doom_init(void);
void doom_game_tick(void);

/* ===============================
 * Scene state machine
 * =============================== */

typedef enum {
    SCENE_INTRO = 0,
    SCENE_GAME,
    SCENE_MID,
    SCENE_SCORE,
} doom_scene_t;

/* Defined once in esp32-doom.c */
extern volatile doom_scene_t current_scene;

/* Scene transitions */
void jumpTo(doom_scene_t next);

/* ===============================
 * Scene tick functions
 * (one frame, non-blocking)
 * =============================== */

void loopIntroTick(void);
void loopGameTick(void);
void loopMidTick(void);
void loopScoreTick(void);

/* ===============================
 * Timing
 * =============================== */

/* ~30 FPS Doom-style tick */
#define DOOM_TICK_MS 33

#ifdef __cplusplus
}
#endif

#endif
