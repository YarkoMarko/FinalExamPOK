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

void print_byte_hex(uint8_t data, const char* label) {
    char str_buf[60];
    char bin_buf[9];
    byte_to_binary_string(data, bin_buf);
    sprintf(str_buf, "%s: 0x%02X (BIN: %s)\r\n", label, data, bin_buf);
    UART_1_UartPutString(str_buf);
}

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
