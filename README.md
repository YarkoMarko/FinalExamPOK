# Звіт до тестеру 74hc595

Протягом цієї роботи треба було написати тестер для 8-бітного зсувного регістру 74hc595 на PSoC 4 в серидовищі PSoC Creator 4.4

# Як працює сам регістр

74HC595 — це 8-бітний зсувний регістр SIPO - Serial in, Parallel out (Дані записуються послідовно біт за бітом через 1 вхід, а виводяться паралельно через декілька виходів). Дані подаються послідовно на вхід SER і зсуваються в регістр по фронту тактового сигналу SHCP. Після завантаження 8 біт інформація передається у вихідний регістр по сигналу STCP, що забезпечує одночасну зміну всіх виходів Q0–Q7. Вихід OE використовується для дозволу/заборони виходів, а MR — для асинхронного скидання регістра.

# Тестер та пояснення коду

Сам тестер має тестувати роботу основного функціоналу регістру - дані на вході SER мають правильно передаватись на виходи Q0-Q7 та тест пам'яті. MR чи OE тестувати не потрібно. Сам тестер спочатку передає 256 різних комбінацій 8 бітів, виходи Q0-Q7 мають показати відповідний біт з входу SER. Якщо на виході дані не співпадають з входом - повідомляється про помилку. Далі йде тест роботи пам'яті. Якщо всі тести пройшли успішно - загорається зелений LED на PSoC 4, в іншому випадку загорається червоний LED. Вся інформація відображається в терміналі, куда вона передається через UART

Спочатку йде оголошення всіх необхідних функцій та змінних

```
#define TEST_DATA_START 0x00
#define TEST_DATA_END   0xFF        

void report_error(void);
void report_OK(void);
void byte_to_binary_string(uint8_t data, char* output);
void print_byte_hex(uint8_t data, const char* label);
void report_test_status(bool passed, const char* message);

void pulse_shift_clock(int bit_index, uint8_t bit_value); 
void pulse_latch_clock(void);
void shift_byte_and_report(uint8_t data);
uint8_t read_parallel_output(void);
```

TEST_DATA_START та TEST_DATA_END - змінні для тестування коректності роботи регістру. 

```
void byte_to_binary_string(uint8_t data, char* output) {
    for (int i = 0; i < 8; i++) {
        output[i] = (data & (1 << (7 - i))) ? '1' : '0';
    }
    output[8] = '\0';
}
```

Функція приймає дані та перетворює їх у 8-бітне двійкове число

```
void print_byte_hex(uint8_t data, const char* label) {
    char str_buf[60];
    char bin_buf[9];
    byte_to_binary_string(data, bin_buf);
    sprintf(str_buf, "%s: 0x%02X (BIN: %s)\r\n", label, data, bin_buf);
    UART_1_UartPutString(str_buf);
}
```

Друкує в терміналі дані в їхньому двійковому та шістнадцятковому форматі. Терміналу присилаються дані через UART

```
void report_error(void) {
    LED_ERROR_Write(0);
    LED_OK_Write(1);
}

void report_OK(void) {
    LED_ERROR_Write(1);
    LED_OK_Write(0);
}
```

Запалюють відповідний LED на PSoC 4, залежно від результату. Зелений, якщо все працює, в іншому випадку червоний

```
void pulse_shift_clock(int bit_index, uint8_t bit_value) {
    char str_buf[50];
    sprintf(str_buf, "  >> SHCP Pulse (Shifting Bit %d, Value %d)\r\n", bit_index, bit_value);
    UART_1_UartPutString(str_buf);

    SHCP_Pin_Write(0);

    SHCP_Pin_Write(1);

    SHCP_Pin_Write(0);
}

void pulse_latch_clock(void) {
    UART_1_UartPutString("--- RCLK Pulse (Latch: Data Transfer to Output) ---\r\n");
    STCP_Pin_Write(0);

    STCP_Pin_Write(1);

    STCP_Pin_Write(0);
}
```

Функції для генерації clock на регістрі зсуву та регістрі пам'яті відповідно

```
void shift_byte_and_report(uint8_t data) {
    print_byte_hex(data, "INPUT Data (SER)");
    for (int i = 0; i < 8; i++) {
        uint8_t bit = (data >> (7 - i)) & 0x01;
        DS_Pin_Write(bit);
        pulse_shift_clock(7 - i, bit);
    }
}
```

Функція друкує дані в терміналі, по черзі передає на регістр даних кожен з 8 бітів і створює тактовий сигнал на SHCP для зсуву

```
uint8_t read_parallel_output(void) {
    uint8_t result = 0;
    if (Q7_Pin_Read()) result |= (1 << 7);
    if (Q6_Pin_Read()) result |= (1 << 6);
    if (Q5_Pin_Read()) result |= (1 << 5);
    if (Q4_Pin_Read()) result |= (1 << 4);
    if (Q3_Pin_Read()) result |= (1 << 3);
    if (Q2_Pin_Read()) result |= (1 << 2);
    if (Q1_Pin_Read()) result |= (1 << 1);
    if (Q0_Pin_Read()) result |= (1 << 0);
    print_byte_hex(result, "OUTPUT Data (Q)");
    return result;
}
```

Функція зчитує дані зі всіх виходів Q0-Q7

Основна робота відбувається в функції main

```
CyGlobalIntEnable;

    UART_1_Start();

    MR_Pin_Write(0); 
    
    MR_Pin_Write(1); 
    
    OE_Pin_Write(0); 
    
    STCP_Pin_Write(0);
    DS_Pin_Write(0);
    SHCP_Pin_Write(0);

    UART_1_UartPutString("\r\n==============================================\r\n");
    UART_1_UartPutString("  74HC595 Functional Tester (Software Speed Test) Started\r\n");
    UART_1_UartPutString("==============================================\r\n");
    UART_1_UartPutString("!!! WARNING: Speed test relies on software delays (CyDelayUs) and is only an estimate. !!!\r\n");

```

Спочатку виставляються потрібні значення на всіх пінах регістру і в терміналі з'являється повідомлення про початок тестування

```
if (test_passed) {
    UART_1_UartPutString("\r\n--- TEST 1: Full Shift and Latch Cycle (Slow, with Log) ---\r\n");

    for (uint16_t i = TEST_DATA_START; i <= TEST_DATA_END; i++) {
        test_data = (uint8_t)i;

        shift_byte_and_report(test_data);
        pulse_latch_clock();
        uint8_t actual_output = read_parallel_output();

        if (actual_output != test_data) {
            report_test_status(false, "Data Mismatch in Full Cycle Test (Output != Input).");
            test_passed = false;
        }
    }
}
```

Перший тест. Проходиться по всіх комбінаціях 0 та 1 в 8-бітному двійковому числі і перевіряє чи співпадають дані на виході з даними на вході

```
uint8_t last_successful_output = test_data;

if (test_passed) {
    UART_1_UartPutString("\r\n--- TEST 2: Memory Integrity Check (Slow) ---\r\n");

    shift_byte_and_report(0x55);
    uint8_t memory_check = read_parallel_output();

    if (memory_check != last_successful_output) {
        char fail_msg[100];
        sprintf(fail_msg, "Latch/Memory Failure: Output changed (Expected 0x%02X, Got 0x%02X) without RCLK pulse.", last_successful_output, memory_check);
        report_test_status(false, fail_msg);
        test_passed = false;
        goto final_report; 
    } else {
        char ok_msg[80];
        sprintf(ok_msg, "Memory check OK (Output stable at 0x%02X).\r\n", last_successful_output);
        UART_1_UartPutString(ok_msg);

        pulse_latch_clock(); 
        uint8_t final_check = read_parallel_output();
        if (final_check != 0x55) {
            report_test_status(false, "Final Latch Failure: Expected 0x55 after RCLK.");
            test_passed = false;
            goto final_report; 
        } else {
            UART_1_UartPutString("Final Latch check OK.\r\n");
        }
    }
}
```

Другий тест. Перевіряє чи змінюються дані лише тоді, коли подався тктовий сигнал на STCP. Стан виходів не може змінюватись від тактового сигналу на SHCP - регістру зсуву. Тільки від стану STCP. Після попередгього тесту на регістрі пам'яті і, відповідно, виходах Q0-Q7 двійкове представлення числа 0хFF. На вхід подається число 0х55 і подається сигнал на регістр зсуву. Зчитуються дані з виходів. Якщо вони залишились 0хFF - все працює коректно. Потім подається сигнал на регістр памяті. Якщо виходи показали 0х55 - все добре

```
if (test_passed) {
    report_test_status(true, "74HC595 PASSED ALL BASIC FUNCTIONAL TESTS.");
} else {
    report_test_status(false, "74HC595 FAILED TEST.");
}
```

В кінці виводиться відповідне повідомлення
