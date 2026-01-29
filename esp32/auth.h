#ifndef _auth_h
#define _auth_h

#include <Arduino.h>

// UART Configuration for Arduino Communication
#define UART_BAUDRATE       115200
#define UART_MAX_BUFFER     64

// Authentication states
typedef enum {
  AUTH_STATE_LOCKED = 0,           // Waiting for password entry
  AUTH_STATE_AUTHENTICATING = 1,   // Password being entered
  AUTH_STATE_UNLOCKED = 2          // Password accepted, game can run
} AuthState;

// Function declarations
void auth_setup();
void auth_update();
bool auth_is_authenticated();
char auth_get_input();
void auth_send_command(const char* cmd);

// Global state tracking
extern AuthState authState;
extern uint8_t authenticated;

#endif
