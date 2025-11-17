flowchart TD
    A[UART Command Received] --> B[uart_handle_command()]
    B --> C{Command Type}
    C -->|F| D[forward()]
    C -->|B| E[backward()]
    C -->|L| F[left()]
    C -->|R| G[right()]
    C -->|S| H[stop()]
    C -->|*| I[spin()]
    C -->|T| J[self_test_mode()]

    D --> K[Update Motors via GPIOB]
    E --> K
    F --> K
    G --> K
    H --> K
    I --> K
    J --> K

    K --> L[Update Lights / Indicators / Buzzer via GPIOC]
    K --> M[Update LCD Status]


graph TD
    MCU[STM32 MCU]

    MCU -->|PB15| LM1[Left Motor Input 1]
    MCU -->|PB2| LM2[Left Motor Input 2]
    MCU -->|PB3| RM1[Right Motor Input 1]
    MCU -->|PB4| RM2[Right Motor Input 2]

    MCU -->|PC9| HEAD[Headlights]
    MCU -->|PC8| BRAKE[Brake Lights]
    MCU -->|PC6| LIND[Left Indicator]
    MCU -->|PC5| RIND[Right Indicator]
    MCU -->|PC13| BUZZ[Buzzer]
    MCU -->|PC1| CARTLED[Heartbeat LED]

    MCU -->|PA0| LCD_RS[RS]
    MCU -->|PB13| LCD_EN[EN]
    MCU -->|PA5-PA12| LCD_DATA[Data D0-D7]

    MCU -->|PA9| UART_TX[UART TX]
    MCU -->|PA10| UART_RX[UART RX]

    MCU -->|PA5| LED[Onboard LED]
