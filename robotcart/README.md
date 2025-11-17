# BAT Mobile Robot Cart Documentation

## 1. Overview
The BAT Mobile is a microcontroller-driven robot cart capable of receiving movement commands via UART, controlling motor directions, lights, a buzzer, and displaying status on a 16×2 LCD.  

### System Components
- **Motors:** Left and right wheels, H-bridge controlled via GPIO  
- **Lights & Indicators:** Headlights, brake lights, turn indicators, hazard lights, reversing buzzer, heartbeat LED  
- **Display:** 16×2 LCD for status messages  
- **Communication:** UART interface for remote control (e.g., BLE module)  
- **Timers:** TIM6 & TIM15 for lights, buzzer, and LED patterns  
- **MCU:** STM32 (generic)  

---

## 2. Pin Mapping Table

| Component          | Pin / Port | Description                         |
|-------------------|------------|-------------------------------------|
| Left Motor Input 1 | PB15       | Motor control H-bridge input         |
| Left Motor Input 2 | PB2        | Motor control H-bridge input         |
| Right Motor Input 1| PB3        | Motor control H-bridge input         |
| Right Motor Input 2| PB4        | Motor control H-bridge input         |
| LCD RS             | PA0        | LCD register select                  |
| LCD EN             | PB13       | LCD enable                           |
| LCD D0-D7          | PA5–PA12   | LCD data bus                         |
| Headlights         | PC9        | Front lights                         |
| Brake Lights       | PC8        | Rear lights                           |
| Left Indicator     | PC6        | Blinking left turn indicator          |
| Right Indicator    | PC5        | Blinking right turn indicator         |
| Reversing Buzzer   | PC13       | Beeps while reversing                 |
| Onboard LED        | PA5        | Heartbeat / status LED                |
| Cart Status LED    | PC1        | TIM15 heartbeat pattern LED           |
| UART TX            | PA9        | Transmit UART1                        |
| UART RX            | PA10       | Receive UART1                         |

---

## 3. Software Architecture

### 3.1 Main Module (`main.c`)
- Initializes peripherals: LCD, UART, GPIO, Lights, Motors, Timers  
- Main loop: waits for UART command, processes via `uart_handle_command()`  

### 3.2 Motor Module (`motor.c` / `motor.h`)
- Functions:
  - `forward()`, `backward()`, `left()`, `right()`, `spin()`, `stop()`  
  - `self_test_mode()` – runs all movements sequentially for 3s each  
- GPIO pins for motor H-bridge controlled via BSRR/BRR registers  
- Updates LCD with movement status  
- Updates lights accordingly  

### 3.3 Communications Module (`comms.c` / `comms.h`)
- UART initialization and handling  
- Functions:
  - `uart_init()`, `uart_send_char()`, `uart_receive_char()`  
  - `uart_handle_command(char cmd)` – maps characters to movement functions  
- Supported commands:

| Command | Action                 |
|---------|-----------------------|
| F       | Forward               |
| B       | Backward + reversing  |
| L       | Turn Left             |
| R       | Turn Right            |
| S       | Stop                  |
| *       | Spin                  |
| T       | Run Self-test Mode    |

- Sends acknowledgement via UART for each command  

### 3.4 Lights Module (`lights.c` / `lights.h`)
- Functions:
  - `lights_init()`, `lights(x)`, `buzzer(x)`, `onboard_led_init()`  
  - `timer6_init()`, `timer15_init()`, `cartled_init()`  
- Global state variables:
  - `turning_left`, `turning_right`, `reversing`  
- **TIM6 ISR handles:**
  - Onboard LED blinking (200ms)  
  - Turn indicator flashing (500ms)  
  - Reversing buzzer (1s)  
- **TIM15 ISR handles heartbeat pattern** for `CARTLED`  

### 3.5 LCD Module (`lcd.c` / `lcd.h`)
- 16×2 LCD driver with custom pin mapping  
- Functions:
  - `lcd_init()`, `lcd_clear()`, `lcd_gotoXY()`, `lcd_print_string()`, `lcd_clear_bottomRow()`  
- Uses 8-bit data bus (PA5–PA12) + RS (PA0) + EN (PB13)  

---

## 4. System Flow


- Self-test mode runs motors, lights, and buzzer sequentially  
- Timers ensure blinking indicators, buzzer timing, and heartbeat LED  

---

## 5. Timer & Interrupt Behavior
- **TIM6:** 100ms tick  
  - Onboard LED toggle every 200ms  
  - Indicators toggle every 500ms  
  - Buzzer toggle every 1s while reversing  
- **TIM15:** 100ms tick  
  - Heartbeat LED pattern  

---

## 6. Self-Test Mode Sequence
1. Forward → 3s  
2. Backward → 3s  
3. Left → 3s  
4. Right → 3s  
5. Spin → 3s  
6. Stop & turn off all lights  

All steps update the LCD and indicator/buzzer patterns.  

---

## 7. Developer Notes
- **Extending Commands:** Add new case in `uart_handle_command()` and implement motor/light functions as needed  
- **Changing Pins:** Update macros in `motor.h`, `lights.h`, `lcd.h`  
- **Porting:** All direct register manipulations may need adaptation for different STM32 models  

---

## 8. Safety & Maintenance
- Ensure motors are disconnected when modifying hardware  
- Do not exceed GPIO current limits (typical max 20–25mA per pin)  
- Use series resistors if external LEDs are added  
- Confirm UART connections match voltage levels of external controller  
