#ifndef PROCESS_TASK_H
#define PROCESS_TASK_H

#define EVENT_BIT_0 (1 << 0)
#define EVENT_BIT_1 (1 << 1)
#define EVENT_BIT_2 (1 << 2)
#define EVENT_BIT_3 (1 << 3)

void vProcessTask(void *pvParameters);

#endif
