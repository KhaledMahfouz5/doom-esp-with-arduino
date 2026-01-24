#include "config.h"
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "doomstat.h"
#include "doomdef.h"
#include "doomtype.h"
#include "v_video.h"
#include "r_draw.h"
#include "d_main.h"
#include "d_event.h"
#include "i_video.h"
#include "z_zone.h"
#include "w_wad.h"
#include "st_stuff.h"
#include "lprintf.h"

// مكاتب ESP-IDF الحديثة
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "spi_lcd.h" // تأكد أن هذا الملف محدث ليدعم SPI Bus الموحد

static const char *TAG = "DOOM_VIDEO";

int use_fullscreen = 0;
int use_doublebuffer = 0;

// لوحة الألوان المحسنة
int16_t lcdpal[256];

void I_StartTic(void) {}

void I_ShutdownGraphics(void) {}

void I_StartFrame(void) {}

int I_StartDisplay(void) {
    // الانتظار حتى انتهاء آخر عملية نقل بيانات للشاشة (Non-blocking DMA)
    spi_lcd_wait_finish();
    return true;
}

// دالة تحويل لوحة ألوان Doom إلى تنسيق RGB565 الخاص بشاشات SPI
void I_SetPalette(int pal) {
    int pplump = W_GetNumForName("PLAYPAL");
    const byte *palette = W_CacheLumpNum(pplump);
    palette += pal * (3 * 256);

    for (int i = 0; i < 256; i++) {
        // تحويل 8-bit RGB إلى 16-bit RGB565 مع عكس الـ Bytes لتناسب الـ SPI
        uint16_t r = (palette[0] >> 3) << 11;
        uint16_t g = (palette[1] >> 2) << 5;
        uint16_t b = (palette[2] >> 3);
        uint16_t color = r | g | b;
        
        // تحويل Big Endian لأن أغلب شاشات SPI تستقبله هكذا
        lcdpal[i] = (color << 8) | (color >> 8);
        palette += 3;
    }
    W_UnlockLumpNum(pplump);
}

// دالة إنهاء تحديث الإطار وإرساله للشاشة
void I_FinishUpdate(void) {
    // الحصول على بيانات الصورة (التي هي عبارة عن Indices للوحة الألوان)
    uint16_t *scr = (uint16_t *)screens[0].data;

    // إرسال البيانات إلى الدرايفر المحدث
    // ملاحظة: Doom ترسم بـ 8-bit، والدرايفر يجب أن يحولها لـ 16-bit باستخدام lcdpal
    spi_lcd_send(scr);
}

void I_PreInitGraphics(void) {
    ESP_LOGI(TAG, "Pre-initializing graphics...");
    
    // تخصيص ذاكرة الشاشة في الـ Internal RAM لسرعة قصوى إذا أمكن
    // أو في الـ PSRAM إذا كانت الذاكرة الداخلية لا تكفي
    size_t sz = SCREENWIDTH * SCREENHEIGHT;
    screens[0].data = heap_caps_malloc(sz, MALLOC_CAP_8BIT);
    
    if (!screens[0].data) {
    ESP_LOGW(TAG, "Internal RAM failed, trying PSRAM...");
    screens[0].data = heap_caps_malloc(sz, MALLOC_CAP_SPIRAM);
    }

    if (!screens[0].data) {
    ESP_LOGE(TAG, "Failed to allocate screen buffer!");
    abort(); // Doom cannot run without framebuffer
    }
}

void I_SetRes(void) {
    for (int i = 0; i < 3; i++) {
        screens[i].width = SCREENWIDTH;
        screens[i].height = SCREENHEIGHT;
        screens[i].byte_pitch = SCREENWIDTH; // Pitch لـ 8-bit هو نفس العرض
        screens[i].short_pitch = SCREENWIDTH / 2;
        screens[i].int_pitch = SCREENWIDTH / 4;
    }

    // إعداد شاشة الـ Status Bar
    screens[4].width = SCREENWIDTH;
    screens[4].height = (ST_SCALED_HEIGHT + 1);
    screens[4].byte_pitch = SCREENWIDTH;

    ESP_LOGI(TAG, "Resolution set to %dx%d", SCREENWIDTH, SCREENHEIGHT);
}

void I_InitGraphics(void) {
    static int firsttime = 1;
    if (firsttime) {
        firsttime = 0;
        ESP_LOGI(TAG, "Initializing Graphics System...");
        
        // تهيئة الشاشة هاردوير (يجب أن تستخدم نفس الـ SPI Bus المفتوح في i_system.c)
        spi_lcd_init(); 
        
        I_UpdateVideoMode();
    }
}

void I_UpdateVideoMode(void) {
    ESP_LOGI(TAG, "Setting Video Mode: 8-bit Palette Mode");
    
    V_InitMode(VID_MODE8); 
    I_SetRes();
    R_InitBuffer(SCREENWIDTH, SCREENHEIGHT);
}
