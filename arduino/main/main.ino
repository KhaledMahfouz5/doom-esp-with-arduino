#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <avr/pgmspace.h>

/* =========================================================
   HARDWARE PIN DEFINITIONS (UNO / ATmega328P)
   ========================================================= */

/* ---- UART (ESP32) ----
   D0 = RX
   D1 = TX
   (Hardware Serial)
*/

/* ---- LCD (HD44780, 4-bit) ---- */
#define LCD_RS  7   // PD7
#define LCD_E   8   // PB0
#define LCD_D4  9   // PB1
#define LCD_D5  10  // PB2
#define LCD_D6  11  // PB3
#define LCD_D7  12  // PB4

/* ---- KEYPAD ---- */
#define KP_R1 A0   // PC0
#define KP_R2 A1   // PC1
#define KP_R3 A2   // PC2
#define KP_R4 A3   // PC3

#define KP_C1 A4   // PC4
#define KP_C2 A5   // PC5
#define KP_C3 2    // PD2
#define KP_C4 3    // PD3

/* =========================================================
   CONFIGURATION
   ========================================================= */

#define PASSWORD_LENGTH 8
#define UART_BAUDRATE   115200

/* =========================================================
   SYSTEM STATE MACHINE
   ========================================================= */

typedef enum {
    STATE_LOCKED,
    STATE_AUTHENTICATING,
    STATE_UNLOCKED
} SystemState;

static SystemState systemState = STATE_LOCKED;

/* =========================================================
   PASSWORD DATA
   ========================================================= */

const char PASSWORD[PASSWORD_LENGTH + 1] PROGMEM = "12345678";

static char inputBuffer[PASSWORD_LENGTH + 1];
static uint8_t inputIndex = 0;

/* =========================================================
   LCD INSTANCE
   ========================================================= */

LiquidCrystal lcd(
    LCD_RS,
    LCD_E,
    LCD_D4,
    LCD_D5,
    LCD_D6,
    LCD_D7
);

/* =========================================================
   KEYPAD INSTANCE
   ========================================================= */

const byte ROWS = 4;
const byte COLS = 4;

char keypadMap[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

byte rowPins[ROWS] = {KP_R1, KP_R2, KP_R3, KP_R4};
byte colPins[COLS] = {KP_C1, KP_C2, KP_C3, KP_C4};

Keypad keypad = Keypad(
    makeKeymap(keypadMap),
    rowPins,
    colPins,
    ROWS,
    COLS
);

/* =========================================================
   UART (ESP32) — GUARDED TX
   ========================================================= */

void uartSendKey(char key)
{
    if (systemState == STATE_UNLOCKED) {
        Serial.write(key);
    }
}

/* =========================================================
   AUTHENTICATION HELPERS
   ========================================================= */

bool checkPassword(void)
{
    for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
        if (inputBuffer[i] != pgm_read_byte(&PASSWORD[i])) {
            return false;
        }
    }
    return true;
}

void resetAuthentication(void)
{
    memset(inputBuffer, 0, sizeof(inputBuffer));
    inputIndex = 0;
    systemState = STATE_LOCKED;

    lcd.clear();
    lcd.print("Enter Password");
    lcd.setCursor(0, 1);
}

/* =========================================================
   KEYPAD INPUT HANDLER
   ========================================================= */

void handleKeypadInput(char key)
{
    if (systemState == STATE_UNLOCKED) {
        uartSendKey(key);
        return;
    }

    systemState = STATE_AUTHENTICATING;

    if (inputIndex < PASSWORD_LENGTH) {
        inputBuffer[inputIndex++] = key;
        lcd.print('*');
    }

    if (inputIndex == PASSWORD_LENGTH) {

        if (checkPassword()) {
            lcd.clear();
            lcd.print("Access Granted");

            systemState = STATE_UNLOCKED;
            Serial.println("AUTH_OK");

        } else {
            lcd.clear();
            lcd.print("Wrong Password");
            delay(1500);
            resetAuthentication();
        }
    }
}

/* =========================================================
   SETUP
   ========================================================= */

void setup(void)
{
    Serial.begin(UART_BAUDRATE);

    lcd.begin(16, 2);
    resetAuthentication();
}

/* =========================================================
   MAIN LOOP
   ========================================================= */

void loop(void)
{
    char key = keypad.getKey();

    if (key) {
        handleKeypadInput(key);
    }
}
