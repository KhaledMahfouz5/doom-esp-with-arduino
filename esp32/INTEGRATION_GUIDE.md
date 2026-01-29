## ESP32 Doom Game - Integration Guide

This document describes the new authentication and UART communication system added to support the complete game flow with Arduino password entry.

### System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        ESP32 (Doom Game)                        │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │           AUTH Screen (loopAuth)                        │   │
│  │  - Displays "SYSTEM LOCKED"                             │   │
│  │  - Waits for AUTH_OK from Arduino                       │   │
│  │  - Shows animated waiting dots                          │   │
│  └─────────────────────────────────────────────────────────┘   │
│                              ↑                                   │
│                              │ AUTH_OK                           │
│                              │ (via UART)                        │
│                              │                                   │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │           Game Scenes (INTRO, DIFF, MUS, etc)          │   │
│  │  - After authentication                                 │   │
│  │  - Receives input from Arduino keypad                   │   │
│  └─────────────────────────────────────────────────────────┘   │
│                              ↑                                   │
│                              │ Button presses (chars)           │
│                              │ (via UART Serial)                │
└─────────────────────────────────────────────────────────────────┘
         ↑                                          ↓
         │ UART Serial (115200 baud)               │
         │ (GPIO0=RX, GPIO1=TX)                    │
         │                                          │
┌─────────────────────────────────────────────────────────────────┐
│                    Arduino (ATmega328P)                          │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │              Keypad (4x4 Matrix)                         │   │
│  │  - Reads user input                                     │   │
│  │  - Handles authentication                               │   │
│  │  - Sends "AUTH_OK" when password matches               │   │
│  │  - Forwards button presses to ESP32                     │   │
│  │  - Displays feedback on LCD (16x2)                      │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

### New Files Added

#### 1. **auth.h** and **auth.cpp**

- Handles UART communication initialization and authentication state
- Functions:
  - `auth_setup()` - Initialize UART at 115200 baud
  - `auth_update()` - Check for AUTH_OK message from Arduino
  - `auth_is_authenticated()` - Check authentication status
  - `auth_get_input()` - Get keyboard input from Arduino
  - `auth_send_command()` - Send commands to Arduino (future use)

**Authentication States:**

- `AUTH_STATE_LOCKED` - System locked, waiting for password
- `AUTH_STATE_AUTHENTICATING` - Password being entered
- `AUTH_STATE_UNLOCKED` - Password accepted, game unlocked

#### 2. **sdcard.h** and **sdcard.cpp**

- SD card interface for game data storage
- Functions:
  - `sd_init()` - Initialize SPI for SD card
  - `sd_is_mounted()` - Check if SD card is ready
  - `sd_file_exists()` - Check if game file exists
  - `sd_get_file_size()` - Get size of game file
  - `sd_read_file()` - Read game data from SD card

**Pin Configuration (from README):**

- GPIO23 - MOSI
- GPIO19 - MISO
- GPIO18 - SCLK
- GPIO13 - CS (Chip Select)

### Modified Files

#### 1. **constants.h**

- Added `AUTH = 0` scene at the beginning
- Shifted all other scene IDs by +1:
  - INTRO → 1
  - GAME_PLAY → 2
  - DIFF → 3
  - MUS → 4
  - MID → 5
  - SCORE → 6

#### 2. **doom-nano-brutality.ino**

- Added `#include "auth.h"`
- Updated `setup()` to:
  - Call `auth_setup()` to initialize UART
  - Set initial scene to `AUTH`
- Added `loopAuth()` function - New authentication screen with:
  - "SYSTEM LOCKED" display
  - Animated waiting dots
  - Waits for AUTH_OK from Arduino
  - Shows "ACCESS GRANTED" message
  - Transitions to INTRO when authenticated
- Updated `loopGamePlay()` to:
  - Call `auth_update()` each frame to monitor Arduino
  - Allow reception of input commands from Arduino
- Updated `loop()` switch statement to handle new AUTH scene
- Set initial scene to `AUTH` instead of `INTRO`

#### 3. **input.cpp**

- Added UART input handling system
- New functions:
  - `process_uart_input()` - Process incoming UART data from Arduino
  - `uart_input_left()` - Check UART left button
  - `uart_input_right()` - Check UART right button
  - `uart_input_up()` - Check UART up button
  - `uart_input_down()` - Check UART down button
  - `uart_input_fire()` - Check UART fire button

**Keypad Mapping (from Arduino):**

- '2' = UP arrow
- '4' = LEFT arrow
- '5' = FIRE button
- '6' = RIGHT arrow
- '8' = DOWN arrow
- '\*' = Special button

**Input Timeout:** 100ms - button state resets if no new input received

#### 4. **input.h**

- No changes needed (declarations for UART functions already in .cpp)

### Game Flow

1. **Boot (ESP32)**
   - ESP32 initializes UART at 115200 baud
   - Sets scene to AUTH
   - Waits for authentication

2. **Password Entry (Arduino)**
   - User enters 8-character password on 4x4 keypad
   - Arduino displays on 16x2 LCD:
     - "Enter Password" prompt
     - Dots (\*) for each character entered
   - Arduino validates password against hardcoded string: "12345678"

3. **Authentication (Both)**
   - If password correct:
     - Arduino sends "AUTH_OK" via UART to ESP32
     - ESP32 receives message and sets authenticated flag
     - ESP32 shows "ACCESS GRANTED" message briefly
     - ESP32 transitions to INTRO scene
   - If password wrong:
     - Arduino shows "Wrong Password" message
     - LCD resets for new attempt
     - User can retry

4. **Game Control (After Authentication)**
   - Arduino becomes keypad gateway
   - User presses keypad buttons (2,4,5,6,8)
   - Arduino sends corresponding characters via UART
   - ESP32 processes input and updates game state
   - Arduino LCD shows current key being pressed (for feedback)

5. **Game Execution**
   - ESP32 runs full Doom game
   - Receives input from Arduino keypad
   - Displays game on OLED (128x64)
   - Outputs sound via speaker

### UART Protocol

**Baud Rate:** 115200
**Data:** 8 bits, 1 stop bit, no parity
**Flow Control:** None
**Connector:** USB or direct serial (depends on ESP32 board)

**Messages:**

- Arduino → ESP32: "AUTH_OK" (after successful password)
- Arduino → ESP32: Single character keypresses (2,4,5,6,8,\*)
- ESP32 → Arduino: (Reserved for future commands)

### Usage Notes

1. **First Start:**
   - Power on system
   - ESP32 OLED shows "SYSTEM LOCKED" with waiting animation
   - Arduino LCD shows "Enter Password"

2. **Entering Password:**
   - Use Arduino keypad to enter 8 digits
   - Each digit shows as '\*' on Arduino LCD
   - Backspace/delete not available (must be correct digits)

3. **Game Control:**
   - Arrow keys (2/4/6/8) for movement
   - Center key (5) for firing
   - - for special actions (jump, etc.)

4. **To Exit Authentication Loop:**
   - Wait 60 seconds (will reset prompt)
   - Or provide correct password

### Technical Details

**Authentication Variables (global in auth.cpp):**

```cpp
AuthState authState;  // Current authentication state
uint8_t authenticated; // Flag: 1 if authenticated, 0 if locked
```

**Input Buffer (static in auth.cpp):**

```cpp
char uartBuffer[64];   // Buffer for incoming UART data
uint8_t uartIndex;     // Current position in buffer
```

**UART Input State (static in input.cpp):**

```cpp
uint8_t uart_input_state;    // Bitfield for button states
unsigned long uart_input_timeout; // Timestamp for timeout
```

### Compilation Notes

- Ensure Arduino core for ESP32 is installed
- SPI library needed for potential SD card support
- Serial library for UART communication
- No additional external libraries required for basic functionality

### Future Enhancements

1. Full SD card integration for game data
2. Save game state on SD card
3. Multiple game levels/maps
4. Settings persistence
5. High score storage
6. WiFi update capability

### Troubleshooting

**"SYSTEM LOCKED" never changes:**

- Check Arduino sends "AUTH_OK" after correct password
- Verify UART baud rate (115200)
- Check ESP32 RX/TX pin connections

**No input from keypad during game:**

- Verify password was entered correctly
- Check Arduino continues sending key codes after AUTH_OK
- Verify UART connection is stable

**Garbled characters on display:**

- Check baud rate setting (115200)
- Look for electrical noise on UART lines
- Verify proper grounding

### Arduino-side Code Reference

The Arduino firmware (in `arduino/main/main.ino`) provides:

- 4x4 keypad matrix interface
- 16x2 LCD display driver
- Password validation logic
- UART gateway to ESP32
- User feedback on LCD

See `arduino/main/main.ino` for the complete Arduino implementation.
