#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <event_groups.h>
#include <timers.h>



QueueHandle_t xInputQueue;
SemaphoreHandle_t xInputMutex;
SemaphoreHandle_t xPrintMutex;
EventGroupHandle_t xEventGroup;