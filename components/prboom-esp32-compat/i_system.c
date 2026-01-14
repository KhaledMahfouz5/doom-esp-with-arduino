/* المكاتب المحدثة لبيئة ESP-IDF الحديثة */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <fcntl.h>
#include <errno.h>

// المكاتب الخاصة بـ ESP32 المحدثة
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_partition.h"
#include "esp_flash.h"       // بديلة لـ esp_spi_flash.h القديمة
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "driver/gpio.h"

// مكاتب اللعبة
#include "doomtype.h"
#include "doomdef.h"
#include "i_system.h"
#include "lprintf.h"

static const char *TAG = "DOOM_SYSTEM";

// تعريفات الأرجل (تأكد من مطابقتها لبوردك)
#define PIN_NUM_MISO 2 
#define PIN_NUM_MOSI 13
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   15

static bool init_SD = false;

// دالة التوقيت المحدثة بدقة عالية
static unsigned long getMsTicks() {
    return (unsigned long)(esp_timer_get_time() / 1000);
}

int I_GetTime_RealTime (void)
{
    // TICRATE هو 35 في Doom الأصلية
    return (int)((esp_timer_get_time() * TICRATE) / 1000000);
}

// دالة تخصيص الذاكرة (Memory Allocation)
// Doom تستهلك الكثير من الذاكرة، لذا نستخدم الـ PSRAM إذا توفرت
void* I_Malloc(size_t size) {
    void* p = heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!p) {
        p = malloc(size); // محاولة التخصيص من الرام الداخلية كحل أخير
    }
    return p;
}

// تحديث إعدادات الـ SD Card لتتوافق مع ESP-IDF 5.x
void Init_SD()
{
    ESP_LOGI(TAG, "Initializing SD card...");

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t* card;
    
    // إعداد الـ SPI لبطاقة الذاكرة
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    // ESP-IDF 5.x يتطلب تهيئة الناقل أولاً
    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SDSPI_DEFAULT_DMA_CHAN);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus.");
        return;
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = SPI2_HOST;

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SPI2_HOST;

    ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount filesystem. Error: %s", esp_err_to_name(ret));
        return;
    }
    init_SD = true;
    ESP_LOGI(TAG, "SD Card mounted successfully.");
}

// دالة الخطأ المحدثة
void I_Error (char *error, ...)
{
    va_list argptr;
    va_start(argptr, error);
    vfprintf(stderr, error, argptr);
    va_end(argptr);
    fprintf(stderr, "\n");
    
    fflush(stderr);
    vTaskDelay(pdMS_TO_TICKS(5000));
    esp_restart(); // إعادة التشغيل بدلاً من exit(0)
}

// دالة قراءة الملفات المحدثة (أكثر استقراراً)
void I_Read(int ifd, void* vbuf, size_t sz)
{
    if (fread(vbuf, 1, sz, fds[ifd].file) != sz) {
        ESP_LOGE(TAG, "I_Read: Error reading %d bytes", sz);
        // لا تخرج فوراً، ربما يكون خطأ عابر
    }
}

// استبدال exit(0) بـ esp_restart() في كافة الوظائف
void I_Quit (void)
{
    ESP_LOGI(TAG, "Quitting Doom...");
    if (init_SD) {
        esp_vfs_fat_sdmmc_unmount("/sdcard", NULL);
    }
    esp_restart();
}
