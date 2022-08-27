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
#include "bsp_uart.h"
#include "os.h"
#include "string.h"



/*extern variable declare*/
extern class User_Uart_T Yuki;
extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_uart4_tx;


/**
 * @brief   starting receive data
 * @note    start receive until call end_receive(), the data that been received is load in m_rxbuf,
 *          and next data will load behind the old data, when reach bottom of m_rxbuf, new data 
 *          will override the oldest data located at top of m_rxbuf.
 *  
 */
bool User_Uart_T::m_start_receive(void)
{
    __HAL_DMA_DISABLE(&hdma_uart4_rx);
    SET_BIT(DMA1_Stream0->CR, DMA_SxCR_MINC);
    // __HAL_DMA_ENABLE(&hdma_uart4_rx);
    HAL_UART_Receive_DMA(m_handle, m_rxbuf, 128);
}


User_Uart_T::User_Uart_T()
{

}

User_Uart_T::~User_Uart_T()
{

}

void User_Uart_T::init()
{
    MX_UART4_Init();
    m_handle = &huart4;
    memset(m_rxbuf, 0, sizeof(m_rxbuf));
    m_start_receive();
}
void User_Uart_T::printf(const char *str, ...)
{
    va_list arg_list;
    size_t i = 0;
    char *pstr = (char *)str;
    va_start(arg_list, str);
    for (OS_ERR err = OS_ERR_NONE; pstr[i] != '\0'; i++)
    {
        if(pstr[i] == '%')
        {
            m_send_Nchar((uint8_t *)pstr, i);
            pstr += i;
            i = 0;
            OSTimeDly(3, OS_OPT_TIME_DLY, &err);
            switch (pstr[1])
            {
                static char tmp[12] = {0};
                case 'd':{
                    int input = va_arg(arg_list, int);
                    sprintf(tmp, "%d", input);
                    send_string((uint8_t *)tmp);
                }break;
                case 'x':{
                    unsigned int input = va_arg(arg_list, unsigned int);
                    sprintf(tmp, "0x%x", input);
                    send_string((uint8_t *)tmp);
                }break;
                case 'u':{
                    uint32_t input = va_arg(arg_list, uint32_t);
                    sprintf(tmp, "%lu", input);
                    send_string((uint8_t *)tmp);
                }break;
                case 'f':{
                    double input = va_arg(arg_list, double);
                    sprintf(tmp, "%f", input);
                    send_string((uint8_t *)tmp);
                }break;
                case 's':{
                    uint8_t *input = va_arg(arg_list, uint8_t *);
                    send_string(input);
                }break;
                case 'c':{
                    char input = va_arg(arg_list, int);
                    m_send_Nchar((uint8_t *)&input, 1);
                }break;
                default:{
                    send_string("suggest to check up the input.\r\n");
                }
            }
            pstr += 2; //now pstr point at two char behind '%'
        }
    }
    va_end(arg_list);
    send_string((uint8_t *)pstr);
}

bool User_Uart_T::m_send_Nchar(uint8_t *str, uint32_t n)
{
    __HAL_DMA_DISABLE(&hdma_uart4_tx);
    SET_BIT(DMA1_Stream1->CR, DMA_MINC_ENABLE);
    SET_BIT(m_handle->Instance->CR3 , USART_CR3_DMAT);
    SET_BIT(DMA1_Stream1->CR, DMA_MEMORY_TO_PERIPH);
    if(HAL_DMA_Start_IT(&hdma_uart4_tx, (uint32_t)str, (uint32_t)&(m_handle->Instance->TDR), n) != HAL_OK)
    {
        send_string("send error.\r\n");
        return false;
    }
    return true;
}

bool User_Uart_T::send_string(const char *str)
{
    uint32_t i = 0;
    uint8_t *pstr = (uint8_t *)str;
    for (; pstr[i] != '\0'; i++);
    if(i != 0)
    {
        if(HAL_UART_Transmit(m_handle, pstr, i, 100) != HAL_OK)
            return false;
    }
    else
    {
        send_string("no string.\r\n");
    }
    return true;
}

bool User_Uart_T::send_string(uint8_t *str)
{
    uint32_t i = 0;
    uint8_t *pstr = (uint8_t *)str;
    for (; pstr[i] != '\0'; i++);
    
    if(i != 0)
    {
        __HAL_DMA_DISABLE(&hdma_uart4_tx);
        SET_BIT(DMA1_Stream1->CR, DMA_MINC_ENABLE);
        SET_BIT(m_handle->Instance->CR3 , USART_CR3_DMAT);
        SET_BIT(DMA1_Stream1->CR, DMA_MEMORY_TO_PERIPH);
        if(HAL_DMA_Start_IT(&hdma_uart4_tx, (uint32_t)pstr, (uint32_t)&(m_handle->Instance->TDR), i) != HAL_OK)
        {
            send_string("send error.\r\n");
            return false;
        }
    }
    else
    {
        send_string("no string.\r\n");
    }
    return true;
}


void User_Uart_T::show_rxbuf(void)
{
    send_string(m_rxbuf);
}
