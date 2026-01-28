#ifndef _esp32_doom_h
#define _esp32_doom_h

#ifdef __cplusplus
extern "C" {
#endif

// ESP-IDF entrypoints for Doom-Nano-Brutality
void doom_init(void);
void doom_game_tick(void);

#ifdef __cplusplus
}
#endif

#endif
