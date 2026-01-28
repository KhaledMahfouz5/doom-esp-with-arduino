#include <Arduino.h>
#include "auth.h"

// Global authentication state
AuthState authState = AUTH_STATE_LOCKED;
uint8_t authenticated = 0;

// UART Buffer
static char uartBuffer[UART_MAX_BUFFER];
static uint8_t uartIndex = 0;
static unsigned long lastUartCheckTime = 0;

/**
 * Initialize UART communication with Arduino
 * Baud rate: 115200 (as per Arduino main.ino)
 */
void auth_setup() {
  // Initialize Serial1 (or Serial if available) for ESP32
  // Most ESP32 boards use Serial (GPIO1=TX, GPIO3=RX by default)
  Serial.begin(UART_BAUDRATE);
  
  delay(500);  // Allow UART to stabilize
  
  authState = AUTH_STATE_LOCKED;
  authenticated = 0;
  uartIndex = 0;
  lastUartCheckTime = millis();
}

/**
 * Process incoming UART data from Arduino
 * Looks for "AUTH_OK" message to unlock the system
 */
void auth_update() {
  unsigned long currentTime = millis();
  
  // Check UART every 10ms to avoid blocking
  if (currentTime - lastUartCheckTime < 10) {
    return;
  }
  lastUartCheckTime = currentTime;
  
  // Read available data from UART
  while (Serial.available() > 0) {
    char incomingByte = Serial.read();
    
    // Build the buffer
    if (uartIndex < UART_MAX_BUFFER - 1) {
      uartBuffer[uartIndex++] = incomingByte;
      uartBuffer[uartIndex] = '\0';  // Null terminate for string operations
      
      // Check if we received the authentication message
      if (strstr(uartBuffer, "AUTH_OK") != NULL) {
        authState = AUTH_STATE_UNLOCKED;
        authenticated = 1;
        
        // Clear buffer for next commands
        memset(uartBuffer, 0, UART_MAX_BUFFER);
        uartIndex = 0;
      }
      
      // Check for other messages or clear buffer if too large
      if (uartIndex >= UART_MAX_BUFFER - 1) {
        memset(uartBuffer, 0, UART_MAX_BUFFER);
        uartIndex = 0;
      }
    }
  }
}

/**
 * Check if the system is authenticated
 * @return 1 if authenticated, 0 otherwise
 */
bool auth_is_authenticated() {
  return authenticated == 1;
}

/**
 * Get next input character from UART (if authenticated)
 * @return character if available, 0 otherwise
 */
char auth_get_input() {
  if (!authenticated) {
    return 0;
  }
  
  if (Serial.available() > 0) {
    return Serial.read();
  }
  
  return 0;
}

/**
 * Send a command to Arduino (for future use)
 * @param cmd Command string to send
 */
void auth_send_command(const char* cmd) {
  if (cmd != NULL) {
    Serial.print(cmd);
  }
}
