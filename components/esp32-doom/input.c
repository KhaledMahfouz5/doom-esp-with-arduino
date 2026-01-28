#include "input.h"

#include "constants.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "DOOM_INPUT";

InputState input = {0};

// UART configuration for Arduino UNO communication
#define UART_NUM UART_NUM_1
#define UART_BAUD_RATE 115200
#define UART_TX_PIN (GPIO_NUM_17)  // ESP32 TX to Arduino RX
#define UART_RX_PIN (GPIO_NUM_16)  // ESP32 RX from Arduino TX
#define UART_BUF_SIZE 1024

static inline void add_pin_mask(uint64_t *mask, int pin)
{
    // GPIO pins not used - UART input only
}

// Parse UART command from Arduino UNO
// Original Arduino sketch sends single characters: '1','2','3','4','5','6','7','8','9','0','*','#','A','B','C','D'
// Map them to Doom controls:
// '2' = UP, '8' = DOWN, '4' = LEFT, '6' = RIGHT, '5' = FIRE
// '1','3','7','9','0','*','#' = other keys (can be used for menu navigation)
// 'A','B','C','D' = special keys
static void parse_uart_command(const char* cmd)
{
    // Reset all inputs first
    input.left = false;
    input.right = false;
    input.up = false;
    input.down = false;
    input.fire = false;
    
    // Parse each character in the command
    for (int i = 0; cmd[i] != '\0'; i++) {
        switch (cmd[i]) {
            // Main Doom controls
            case '2':  // UP (from keypad 2)
                input.up = true;
                break;
            case '8':  // DOWN (from keypad 8)
                input.down = true;
                break;
            case '4':  // LEFT (from keypad 4)
                input.left = true;
                break;
            case '6':  // RIGHT (from keypad 6)
                input.right = true;
                break;
            case '5':  // FIRE (from keypad 5)
                input.fire = true;
                break;
            
            // Additional keys for menu navigation
            case '1':  // Could be used for menu up/previous
            case '3':  // Could be used for menu down/next
            case '7':  // Could be used for menu left
            case '9':  // Could be used for menu right
            case '0':  // Could be used for select/enter
            case '*':  // Could be used for back/exit
            case '#':  // Could be used for start/pause
                // For now, just log these keys
                ESP_LOGD(TAG, "Menu key pressed: %c", cmd[i]);
                break;
            
            // Special keys from original sketch
            case 'A':
            case 'B':
            case 'C':
            case 'D':
                ESP_LOGD(TAG, "Special key pressed: %c", cmd[i]);
                break;
                
            case ' ':
            case '\n':
            case '\r':
                // Skip whitespace
                break;
            default:
                ESP_LOGD(TAG, "Unknown UART command: %c", cmd[i]);
                break;
        }
    }
    
    ESP_LOGD(TAG, "UART input: L=%d R=%d U=%d D=%d F=%d", 
             input.left, input.right, input.up, input.down, input.fire);
}

static void read_uart_input(void)
{
    uint8_t data;
    int length = uart_read_bytes(UART_NUM, &data, 1, 0); // Non-blocking read of single byte
    
    if (length > 0) {
        char c = (char)data;
        
        // Process single character immediately (Arduino sketch sends single chars)
        char cmd[2] = {c, '\0'};
        parse_uart_command(cmd);
        
        ESP_LOGD(TAG, "Received UART char: %c", c);
    }
}

void input_setup()
{
    // Setup UART for Arduino UNO communication
    ESP_LOGI(TAG, "Setting up UART input from Arduino UNO");
    
    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    esp_err_t err = uart_driver_install(UART_NUM, UART_BUF_SIZE * 2, 0, 0, NULL, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "uart_driver_install failed: %d", (int)err);
        return;
    }
    
    err = uart_param_config(UART_NUM, &uart_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "uart_param_config failed: %d", (int)err);
        return;
    }
    
    err = uart_set_pin(UART_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "uart_set_pin failed: %d", (int)err);
        return;
    }
    
    ESP_LOGI(TAG, "UART configured: TX=%d, RX=%d, Baud=%d", UART_TX_PIN, UART_RX_PIN, UART_BAUD_RATE);
    ESP_LOGI(TAG, "Using UART input from Arduino UNO only");

    input_update();
}

void input_update()
{
    // Store previous states
    input.left_prev = input.left;
    input.right_prev = input.right;
    input.up_prev = input.up;
    input.down_prev = input.down;
    input.fire_prev = input.fire;

    // Read from UART (primary and only input method)
    read_uart_input();
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
