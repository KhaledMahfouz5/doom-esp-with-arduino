#ifndef _constants_h
#define _constants_h

#include "game_constants.h"
#include "sdkconfig.h"

// SNES Controller (optional, Arduino placeholders)
// #define SNES_CONTROLLER
#define DATA_CLOCK   11
#define DATA_LATCH   12
#define DATA_SERIAL  13

// Sound pin (Arduino placeholder)
#define SOUND_PIN   9

// Display resolution (Kconfig-driven; defaults are Arduino SSD1306 values)
#ifndef CONFIG_HW_SCREEN_WIDTH
#define SCREEN_WIDTH     128
#else
#define SCREEN_WIDTH     CONFIG_HW_SCREEN_WIDTH
#endif

#ifndef CONFIG_HW_SCREEN_HEIGHT
#define SCREEN_HEIGHT    64
#else
#define SCREEN_HEIGHT    CONFIG_HW_SCREEN_HEIGHT
#endif

#define HALF_WIDTH       (SCREEN_WIDTH / 2)
#define HALF_HEIGHT      (SCREEN_HEIGHT / 2)
#define RENDER_HEIGHT    (SCREEN_HEIGHT - 8)  // Reserve space for HUD

// Derived from rendering constants
#define ZBUFFER_SIZE    (SCREEN_WIDTH / Z_RES_DIVIDER + 4)

// Legacy GFX flag (kept for compatibility; can be removed in cleanup)
#define OPTIMIZE_SSD1306

#endif
