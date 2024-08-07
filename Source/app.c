//
// Created by ryzcq on 24-8-7.
//
#include "main.h"
#include "usart.h"
#include "MyRTOS.h"

tTask tTask1;
tTask tTask2;
tTask tTask3;

tTaskStack task1Env[128];
tTaskStack task2Env[128];
tTaskStack task3Env[128];

tList list;
tNode node[8];
void task1Entry(void * param) {
    tSetSysTickPeriod(10);

    while(1) {
        HAL_UART_Transmit(&huart1, (uint8_t *)"This is task1\r\n", 15, 1000);
        tTaskDelay(300);
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

void tInitApp(void) {
    tTaskInit(&tTask1, task1Entry, (void *)0x11111111, 0, &task1Env[128]);
    tTaskInit(&tTask2, task2Entry, (void *)0x22222222, 1, &task2Env[128]);
    tTaskInit(&tTask3, task3Entry, (void *)0x33333333, 1, &task3Env[128]);
}