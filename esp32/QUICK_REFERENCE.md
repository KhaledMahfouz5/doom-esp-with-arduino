# Quick Reference - ESP32 Doom Authentication System

## 🔐 Authentication Flow

```
User Power ON
     ↓
ESP32 UART Init (115200 baud)
     ↓
Display "SYSTEM LOCKED" ← AUTH scene
     ↓
Arduino Prompt: "Enter Password"
     ↓
User enters 8 digits on keypad
     ↓
Arduino validates: "12345678"
     ↓
✅ CORRECT → Arduino: "AUTH_OK" → ✅ GAME UNLOCKED
❌ WRONG → LCD: "Wrong Password" → Retry
```

---

## 📝 Key Files & Functions

| File                      | Function                               | Purpose                       |
| ------------------------- | -------------------------------------- | ----------------------------- |
| `auth.h/cpp`              | `auth_setup()`                         | Initialize UART @ 115200 baud |
| `auth.h/cpp`              | `auth_update()`                        | Check for "AUTH_OK" message   |
| `auth.h/cpp`              | `auth_is_authenticated()`              | Check if unlocked             |
| `auth.h/cpp`              | `auth_get_input()`                     | Get keypress from Arduino     |
| `input.cpp`               | `process_uart_input()`                 | Map keypresses to buttons     |
| `input.cpp`               | `uart_input_left/right/up/down/fire()` | Check button states           |
| `doom-nano-brutality.ino` | `loopAuth()`                           | Auth screen loop              |
| `constants.h`             | `AUTH = 0`                             | New scene ID                  |

---

## 🎮 Keypad Mapping

Arduino keypad → Game input:

```
      2              8              4              6
     ↑             ↓              ←              →
   UP           DOWN            LEFT           RIGHT

      5
   FIRE
   (Center)
```

When user presses these keys on Arduino keypad, the characters ('2','4','5','6','8') are sent via UART to ESP32.

---

## 🔌 UART Configuration

```
Baud Rate:  115200
Data Bits:  8
Stop Bits:  1
Parity:     None
Flow:       None

ESP32 Pins:
  RX: GPIO3 (default)
  TX: GPIO1 (default)

Connect to Arduino:
  ESP32 RX ← Arduino TX
  ESP32 TX → Arduino RX
  GND ← GND
```

---

## 📊 Authentication State Variables

```cpp
// In auth.cpp (global)
AuthState authState;        // Current state (LOCKED/AUTH/UNLOCKED)
uint8_t authenticated;      // Flag: 1 = unlocked, 0 = locked

// In input.cpp (static)
uint8_t uart_input_state;   // Current button state
unsigned long uart_input_timeout;  // Button timeout (100ms)
```

---

## 🔄 Game Execution Flow

```
setup()
  ├─ setupDisplay()
  ├─ input_setup()
  ├─ sound_init()
  └─ auth_setup()           ← Initialize UART

scene = AUTH                ← Start authentication

loop()
  └─ switch(scene)
      ├─ AUTH     → loopAuth()      (wait for password)
      ├─ INTRO    → loopIntro()     (menu)
      ├─ DIFF     → loopDiff()      (difficulty select)
      ├─ MUS      → loopMus()       (music select)
      ├─ MID      → loopMid()       (story screen)
      ├─ GAME_PLAY→ loopGamePlay()  (main game)
      └─ SCORE    → loopScore()     (results)
```

---

## 🛡️ Authentication Check Points

**1. In loopAuth():**

```cpp
if (auth_is_authenticated()) {
  // Show "ACCESS GRANTED"
  jumpTo(INTRO);
}
```

**2. In loopGamePlay():**

```cpp
auth_update();  // Check for new input from Arduino
// Can now receive button presses
```

**3. In input functions:**

```cpp
if (!auth_is_authenticated()) {
  return 0;  // No input while locked
}
```

---

## 📥 Arduino Message Format

**Authentication OK:**

```
"AUTH_OK"
```

**Game Input (one char per press):**

```
'2' = UP
'4' = LEFT
'5' = FIRE
'6' = RIGHT
'8' = DOWN
'*' = SPECIAL
```

---

## 🐛 Debug Tips

1. **Check authentication stuck?**
   - Verify UART is initialized: `auth_setup()` called
   - Check baud rate: Must be 115200
   - Monitor Serial: `Serial.println("AUTH_OK")` to test

2. **No game input?**
   - Check `auth_update()` called in loop
   - Verify `process_uart_input()` processes characters
   - Test keypad mappings: '2','4','5','6','8'

3. **Garbled text?**
   - Verify baud rate matches on both sides
   - Check UART wiring (RX/TX not reversed)
   - Add GND connection between boards

---

## 📌 Important Constants

```cpp
#define UART_BAUDRATE 115200           // auth.h
#define UART_INPUT_TIMEOUT 100         // input.cpp
#define PASSWORD "12345678"            // Arduino main.ino
#define AUTH 0                         // constants.h (new scene)
```

---

## 🚀 Quick Test

1. **Power on ESP32** → Should show "SYSTEM LOCKED" on OLED
2. **Power on Arduino** → Should show "Enter Password" on LCD
3. **Enter 12345678** → Should get "Access Granted"
4. **Game starts** → Use keypad to control
5. **Try wrong password** → Should show "Wrong Password" on Arduino

---

## 🔧 Adding New Input Commands

To add new input command from Arduino:

**1. Update `input.cpp` process_uart_input():**

```cpp
case '#':  // New command
  uart_input_state |= (1 << 6);  // Set new bit
  break;
```

**2. Add new checker function:**

```cpp
bool uart_input_special2() {
  return get_uart_input_bit(6);
}
```

**3. Update Arduino to send this character**
**4. Use in game:**

```cpp
if (uart_input_special2()) {
  // New action
}
```

---

## 📦 Dependencies

✅ No external libraries required!

Uses only:

- Arduino.h (built-in)
- Serial (built-in)
- SPI.h (optional, for future SD card)

---

## ✨ Features Implemented

- [x] UART communication @ 115200 baud
- [x] Authentication screen with animated feedback
- [x] Password validation via Arduino
- [x] Secure game unlock mechanism
- [x] UART input routing to game
- [x] Keypad character mapping
- [x] Button timeout handling
- [x] SD card interface (placeholder for expansion)
- [x] Complete documentation
- [x] Modular, extensible code

---

## 🎯 Next Steps

1. **Verify Hardware:**
   - Connect ESP32 UART to Arduino UART
   - Verify baud rates match
   - Test with Simple UART test

2. **Test Authentication:**
   - Try correct password: "12345678"
   - Try wrong password: verify reset
   - Check "AUTH_OK" message received

3. **Test Game Control:**
   - Use keypad to move in game
   - Verify all buttons work
   - Check timeout after release

4. **Deploy:**
   - Flash both boards
   - Test full system
   - Monitor UART for errors

---

## 📞 Common Issues

| Issue                    | Cause                       | Fix                        |
| ------------------------ | --------------------------- | -------------------------- |
| Stuck on "SYSTEM LOCKED" | Arduino not sending AUTH_OK | Check UART connection      |
| No game input            | auth_update() not called    | Add call in loopGamePlay() |
| Baud rate error          | Wrong baud rate             | Set both to 115200         |
| Garbage on screen        | UART noise                  | Check grounding            |
| Game controls reversed   | Input mapping wrong         | Update input.cpp           |

---

**Last Updated:** 2026-01-29  
**Status:** ✅ Ready for Testing  
**Author:** ESP32 Doom Integration Team
