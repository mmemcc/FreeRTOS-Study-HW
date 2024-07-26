#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <semphr.h>

#include <stdio.h>
#include <string.h>

#include <process_task.h>
#include <console.h>
#include <handle.h>



void vProcessTask(void *pvParameters) {
    for (;;) {
        UBaseType_t input_queue = uxQueueMessagesWaiting(xInputQueue);
        xSemaphoreTake(xPrintMutex, portMAX_DELAY);
        if (uxBits & EVENT_BIT_0) {
            console_print("Process Task: Event bit 0 set.\n");
        }
        if (uxBits & EVENT_BIT_1) {
            console_print("Process Task: Event bit 1 set.\n");
        }
        if (uxBits & EVENT_BIT_2) {
            console_print("Process Task: Event bit 2 set.\n");
        }
        if (uxBits & EVENT_BIT_3) {
            console_print("Process Task: Event bit 3 set.\n");
        }
        xSemaphoreGive(xPrintMutex);
    }
}
