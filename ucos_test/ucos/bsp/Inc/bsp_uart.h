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
#ifndef BSP_UART_H
#define BSP_UART_H

#include "app.h"
#include "usart.h"
#include <stdarg.h>
#include <stdio.h>

/**
 * @brief   a class about uart, provide debug ouput function -- printf()
 * @note    to use thie class, you should declare a User_Uart_T variable, 
 *          like User_Uart_T debug, and call debug.init() first.
 */
class User_Uart_T
{
private:
    UART_HandleTypeDef *m_handle; /* is the base address of the uart that use to debug */
    uint8_t m_rxbuf[128];   /* is the receive buffer */
    bool m_start_receive(void); /* start receive until call end_receive(),
                                    the data that been received is load in m_rxbuf,
                                    and next data will load behind the old data, 
                                    when reach bottom of m_rxbuf, new data will override
                                    the oldest data located at top of m_rxbuf */
    bool m_send_Nchar(uint8_t *str, uint32_t n);    /* send N char */
    bool send_string(const char *str);
    bool send_string(uint8_t *str);
    void low_level_init(void);
public:
    User_Uart_T();
    ~User_Uart_T();
    void init();
    void printf(const char *str, ...); /* just like c standard print() */
    void show_rxbuf(void);
};

#endif // !BSP_UART_H
