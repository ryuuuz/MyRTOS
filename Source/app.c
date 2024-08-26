//
// Created by ryzcq on 24-8-7.
//
#include "main.h"
#include "usart.h"
#include "MyRTOS.h"

tTask tTask1;
tTask tTask2;
tTask tTask3;
tTask tTask4;

tTaskStack task1Env[128];
tTaskStack task2Env[128];
tTaskStack task3Env[128];
tTaskStack task4Env[128];

tTimer timer1;
tTimer timer2;

void timerFunc1(void * arg) {
    HAL_UART_Transmit(&huart1, (uint8_t *)"Timer1 timeout\r\n", 17, 1000);
}

void timerFunc2(void * arg) {
    HAL_UART_Transmit(&huart1, (uint8_t *)"Timer2 timeout\r\n", 17, 1000);
}

void task1Entry(void * param) {
    tSetSysTickPeriod(10);

    tTimerInit(&timer1, 100, 100, timerFunc1, (void *)0x11111111);
    tTimerStart(&timer1);
    tTimerInit(&timer2, 200, 200, timerFunc2, (void *)0x22222222);
    tTimerStart(&timer2);

    while(1) {
        HAL_UART_Transmit(&huart1, (uint8_t *)"This is task1\r\n", 15, 1000);
        tTaskDelay(100);
    }
}

void task2Entry(void * param) {
    while(1) {
        HAL_UART_Transmit(&huart1, (uint8_t *)"This is task2\r\n", 15, 1000);
        tTaskDelay(100);
    }
}

void task3Entry(void * param) {
    while(1) {
        HAL_UART_Transmit(&huart1, (uint8_t *)"This is task3\r\n", 15, 1000);
        tTaskDelay(100);
    }
}

void task4Entry(void * param) {
    while(1) {
        HAL_UART_Transmit(&huart1, (uint8_t *)"This is task4\r\n", 15, 1000);
        tTaskDelay(100);
    }
}

void tInitApp(void) {
    tTaskInit(&tTask1, task1Entry, (void *)0x11111111, 0, &task1Env[128]);
    tTaskInit(&tTask2, task2Entry, (void *)0x22222222, 1, &task2Env[128]);
    // tTaskInit(&tTask3, task3Entry, (void *)0x33333333, 1, &task3Env[128]);
    // tTaskInit(&tTask4, task4Entry, (void *)0x44444444, 1, &task4Env[128]);
}