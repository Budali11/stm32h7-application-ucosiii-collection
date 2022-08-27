// Copyright 2022 HaoNan
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "os.h"
#include "usart.h"
#include "gpio.h"
#include "app.h"
#include "bsp_uart.h"

/*extern variable declare*/
extern class User_Uart_T Yuki;

OS_TCB  MyTaskTCB;
CPU_STK my_task_stk[256];
void My_Task(void *param)
{
	OS_ERR err = OS_ERR_NONE;
    Yuki.printf("Mytask, running. %d \r\n", -1);
	while(DEF_TRUE)
	{
		OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
	}
}

void Start_Task(void *param)
{
	OS_ERR err = OS_ERR_NONE;
    OSTaskCreate(&MyTaskTCB, (CPU_CHAR *)"my task", My_Task, NULL, 10, my_task_stk, \
            0, 256, 0, 10, NULL, OS_OPT_NONE, &err);
    while(DEF_TRUE)
    {
        HAL_GPIO_TogglePin(BLUE_GPIO_Port, BLUE_Pin);
        OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
    }
}
