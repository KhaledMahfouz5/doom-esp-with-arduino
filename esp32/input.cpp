#include <Arduino.h>
#include "input.h"
#include "constants.h"
#include "auth.h"

#ifdef USE_INPUT_PULLUP
#define INPUT_MODE INPUT_PULLUP
#define INPUT_STATE LOW
#else
#define INPUT_MODE INPUT
#define INPUT_STATE HIGH
#endif

// UART-based input state (receives commands from Arduino)
static uint8_t uart_input_state = 0;
static unsigned long uart_input_timeout = 0;
#define UART_INPUT_TIMEOUT 100 // 100ms timeout for button holds

#ifdef SNES_CONTROLLER
uint16_t buttons = 0;

void input_setup()
{
  // Set DATA_CLOCK normally HIGH
  pinMode(DATA_CLOCK, OUTPUT);
  digitalWrite(DATA_CLOCK, HIGH);

  // Set DATA_LATCH normally LOW
  pinMode(DATA_LATCH, OUTPUT);
  digitalWrite(DATA_LATCH, LOW);

  // Set DATA_SERIAL normally HIGH
  pinMode(DATA_SERIAL, OUTPUT);
  digitalWrite(DATA_SERIAL, HIGH);
  pinMode(DATA_SERIAL, INPUT);
}

void getControllerData(void)
{
  // Latch for 12us
  digitalWrite(DATA_LATCH, HIGH);
  delayMicroseconds(12);
  digitalWrite(DATA_LATCH, LOW);
  delayMicroseconds(6);
  buttons = 0;
  // Retrieve button presses from shift register by pulling the clock high for 6us
  for (uint8_t i = 0; i < 16; ++i)
  {
    digitalWrite(DATA_CLOCK, LOW);
    delayMicroseconds(6);
    buttons |= !digitalRead(DATA_SERIAL) << i;
    digitalWrite(DATA_CLOCK, HIGH);
    delayMicroseconds(6);
  }
}

bool input_left()
{
  return buttons & LEFT;
};

bool input_right()
{
  return buttons & RIGHT;
};

bool input_up()
{
  return buttons & UP;
};

bool input_down()
{
  return buttons & DOWN;
};

bool input_fire()
{
  return buttons & Y;
};

bool input_start()
{
  return buttons & START;
}
#else

void input_setup()
{
  pinMode(K_LEFT, INPUT_MODE);
  pinMode(K_RIGHT, INPUT_MODE);
  pinMode(K_UP, INPUT_MODE);
  pinMode(K_DOWN, INPUT_MODE);
  pinMode(K_FIRE, INPUT_MODE);
}

bool input_left()
{
  return digitalRead(K_LEFT) == INPUT_STATE;
};

bool input_right()
{
  return digitalRead(K_RIGHT) == INPUT_STATE;
};

bool input_up()
{
  return digitalRead(K_UP) == INPUT_STATE;
};

bool input_down()
{
  return digitalRead(K_DOWN) == INPUT_STATE;
};

bool input_fire()
{
  return digitalRead(K_FIRE) == INPUT_STATE;
};
#endif

/**
 * Process UART input from Arduino
 * Maps keyboard characters to game button states
 * Called periodically to update input state from Arduino
 */
void process_uart_input()
{
  if (!auth_is_authenticated())
  {
    return; // No input until authenticated
  }

  char input = auth_get_input();

  if (input != 0)
  {
    // Clear previous timeout
    uart_input_timeout = millis();

    // Map Arduino keyboard input to game commands
    switch (input)
    {
    case '4':                       // LEFT arrow
      uart_input_state |= (1 << 0); // Set LEFT bit
      break;
    case '6':                       // RIGHT arrow
      uart_input_state |= (1 << 1); // Set RIGHT bit
      break;
    case '2':                       // UP arrow
      uart_input_state |= (1 << 2); // Set UP bit
      break;
    case '8':                       // DOWN arrow
      uart_input_state |= (1 << 3); // Set DOWN bit
      break;
    case '5':                       // Fire button (center)
      uart_input_state |= (1 << 4); // Set FIRE bit
      break;
    case '*':                       // Special button (could be jump/action)
      uart_input_state |= (1 << 5); // Set special bit
      break;
    default:
      break;
    }
  }

  // Timeout input after UART_INPUT_TIMEOUT ms
  if (millis() - uart_input_timeout > UART_INPUT_TIMEOUT)
  {
    uart_input_state = 0;
  }
}

/**
 * Get UART input state
 * @param bit Bit position to check
 * @return 1 if bit is set, 0 otherwise
 */
static uint8_t get_uart_input_bit(uint8_t bit)
{
  process_uart_input();
  return (uart_input_state >> bit) & 1;
}

// Alternative input functions for UART-based controls
bool uart_input_left()
{
  return get_uart_input_bit(0);
}

bool uart_input_right()
{
  return get_uart_input_bit(1);
}

bool uart_input_up()
{
  return get_uart_input_bit(2);
}

bool uart_input_down()
{
  return get_uart_input_bit(3);
}

bool uart_input_fire()
{
  return get_uart_input_bit(4);
}
