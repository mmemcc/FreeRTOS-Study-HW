

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
#include <console.h>
#include <error.h>
#include <command.h>
#include <console.h>

#define ECHO_QUEUE_LENGTH 15

// Simulation of the CPU hardware sleeping mode
// Idle task hook, 지우면 안됨
void vApplicationIdleHook( void )
{
    usleep( 15000 );
}

int main( void )
{
    xEventGroup = xEventGroupCreate();
    xEchoQueue = xQueueCreate(ECHO_QUEUE_LENGTH, sizeof(char[MAX_INPUT_LENGTH]));

    console_init();

    vTaskStartScheduler();

    for (;;);
    return 0;
}