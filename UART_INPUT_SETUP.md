# UART Input Setup for ESP32 Doom

This document explains how to connect an Arduino UNO to the ESP32 Doom project for keypad input.

## Hardware Connections

### Arduino UNO to ESP32 Wiring:

```
Arduino UNO          ESP32
-----------          -----
D0 (RX)    <--->   GPIO 16 (TX)
D1 (TX)    <--->   GPIO 17 (RX)
GND        <--->   GND
```

### Keypad Wiring (Arduino UNO):

```
Arduino UNO    Keypad Matrix
------------    -------------
A0 (PC0)    <---> Row 1
A1 (PC1)    <---> Row 2
A2 (PC2)    <---> Row 3
A3 (PC3)    <---> Row 4

A4 (PC4)    <---> Col 1
A5 (PC5)    <---> Col 2
D2 (PD2)    <---> Col 3
D3 (PD3)    <---> Col 4
```

## Control Mapping

The Arduino sketch sends single characters that map to Doom controls:

| Keypad | Character | Doom Control |
| ------ | --------- | ------------ |
| 2      | '2'       | UP           |
| 8      | '8'       | DOWN         |
| 4      | '4'       | LEFT         |
| 6      | '6'       | RIGHT        |
| 5      | '5'       | FIRE         |

### Additional Keys (for menu navigation):

| Keypad | Character | Purpose      |
| ------ | --------- | ------------ |
| 1      | '1'       | Menu Up      |
| 3      | '3'       | Menu Down    |
| 7      | '7'       | Menu Left    |
| 9      | '9'       | Menu Right   |
| 0      | '0'       | Select/Enter |
| \*     | '\*'      | Back/Exit    |
| #      | '#'       | Start/Pause  |

## Arduino Sketch

The Arduino sketch is located at `arduino/main/main.ino` and includes:

1. **Password Authentication**: Default password is "12345678"
2. **Keypad Matrix**: 4x4 matrix scanning
3. **UART Communication**: Sends single characters at 115200 baud
4. **LCD Display**: Shows current status and pressed keys

## Usage Instructions

### 1. Setup Hardware:

- Connect Arduino UNO to ESP32 as shown above
- Connect the 4x4 keypad to Arduino UNO
- Power both devices

### 2. Upload Arduino Sketch:

- Open `arduino/main/main.ino` in Arduino IDE
- Select "Arduino UNO" as board
- Upload the sketch

### 3. Flash ESP32:

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

### 4. Authenticate:

- On Arduino LCD, enter password "12345678"
- System will show "Access Granted" when successful

### 5. Play Doom:

- Use keypad controls to navigate menus and play
- 2/8/4/6/5 for movement and shooting
- Other keys for menu navigation

## Troubleshooting

### No Input Detected:

1. Check UART connections (TX/RX cross-connected)
2. Verify Arduino is powered and running
3. Check that password is entered correctly
4. Monitor ESP32 logs for UART activity

### Display Issues:

1. Verify ILI9341 display connections
2. Check display power supply
3. Monitor ESP32 logs for display initialization

### Keypad Issues:

1. Check keypad matrix wiring
2. Verify all row/column connections
3. Check for loose connections

## Technical Details

### UART Configuration:

- **Baud Rate**: 115200
- **Data Bits**: 8
- **Stop Bits**: 1
- **Parity**: None
- **Flow Control**: None

### Communication Protocol:

- Arduino sends single characters immediately when keys are pressed
- ESP32 reads characters in real-time
- No command termination needed (single char protocol)

### ESP32 Input Processing:

- Primary input: UART from Arduino
- Fallback input: GPIO pins (if configured)
- Input state maintained in `InputState` structure
- Real-time processing with no delays

## Future Enhancements

1. **Multi-key Support**: Modify Arduino to send combined commands
2. **Analog Input**: Add potentiometer for smooth movement
3. **Wireless**: Replace UART with Bluetooth/WiFi
4. **Additional Buttons**: Add more controls for advanced gameplay
