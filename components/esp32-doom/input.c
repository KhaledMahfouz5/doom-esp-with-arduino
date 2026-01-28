#include "input.h"

#include "constants.h"

#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "DOOM_INPUT";

InputState input = {0};

static inline void add_pin_mask(uint64_t *mask, int pin)
{
    if (pin < 0 || pin >= 64) {
        return;
    }
    *mask |= (1ULL << (uint64_t)pin);
}

static bool read_button(int pin)
{
    if (pin < 0) {
        return false;
    }

    return gpio_get_level((gpio_num_t)pin) == 0;
}

void input_setup()
{
    uint64_t pin_mask = 0;
    add_pin_mask(&pin_mask, K_LEFT);
    add_pin_mask(&pin_mask, K_RIGHT);
    add_pin_mask(&pin_mask, K_UP);
    add_pin_mask(&pin_mask, K_DOWN);
    add_pin_mask(&pin_mask, K_FIRE);

    if (pin_mask == 0) {
        ESP_LOGW(TAG, "No input GPIOs configured");
        return;
    }

    const gpio_config_t cfg = {
        .pin_bit_mask = pin_mask,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    esp_err_t err = gpio_config(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "gpio_config failed: %d", (int)err);
    }

    input_update();
}

void input_update()
{
    input.left_prev = input.left;
    input.right_prev = input.right;
    input.up_prev = input.up;
    input.down_prev = input.down;
    input.fire_prev = input.fire;

    input.left = read_button(K_LEFT);
    input.right = read_button(K_RIGHT);
    input.up = read_button(K_UP);
    input.down = read_button(K_DOWN);
    input.fire = read_button(K_FIRE);
}

bool input_left()
{
    return input.left;
}

bool input_right()
{
    return input.right;
}

bool input_up()
{
    return input.up;
}

bool input_down()
{
    return input.down;
}

bool input_fire()
{
    return input.fire;
}
