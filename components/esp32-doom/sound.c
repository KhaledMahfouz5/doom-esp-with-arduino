#include "sound.h"
#include "esp_log.h"

static const char *TAG = "DOOM_SOUND";

// Minimal non-blocking stub implementation.
// Audio data is kept as const in flash; playback is a no-op for now.
// Later: replace with PWM/I2S task that never blocks the game loop.

void sound_init(void)
{
    ESP_LOGI(TAG, "Sound stub: no-op init");
}

void playSound(const uint8_t *snd, uint8_t len)
{
    // Non-blocking stub: ignore parameters.
    // In a future implementation, queue the sound to a dedicated audio task.
    (void)snd;
    (void)len;
    // ESP_LOGD(TAG, "Sound stub: playSound %p len=%d (no-op)", snd, len);
}
