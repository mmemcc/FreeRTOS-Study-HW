#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <timers.h>

#include <stdio.h>
#include <time.h>

#include <time_task.h>
#include <console.h>

extern SemaphoreHandle_t xInputMutex;
extern SemaphoreHandle_t xPrintMutex;
TimerHandle_t xOneSecTimer;

static void vOneSecTimerCallback(TimerHandle_t xTimer) {
    console_print("One second timer callback triggered.\n");
    xSemaphoreGive(xInputMutex);
}

void vTimeTask(void *pvParameters) {
    TickType_t xStartTime = xTaskGetTickCount();

    xOneSecTimer = xTimerCreate("OneSecTimer", pdMS_TO_TICKS(1000), pdFALSE, (void *)0, vOneSecTimerCallback);
    if (xOneSecTimer == NULL) {
        console_print("Failed to create timer.\n");
        vTaskDelete(NULL);
    }

    for (;;) {
        TickType_t xElapsedTime = xTaskGetTickCount() - xStartTime;
        xSemaphoreTake(xPrintMutex, portMAX_DELAY);
        console_print("Elapsed time: %u seconds\n", xElapsedTime / configTICK_RATE_HZ);
        xSemaphoreGive(xPrintMutex);
        if (xElapsedTime / configTICK_RATE_HZ % 5 == 0 && xElapsedTime != 0) {
            printf("Starting 1 second timer for input.\n");
            xTimerStart(xOneSecTimer, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
