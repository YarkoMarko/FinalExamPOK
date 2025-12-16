#include "project.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define TEST_DATA_START 0x00
#define TEST_DATA_END   0xFF

#define START_DELAY_US  10UL       
#define DELAY_STEP_US   1UL        
#define MIN_DELAY_US    0UL        

void report_error(void);
void report_OK(void);
void byte_to_binary_string(uint8_t data, char* output);
void print_byte_hex(uint8_t data, const char* label);
void report_test_status(bool passed, const char* message);

void pulse_shift_clock(int bit_index, uint8_t bit_value); 
void pulse_latch_clock(void);
void shift_byte_and_report(uint8_t data);
uint8_t read_parallel_output(void);

void pulse_shift_clock_fast(uint8_t bit_value, uint32_t delay_us); 
void shift_byte_software_speed_test(uint8_t data, uint32_t delay_us);
uint8_t read_parallel_output_fast(void);
void pulse_latch_clock_fast(void);


void byte_to_binary_string(uint8_t data, char* output) {
    for (int i = 0; i < 8; i++) {
        output[i] = (data & (1 << (7 - i))) ? '1' : '0';
    }
    output[8] = '\0';
}

void print_byte_hex(uint8_t data, const char* label) {
    char str_buf[60];
    char bin_buf[9];
    byte_to_binary_string(data, bin_buf);
    sprintf(str_buf, "%s: 0x%02X (BIN: %s)\r\n", label, data, bin_buf);
    UART_1_UartPutString(str_buf);
}

void report_error(void) {
    LED_ERROR_Write(0);
    LED_OK_Write(1);
}

void report_OK(void) {
    LED_ERROR_Write(1);
    LED_OK_Write(0);
}

void report_test_status(bool passed, const char* message) {
    if (passed) {
        UART_1_UartPutString("\r\n*** SUCCESS: ");
        report_OK();
    } else {
        UART_1_UartPutString("\r\n!!! FAILURE: ");
        report_error();
    }
    UART_1_UartPutString(message);
    UART_1_UartPutString(" ***\r\n");
}

void pulse_shift_clock(int bit_index, uint8_t bit_value) {
    char str_buf[50];
    sprintf(str_buf, "  >> SHCP Pulse (Shifting Bit %d, Value %d)\r\n", bit_index, bit_value);
    UART_1_UartPutString(str_buf);

    SHCP_Pin_Write(0);
    //CyDelayUs(1);
    SHCP_Pin_Write(1);
    //CyDelayUs(1);
    SHCP_Pin_Write(0);
}

void pulse_shift_clock_fast(uint8_t bit_value, uint32_t delay_us) {
    DS_Pin_Write(bit_value); // Встановлюємо біт перед імпульсом
    
    SHCP_Pin_Write(0);
    //if (delay_us > 0) CyDelayUs(delay_us);
    
    SHCP_Pin_Write(1);
    //if (delay_us > 0) CyDelayUs(delay_us);
    
    SHCP_Pin_Write(0);
}

void pulse_latch_clock(void) {
    UART_1_UartPutString("--- RCLK Pulse (Latch: Data Transfer to Output) ---\r\n");
    STCP_Pin_Write(0);
    //CyDelayUs(10);
    STCP_Pin_Write(1);
    //CyDelayUs(10);
    STCP_Pin_Write(0);
}

void pulse_latch_clock_fast(void) {
    STCP_Pin_Write(0);
    STCP_Pin_Write(1);
    STCP_Pin_Write(0);
}

void shift_byte_and_report(uint8_t data) {
    print_byte_hex(data, "INPUT Data (SER)");
    for (int i = 0; i < 8; i++) {
        uint8_t bit = (data >> (7 - i)) & 0x01;
        DS_Pin_Write(bit);
        pulse_shift_clock(7 - i, bit);
    }
}

void shift_byte_software_speed_test(uint8_t data, uint32_t delay_us) {
    uint8_t bit_to_shift = (data == 0xFF) ? 1 : 0;
    
    for (int i = 0; i < 8; i++) {
        pulse_shift_clock_fast(bit_to_shift, delay_us); 
    }
}

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

uint8_t read_parallel_output_fast(void) {
    uint8_t result = 0;
    if (Q7_Pin_Read()) result |= (1 << 7);
    if (Q6_Pin_Read()) result |= (1 << 6);
    if (Q5_Pin_Read()) result |= (1 << 5);
    if (Q4_Pin_Read()) result |= (1 << 4);
    if (Q3_Pin_Read()) result |= (1 << 3);
    if (Q2_Pin_Read()) result |= (1 << 2);
    if (Q1_Pin_Read()) result |= (1 << 1);
    if (Q0_Pin_Read()) result |= (1 << 0);
    return result;
}

int main(void)
{
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

    LED_ERROR_Write(1);
    LED_OK_Write(1);

    bool test_passed = true;
    uint8_t test_data = 0;

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
                goto final_report; 
            }

            //CyDelay(5);
        }
    }
    
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

            pulse_latch_clock(); // Фіксуємо 0x55
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
    
    if (test_passed) {
        UART_1_UartPutString("\r\n--- TEST 3: Software Shift Clock Speed Test ---\r\n");
        
        uint32_t current_delay_us = START_DELAY_US;
        unsigned long max_stable_freq_khz = 0;
        
        while (current_delay_us >= MIN_DELAY_US) {
            
            unsigned long current_freq_khz = (current_delay_us > 0) 
                ? (1000000UL / (2 * current_delay_us)) / 1000UL 
                : 99999UL; 
            
            char str_buf[150];
            
            shift_byte_software_speed_test(0x00, current_delay_us);
            pulse_latch_clock_fast();
            uint8_t actual_output_00 = read_parallel_output_fast();
            
            if (actual_output_00 != 0x00) {
                max_stable_freq_khz = (current_delay_us + DELAY_STEP_US) > START_DELAY_US ? 0 : (1000000UL / (2 * (current_delay_us + DELAY_STEP_US)) / 1000UL);
                
                sprintf(str_buf, "\r\n!!! FAILURE at estimated %.1f kHz (Test 0x00) (Delay %lu us) !!!\r\n", 
                        (float)current_freq_khz, current_delay_us);
                UART_1_UartPutString(str_buf);
                print_byte_hex(0x00, "Expected");
                print_byte_hex(actual_output_00, "Actual");
                test_passed = false;
                goto final_report; 
            }
            
            shift_byte_software_speed_test(0xFF, current_delay_us);
            pulse_latch_clock_fast();
            uint8_t actual_output_FF = read_parallel_output_fast();
            
            if (actual_output_FF != 0xFF) {
                max_stable_freq_khz = (current_delay_us + DELAY_STEP_US) > START_DELAY_US ? 0 : (1000000UL / (2 * (current_delay_us + DELAY_STEP_US)) / 1000UL);
                
                sprintf(str_buf, "\r\n!!! FAILURE at estimated %.1f kHz (Test 0xFF) (Delay %lu us) !!!\r\n", 
                        (float)current_freq_khz, current_delay_us);
                UART_1_UartPutString(str_buf);
                print_byte_hex(0xFF, "Expected");
                print_byte_hex(actual_output_FF, "Actual");
                test_passed = false;
                goto final_report; 
            }

            
            max_stable_freq_khz = current_freq_khz;
            sprintf(str_buf, "  >> Stable at estimated %.1f kHz (Delay %lu us).\r\n", (float)current_freq_khz, current_delay_us);
            
            if (current_delay_us == MIN_DELAY_US) break;
            
            current_delay_us = (current_delay_us > DELAY_STEP_US) ? (current_delay_us - DELAY_STEP_US) : MIN_DELAY_US;
        }
        
        
        if (max_stable_freq_khz > 0) {
            char final_msg[120];
            sprintf(final_msg, "Max Stable Frequency (ESTIMATED): %.1f kHz (~%.2f MHz).\r\n", 
                    (float)max_stable_freq_khz, (float)max_stable_freq_khz / 1000.0f);
            report_test_status(true, final_msg);
        } else {
             report_test_status(false, "Speed test failed immediately (check connections).");
             test_passed = false;
        }
    }

final_report:; 
    if (test_passed) {
        report_test_status(true, "74HC595 PASSED ALL BASIC FUNCTIONAL TESTS.");
    } else {
        report_test_status(false, "74HC595 FAILED TEST.");
    }
    
    return 0; 
}