

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Local includes. */
#include "console.h" // posix 환경에서 console을 사용하기 위한 헤더파일. printf 대신 사용

// Simulation of the CPU hardware sleeping mode
// Idle task hook, 지우면 안됨
void vApplicationIdleHook( void )
{
    usleep( 15000 );
}


// Task "Sleep"
static void prvTaskHello( void * _ )
{
    while ( 1 )
    {
        console_print( "zzzzzzz\n" );
        vTaskDelay( 1000 );
    }
}


// Task "Ring"
static void prvTaskRing( void *t_arg )
{
    TaskHandle_t l_task_id = * ( TaskHandle_t * ) t_arg;
    while ( 1 )
    {
        vTaskDelay( 5000 );
        console_print( "알람! 알람! 알람!\n" );
        vTaskResume( l_task_id );
    }
}


// Task "PutOff"
static void prvTaskSleep( void * _ )
{
    while ( 1 )
    {
        vTaskSuspend( NULL );
        console_print( "5분만..\n" );
    }
}


int main()
{
    // Init output console
    console_init();

    TaskHandle_t l_handle;

    // Three tasks
    xTaskCreate( prvTaskHello, "Sleep", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 4, NULL );
    xTaskCreate( prvTaskSleep, "PutOff", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 3, &l_handle );
    xTaskCreate( prvTaskRing, "Ring", configMINIMAL_STACK_SIZE, &l_handle, configMAX_PRIORITIES - 2, NULL );

    // Start scheduler
    vTaskStartScheduler();

    return 0;
}