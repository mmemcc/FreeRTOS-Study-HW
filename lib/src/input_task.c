#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <event_groups.h>


#include <stdio.h>
#include <string.h>
#include <input_task.h>
#include <console.h>

extern QueueHandle_t xQueue;
extern SemaphoreHandle_t xInputMutex;
extern SemaphoreHandle_t xPrintMutex;
extern EventGroupHandle_t xEventGroup;

void vInputTask(void *pvParameters) {
    char buffer[20];
    for (;;) {
        if (xSemaphoreTake(xInputMutex, portMAX_DELAY) == pdPASS) {
            xSemaphoreTake(xPrintMutex, portMAX_DELAY);
            console_print("Input Task: Please enter a 4-bit binary value (e.g., 1011): ");
            fflush(stdout);
            console_scanf("%s", buffer);
            
            if  (strlen(buffer) > 0) {
                EventBits_t uxBits = 0;
                for (int i = 0; i < 4; i++) {
                    if (buffer[i] == '1') {
                        uxBits |= (1 << i);
                    }
                }
                xEventGroupSetBits(xEventGroup, uxBits);
                xSemaphoreGive(xPrintMutex);
            }
            
        }
    }
}
