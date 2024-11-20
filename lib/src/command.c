#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <event_groups.h>

#include <console.h>
#include <command.h>
#include <error.h>

error_t help_command(void)
{
    console_printf("help: 현재 등록된 커맨드와, 커맨드에 대한 설명을 출력\n");
    console_printf("time: 현재 시간을 출력\n");
    console_printf("task: 현재 등록된 task 종류\n");
    console_printf("event: 이벤트 비트 설정\n");
    console_printf("echo: 입력값을 콘솔창에 출력\n");
    console_printf("start_view: 5초 동안 현재 시간과 수행되고 있는 task 출력, 입력 제한\n");

    return OK;
}

error_t time_command(void)
{
    char buffer[30];
    struct tm *tm_info;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    tm_info = localtime(&tv.tv_sec);

    strftime(buffer, 30, "%Y/%m/%d %H:%M:%S", tm_info);
    console_printf("현재 시간은 %s.%04ld 입니다.\n", buffer, tv.tv_usec / 1000);

    return OK;
}

error_t task_command(void)
{
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    uint32_t ulTotalRunTime;

    uxArraySize = uxTaskGetNumberOfTasks();
    pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

    if (pxTaskStatusArray != NULL)
    {
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);

        console_printf("     NAME      | PERIOD |  PRIORITY  |    STATE    |\n");

        for (x = 0; x < uxArraySize; x++)
        {
            console_printf("%-15s|    %-4d|      %-6d|   %-10s|\n",
                           pxTaskStatusArray[x].pcTaskName,
                           pxTaskStatusArray[x].ulRunTimeCounter,
                           pxTaskStatusArray[x].uxCurrentPriority,
                           pxTaskStatusArray[x].eCurrentState == eReady ? "Ready" : pxTaskStatusArray[x].eCurrentState == eRunning ? "Running"
                                                                                : pxTaskStatusArray[x].eCurrentState == eBlocked   ? "Blocked"
                                                                                : pxTaskStatusArray[x].eCurrentState == eSuspended ? "Suspended"
                                                                                                                                   : "Unknown");
        }

        vPortFree(pxTaskStatusArray);
        
    }
    else
    {
        return ERROR_COMMAND_TASK;
    }
    return OK;
}

void start_view_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xDelay = pdMS_TO_TICKS(1000);
    uint32_t i;
    const TickType_t xFiveSeconds = pdMS_TO_TICKS(5000);
    char input[MAX_INPUT_LENGTH];

    // 입력 제한
    if (xSemaphoreTake(xInputMutex, portMAX_DELAY) == pdTRUE)
    {
        for (i = 0; i < 5; i++)
        {
            vTaskSuspend(xConsoleTaskHandle);   

            TickType_t xCurrentTime = xTaskGetTickCount();
            char buffer[30];
            struct tm *tm_info;

            // 현재 시간을 형식에 맞게 변환
            tm_info = localtime(&xCurrentTime);
            strftime(buffer, 30, "%Y/%m/%d %H:%M:%S", tm_info);

            console_printf("-%s.%03ld\n", buffer, xCurrentTime % 1000);

            // 태스크 정보 출력
            task_command();

            char *test = NULL;
            test = fgets(input, MAX_INPUT_LENGTH, stdin);
          
            vTaskDelayUntil(&xLastWakeTime, xDelay);
        }
        
        console_printf("출력이 종료되었습니다. 지금부터 입력이 허용됩니다.\n");
        console_prompt();
        vTaskResume(xConsoleTaskHandle);
        xSemaphoreGive(xInputMutex);
    }

    vTaskDelete(NULL);
}

void echo_task(void *pvParameters)
{
    char buffer[MAX_INPUT_LENGTH];

    for (;;)
    {
        if (xQueueReceive(xEchoQueue, buffer, portMAX_DELAY) == pdTRUE)
        {
            console_printf("입력한 메세지: %s\n", buffer);
            console_prompt();
            vTaskDelete(NULL);
        }
    }
}

error_t echo_command(void)
{
    char input[MAX_INPUT_LENGTH];

    if (xSemaphoreTake(xInputMutex, portMAX_DELAY) == pdTRUE)
    {
        console_printf("콘솔에 출력할 메세지를 입력하세요 : ");
        fflush(stdout);
        char *input_char = NULL;
        for (;;)
        {
            input_char = fgets(input, MAX_INPUT_LENGTH, stdin);
            if (input_char != NULL)
            {
                input_char[strcspn(input_char, "\n")] = '\0';
                xSemaphoreGive(xInputMutex);
                break;
            }
        }
        if (xQueueSend(xEchoQueue, input_char, portMAX_DELAY) != pdTRUE)
        {
            return ERROR_COMMAND_ECHO;
        }
        if (xTaskCreate(vTaskEcho, "echo_task", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL) != pdTRUE)
        {
            return ERROR_COMMAND_ECHO;
        }
    }

    return OK;
}

void event_task1(void *pvParameters)
{
    console_printf("Event Task 1: 이벤트 1이 설정되었습니다.\n");
    for(;;){
        vTaskDelay(pdMS_TO_TICKS(10000));
        vTaskDelete(NULL);
    }
}

void event_task2(void *pvParameters)
{
    console_printf("Event Task 2: 이벤트 2가 설정되었습니다.\n");
    for(;;){
        vTaskDelay(pdMS_TO_TICKS(10000));
        vTaskDelete(NULL);
    }
}

void event_task3(void *pvParameters)
{
    console_printf("Event Task 3: 이벤트 3이 설정되었습니다.\n");
    for(;;){
        vTaskDelay(pdMS_TO_TICKS(10000));
        vTaskDelete(NULL);
    }
}

void event_task4(void *pvParameters)
{
    console_printf("Event Task 4: 이벤트 4가 설정되었습니다.\n");
    for(;;){
        vTaskDelay(pdMS_TO_TICKS(10000));
        vTaskDelete(NULL);
    }
}

void event_create_task(void *pvParameters)
{
    EventBits_t uxBits;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(1000);

    xEventGroupWaitBits(xEventGroup, (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3), pdFALSE, pdFALSE, xTicksToWait);

    if ((uxBits = xEventGroupGetBits(xEventGroup)) & (1 << 0))
    {
        xTaskCreate(event_task1, "Event Task 1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        xEventGroupClearBits(xEventGroup, (1 << 0));
    }
    if ((uxBits = xEventGroupGetBits(xEventGroup)) & (1 << 1))
    {
        xTaskCreate(event_task2, "Event Task 2", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        xEventGroupClearBits(xEventGroup, (1 << 0));
    }
    if ((uxBits = xEventGroupGetBits(xEventGroup)) & (1 << 2))
    {
        xTaskCreate(event_task3, "Event Task 3", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        xEventGroupClearBits(xEventGroup, (1 << 0));
    }
    if ((uxBits = xEventGroupGetBits(xEventGroup)) & (1 << 3))
    {
        xTaskCreate(event_task4, "Event Task 4", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        xEventGroupClearBits(xEventGroup, (1 << 0));
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    console_prompt();
    vTaskDelete(NULL);
}

error_t event_command(void)
{
    char input[5];
    EventBits_t uxBits = 0;

    if (xEventGroup == NULL)
    {
        return ERROR_COMMAND_EVENT;
    }
    if (xSemaphoreTake(xInputMutex, portMAX_DELAY) == pdTRUE)
    {
        console_printf("설정할 이벤트 비트를 4 bit 이진수로 입력하세요: ");
        fflush(stdout);
        char *input_char = NULL;
        for (;;)
        {
            input_char = fgets(input, 5, stdin);
            if (input_char != NULL)
            {
                input_char[strcspn(input_char, "\n")] = '\0';
                if (strlen(input_char) != 4 || strspn(input_char, "01") != 4)
                {
                    console_printf("ERROR: 4 bit 이진수를 입력하세요!\n");
                }
                else
                {
                    while (getchar() != '\n');
                    xSemaphoreGive(xInputMutex);
                    break;
                }
            }
        }

        for (int i = 0; i < 4; i++)
        {
            if (input[i] == '1')
            {
                uxBits |= (1 << (3 - i));
            }
        }

        xTaskCreate(event_create_task, "Event Create Task", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
        xEventGroupSetBits(xEventGroup, uxBits);
    }

    return OK;
}

error_t start_view_command(void)
{
    if (xTaskCreate(start_view_task, "Start View Task", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL) != pdPASS)
    {
        return ERROR_COMMAND_START_VIEW;
    }

    return OK;
}

void execute_command(const char *command)
{
    error_t error;

    if (strcmp(command, "help") == 0)
    {
        error = help_command();
        console_prompt();
    }
    else if (strcmp(command, "time") == 0)
    {
        error = time_command();
        console_prompt();
    }
    else if (strcmp(command, "task") == 0)
    {
        error = task_command();
        console_prompt();
    }
    else if (strcmp(command, "event") == 0)
    {
        error = event_command();
    }
    else if (strcmp(command, "echo") == 0)
    {
        error = echo_command();
    }
    else if (strcmp(command, "start_view") == 0)
    {
        error = start_view_command();
    }
    else
    {
        console_printf("ERROR: 등록된 커맨드가 아닙니다.\n");
        console_prompt();
    }
    if (error != OK)
    {
        console_printf("ERROR CODE: %d\n", error);
        console_prompt();
    }
}
