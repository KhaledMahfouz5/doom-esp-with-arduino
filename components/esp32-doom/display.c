#include "display.h"
#include "constants.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_heap_caps.h"
#include <string.h>

#include "esp_lcd_ili9341.h"

#ifndef CONFIG_HW_INV_BL
#define CONFIG_HW_INV_BL 0
#endif

static const char *TAG = "DOOM_DISPLAY";

static esp_lcd_panel_handle_t s_panel = NULL;

// Frame buffer for Doom-ESP32 rendering
static uint16_t *framebuffer = NULL;

void setupDisplay() {
    ESP_LOGI(TAG, "Initializing display for Doom-ESP32-Brutailty");

    ESP_LOGI(TAG, "Initialize SPI bus");
    const spi_bus_config_t bus_config = ILI9341_PANEL_BUS_SPI_CONFIG(CONFIG_HW_LCD_CLK_GPIO, CONFIG_HW_LCD_MOSI_GPIO, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16_t));
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    const esp_lcd_panel_io_spi_config_t io_config = ILI9341_PANEL_IO_SPI_CONFIG(CONFIG_HW_LCD_CS_GPIO, CONFIG_HW_LCD_DC_GPIO, NULL, NULL);
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, &io_handle));

    ESP_LOGI(TAG, "Install ILI9341 panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = CONFIG_HW_LCD_RESET_GPIO,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9341(io_handle, &panel_config, &panel_handle));
    
    s_panel = panel_handle;
    
    ESP_ERROR_CHECK(esp_lcd_panel_reset(s_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(s_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(s_panel, true));

    if (CONFIG_HW_LCD_BL_GPIO >= 0) {
        gpio_config_t bl_cfg = {
            .pin_bit_mask = 1ULL << CONFIG_HW_LCD_BL_GPIO,
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&bl_cfg);
        gpio_set_level(CONFIG_HW_LCD_BL_GPIO, CONFIG_HW_INV_BL ? 0 : 1);
    }
    
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

// Flush the current framebuffer to the LCD via ESP-IDF esp_lcd
void display_flush(void) {
    if (!framebuffer) return;

    if (!s_panel) return;

    esp_lcd_panel_draw_bitmap(s_panel, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, framebuffer);
}

// Get framebuffer pointer for direct access (for optimized rendering)
uint16_t* display_get_framebuffer(void) {
    return framebuffer;
}
