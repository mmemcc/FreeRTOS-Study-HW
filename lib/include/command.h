#ifndef COMMAND_H
#define COMMAND_H

#include <FreeRTOS.h>
#include <queue.h>
#include <event_groups.h>

/**
 * echo 커맨드로 입력된 문자열을 저장하는 큐
 */
QueueHandle_t xEchoQueue;
/**
 * event 커맨드로 설정되는 이벤트 그룹
 */
EventGroupHandle_t xEventGroup;

/**
 * 커맨드 입력 시 실행되는 함수
 */
void execute_command(const char *command);

#endif