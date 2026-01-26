/* المكاتب المحدثة لبيئة ESP-IDF الحديثة */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

// المكاتب الخاصة بـ ESP32 المحدثة
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_partition.h"
#include "esp_flash.h"
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

#define MAXOPENFILES 32

static const char *TAG = "DOOM_SYSTEM";
static sdmmc_card_t* sd_card = NULL;

// يجب تعريف هيكل الملفات هنا لأن I_Read تحتاجه
typedef struct {
    FILE *file;
    int handle_type;  // optional
} FileDesc;

// الإشارة إلى المصفوفة المعرفة في مكان آخر
FileDesc fds[MAXOPENFILES];

// تعريفات الأرجل (تأكد من مطابقتها لبوردك)
#define PIN_NUM_MISO CONFIG_HW_LCD_MISO_GPIO
#define PIN_NUM_MOSI CONFIG_HW_LCD_MOSI_GPIO
#define PIN_NUM_CLK  CONFIG_HW_LCD_CLK_GPIO
#define PIN_NUM_CS   CONFIG_HW_SD_CS_GPIO

static bool init_SD = false;

// دالة التوقيت المحدثة
static unsigned long getMsTicks() {
    return (unsigned long)(esp_timer_get_time() / 1000);
}

int I_GetTime_RealTime (void)
{
    return (int)((esp_timer_get_time() * TICRATE) / 1000000);
}

// دالة تخصيص الذاكرة المحسنة لـ Doom
void* I_Malloc(size_t size) {
    void* p = heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!p) {
        p = heap_caps_malloc(size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    }
    return p;
}

void Init_SD()
{
    if (init_SD) return;

    ESP_LOGI(TAG, "Initializing SD card via SPI...");

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t* card_local; // local temporary pointer

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus.");
        return;
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = SPI2_HOST;

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SPI2_HOST;

    ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card_local);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SD Card. Error: %s", esp_err_to_name(ret));
        return;
    }

    // ✅ Store pointer globally for later unmount
    sd_card = card_local;

    init_SD = true;
    ESP_LOGI(TAG, "SD Card mounted successfully.");
}

// التصحيح 2: إضافة const لتطابق تعريف lprintf.h ومنع تعارض الأنواع
void I_Error (const char *error, ...)
{
    va_list argptr;
    va_start(argptr, error);
    vfprintf(stderr, error, argptr);
    va_end(argptr);
    fprintf(stderr, "\n");
    
    vTaskDelay(pdMS_TO_TICKS(5000));
    esp_restart();
}

void I_Read(int ifd, void* vbuf, size_t sz)
{
    if (fds[ifd].file == NULL) return;
    
    size_t readBytes = fread(vbuf, 1, sz, fds[ifd].file);
    if (readBytes != sz) {
        ESP_LOGE(TAG, "I_Read: Error! Requested %d bytes, got %d", (int)sz, (int)readBytes);
    }
}

void I_Quit(void)
{
    ESP_LOGI(TAG, "Quitting Doom...");
    if (init_SD && sd_card) {
        esp_vfs_fat_sdcard_unmount("/sdcard", sd_card);
        sd_card = NULL;
    }
    esp_restart();
}

void *I_Mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    (void)addr;
    (void)prot;
    (void)flags;

    void *buf = malloc(length);
    if (!buf)
        return NULL;

    // Seek to lump position
    if (lseek(fd, offset, SEEK_SET) < 0) {
        free(buf);
        return NULL;
    }

    // Read lump data
    if (read(fd, buf, length) != (ssize_t)length) {
        free(buf);
        return NULL;
    }

    return buf;
}
int I_Lseek(int fd, off_t offset, int whence)
{
    return (int)lseek(fd, offset, whence);
}

int I_Filelength(int fd)
{
    struct stat st;

    if (fstat(fd, &st) < 0)
        return -1;

    return (int)st.st_size;
}

void I_Close(int fd) { close(fd); }


int I_Open(const char *path, int flags)
{
    return open(path, flags);
}

int I_Munmap(void *addr, size_t length)
{
    (void)length;
    free(addr);
    return 0;
}


int I_GetTime_SaveMS(void)
{
    // esp_timer_get_time() returns microseconds
    return (int)(esp_timer_get_time() / 1000);
}

void I_uSleep(unsigned long usecs)
{
    // convert microseconds to ticks
    TickType_t ticks = (usecs + 999) / 1000; // ceil to nearest ms
    if (ticks > 0) {
        vTaskDelay(ticks);
    }
}

char *I_FindFile(const char *wfname, const char *ext)
{
    char buf[128];

    if (ext && ext[0] != '\0')
        snprintf(buf, sizeof(buf), "%s.%s", wfname, ext);
    else
        snprintf(buf, sizeof(buf), "%s", wfname);

    char *ret = malloc(strlen(buf) + 1);
    if (!ret)
        return NULL;

    strcpy(ret, buf);
    return ret;
}

fixed_t I_GetTimeFrac(void)
{
    // esp_timer_get_time() returns microseconds
    int usecs = (int)(esp_timer_get_time() % (1000000 / 35));

    // convert fraction of tic to 16.16 fixed point
    return (usecs << 16) / (1000000 / 35);
}
 
const char* I_DoomExeDir(void)
{
    return "."; // or "/spiffs" if using SPIFFS
}

int I_GetRandomTimeSeed(void)
{
    return (int)esp_timer_get_time();
}

void I_EndDisplay(void)
{
    // Optional: yield to FreeRTOS or sleep a bit
    taskYIELD();
}

void I_UpdateNoBlit(void)
{
    taskYIELD(); // give other tasks CPU time
}
