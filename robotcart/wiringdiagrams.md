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



