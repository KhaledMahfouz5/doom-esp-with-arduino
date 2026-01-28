#ifndef _sdcard_h
#define _sdcard_h

#include <Arduino.h>

// SD Card Configuration - Based on ESP32 pins from README
#define SD_CS_PIN           13    // Chip Select
#define SD_MOSI_PIN         23   // Master Out Slave In
#define SD_MISO_PIN         19   // Master In Slave Out
#define SD_SCLK_PIN         18   // Serial Clock

// SD Card file paths
#define GAME_DATA_DIR       "/games/"
#define GAME_E1M1_FILE      "/games/E1M1.dat"
#define GAME_E1M2_FILE      "/games/E1M2.dat"

// Function declarations
void sd_init();
bool sd_is_mounted();
bool sd_file_exists(const char* filepath);
uint32_t sd_get_file_size(const char* filepath);
bool sd_read_file(const char* filepath, uint8_t* buffer, uint32_t size);

#endif
