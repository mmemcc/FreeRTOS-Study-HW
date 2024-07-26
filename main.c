

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <event_groups.h>
#include <timers.h>

/* Local includes. */
#include <console.h> // posix 환경에서 console을 사용하기 위한 헤더파일. printf 대신 사용
#include <time_task.h>
#include <input_task.h>
#include <process_task.h>

// Simulation of the CPU hardware sleeping mode
// Idle task hook, 지우면 안됨
void vApplicationIdleHook( void )
{
    usleep( 15000 );
}

#define QUEUE_LENGTH 10


QueueHandle_t xQueue;
SemaphoreHandle_t xInputMutex;
SemaphoreHandle_t xPrintMutex;
EventGroupHandle_t xEventGroup;
TimerHandle_t xInputTimer;

int main( void )
{
    // 콘솔 초기화
    console_init();

    // FreeRTOS 초기화
    xQueue = xQueueCreate(QUEUE_LENGTH, sizeof(char[100]));
    xInputMutex = xSemaphoreCreateMutex();
    xPrintMutex = xSemaphoreCreateMutex();
    xEventGroup = xEventGroupCreate();

    if (xQueue == NULL || xInputMutex == NULL || xEventGroup == NULL) {
        printf("Failed to create queue, mutex or event group.\n");
        return 1;
    }

    // 뮤텍스 초기 상태를 잠금 상태로 설정
    xSemaphoreTake(xInputMutex, 0);

    // 태스크 생성
    xTaskCreate(vTimeTask, "Time Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vInputTask, "Input Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vProcessTask, "Process Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // 스케줄러 시작
    vTaskStartScheduler();

    // 스케줄러가 종료되는 경우는 메모리 부족 등 치명적인 오류가 발생했을 때 뿐입니다.
    for (;;);
    return 0;
}