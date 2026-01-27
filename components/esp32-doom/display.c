#include "display.h"
#include "constants.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_heap_caps.h"
 #include <string.h>

static const char *TAG = "DOOM_DISPLAY";

// External SPI LCD functions from prboom-esp32-compat
extern void spi_lcd_init(void);
extern void spi_lcd_send(const uint16_t *buffer);

// Frame buffer for Doom-ESP32 rendering
static uint16_t *framebuffer = NULL;

void setupDisplay() {
    ESP_LOGI(TAG, "Initializing display for Doom-ESP32-Brutailty");
    
    // Initialize the existing SPI LCD system
    spi_lcd_init();
    
    // Allocate framebuffer
    size_t buffer_size = SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16_t);
    framebuffer = (uint16_t*)heap_caps_malloc(buffer_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    
    if (!framebuffer) {
        ESP_LOGE(TAG, "Failed to allocate framebuffer!");
        // Try with any available memory
        framebuffer = (uint16_t*)heap_caps_malloc(buffer_size, MALLOC_CAP_DEFAULT);
    }
    
    if (!framebuffer) {
        ESP_LOGE(TAG, "Cannot allocate framebuffer!");
        return;
    }
    
    ESP_LOGI(TAG, "Framebuffer allocated: %d bytes", buffer_size);
    clearDisplay();
}

void clearDisplay() {
    if (!framebuffer) return;
    
    memset(framebuffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16_t));
}

void drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (!framebuffer) return;
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    
    framebuffer[y * SCREEN_WIDTH + x] = color;
}

void drawVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    if (!framebuffer) return;
    if (x < 0 || x >= SCREEN_WIDTH) return;
    
    for (int16_t i = 0; i < h; i++) {
        int16_t py = y + i;
        if (py >= 0 && py < SCREEN_HEIGHT) {
            framebuffer[py * SCREEN_WIDTH + x] = color;
        }
    }
}

void drawHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    if (!framebuffer) return;
    if (y < 0 || y >= SCREEN_HEIGHT) return;
    
    for (int16_t i = 0; i < w; i++) {
        int16_t px = x + i;
        if (px >= 0 && px < SCREEN_WIDTH) {
            framebuffer[y * SCREEN_WIDTH + px] = color;
        }
    }
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    drawHLine(x, y, w, color);
    drawHLine(x, y + h - 1, w, color);
    drawVLine(x, y, h, color);
    drawVLine(x + w - 1, y, h, color);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (!framebuffer) return;
    
    for (int16_t j = 0; j < h; j++) {
        int16_t py = y + j;
        if (py >= 0 && py < SCREEN_HEIGHT) {
            for (int16_t i = 0; i < w; i++) {
                int16_t px = x + i;
                if (px >= 0 && px < SCREEN_WIDTH) {
                    framebuffer[py * SCREEN_WIDTH + px] = color;
                }
            }
        }
    }
}

void display() {
    if (!framebuffer) return;
    
    // Send the framebuffer to the LCD using the existing SPI LCD system
    spi_lcd_send(framebuffer);
}

// Get framebuffer pointer for direct access (for optimized rendering)
uint16_t* getFramebuffer() {
    return framebuffer;
}
