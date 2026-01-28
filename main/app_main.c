// Copyright 2016-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_attr.h"

#include "rom/cache.h"
#include "rom/ets_sys.h"
#include "rom/crc.h"
#include "uart_control.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/efuse_reg.h"
#include "soc/rtc_cntl_reg.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdlib.h>
#include "esp_err.h"
#include "esp32-doom.h"

void doomEngineTask(void *arg) {
    TickType_t last = xTaskGetTickCount();

    for (;;) {
        switch (current_scene) {
            case SCENE_INTRO:
                loopIntroTick();
                break;

            case SCENE_GAME:
                loopGameTick();
                break;

            case SCENE_SCORE:
                loopScoreTick();
                break;

            case SCENE_MID:
                loopMidTick();
                break;
        }

        vTaskDelayUntil(&last, pdMS_TO_TICKS(DOOM_TICK_MS));
    }
}

void app_main() {
	xTaskCreatePinnedToCore(&doomEngineTask, "doomEngineTask", 18000, NULL, 5, NULL, 0);
}
