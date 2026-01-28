/*
   Sound abstraction for Doom-Nano-Brutality ESP32 port.
   Original implementation used AVR timer ISR; this version provides a non-blocking stub.
   Future: replace with PWM/I2S task that never blocks the game loop.
*/

#ifndef _sound_h
#define _sound_h

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Sound data stored in flash as const (no PROGMEM on ESP32)
#define MUS_S1_SND_LEN 38
extern const uint8_t mus_s1_snd[];

#define MUS_P1_SND_LEN 19
extern const uint8_t mus_p1_snd[];

#define MUS_P2_SND_LEN 19
extern const uint8_t mus_p2_snd[];

#define MUS_P3_SND_LEN 19
extern const uint8_t mus_p3_snd[];

#define MUS_P4_SND_LEN 19
extern const uint8_t mus_p4_snd[];

#define MUS_P5_SND_LEN 19
extern const uint8_t mus_p5_snd[];

#define MUS_P6_SND_LEN 19
extern const uint8_t mus_p6_snd[];

#define MUS_P7_SND_LEN 19
extern const uint8_t mus_p7_snd[];

#define MUS_S21_SND_LEN 38
extern const uint8_t mus_s21_snd[];

#define MUS_P21_SND_LEN 19
extern const uint8_t mus_p21_snd[];

#define MUS_P22_SND_LEN 19
extern const uint8_t mus_p22_snd[];

#define MUS_P24_SND_LEN 19
extern const uint8_t mus_p24_snd[];

#define MUS_P26_SND_LEN 19
extern const uint8_t mus_p26_snd[];

#define MUS_P27_SND_LEN 38
extern const uint8_t mus_p27_snd[];

#define MUS_P28_SND_LEN 38
extern const uint8_t mus_p28_snd[];

#define MUS_P29_SND_LEN 38
extern const uint8_t mus_p29_snd[];

#define MUS_EP_SND_LEN 38
extern const uint8_t mus_ep_snd[];

#define MUS_EP2_SND_LEN 19
extern const uint8_t mus_ep2_snd[];

#define JUMP_SND_LEN 18
extern const uint8_t jump_snd[];

#define S_SND_LEN 38
extern const uint8_t s_snd[];

#define R1_SND_LEN 9
extern const uint8_t r1_snd[];

#define R2_SND_LEN 9
extern const uint8_t r2_snd[];

#define SHOT_SND_LEN 44
extern const uint8_t shot_snd[];

#define SHOOT_SND_LEN 27
extern const uint8_t shoot_snd[];

#define GET_KEY_SND_LEN 90
extern const uint8_t get_key_snd[];

#define HIT_WALL_SND_LEN 8
extern const uint8_t hit_wall_snd[];

#define WALK1_SND_LEN 3
extern const uint8_t walk1_snd[];

#define WALK2_SND_LEN 3
extern const uint8_t walk2_snd[];

#define MEDKIT_SND_LEN 69
extern const uint8_t medkit_snd[];

#define MELEE_SND_LEN 9
extern const uint8_t melee_snd[];

// API: non-blocking init/play
void sound_init(void);
void playSound(const uint8_t *snd, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif