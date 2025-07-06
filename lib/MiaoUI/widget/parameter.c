/*
 * This file is part of the MiaoUI Library.
 *
 * Copyright (c) 2025, JFeng-Z, <2834294740@qq.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Created on: 2025-02-08
 */
#include "widget.h"
#include "stdlib.h"
#include "string.h"
#include "ui.h"
#include "dispDriver.h"
#include "fonts.h"
#include "math.h"

void ParameterSetting_Widget_Style1(ui_t *ui)
{
    char value[24] = {0};
    int x = 4, y = 12, w = ui->hor_res - 8, h = ui->ver_res - 32;
    float Step = ui->nowItem->element->data->step;

    #if ( UI_USE_FREERTOS == 1 )
    if(ui->nowItem->element->data->functionType == UI_DATA_FUNCTION_EXIT_EXECUTE)
    {
        if(ui->nowItem->element->data->dataRootMutex != NULL)xSemaphoreTake(*ui->nowItem->element->data->dataRootMutex, portMAX_DELAY);
        if(ui->nowItem->element->data->dataRootTask != NULL)vTaskSuspend(*ui->nowItem->element->data->dataRootTask);
        if(ui->nowItem->element->data->dataRootMutex != NULL)xSemaphoreGive(*ui->nowItem->element->data->dataRootMutex);
    }
    #endif
    
    if (Dialog_Show(ui, x, y, w, h))
    {
        switch (ui->action)
        {
        case UI_ACTION_UP:
            Draw_Scrollbar(ui, x + 4, y + 18, w - 30, 6, 3, Step);
            if(ui->nowItem->element->data->function != NULL && ui->nowItem->element->data->functionType == UI_DATA_FUNCTION_STEP_EXECUTE)ui->nowItem->element->data->function(ui);
            break;
        case UI_ACTION_DOWN:
            Step = -Step;
            Draw_Scrollbar(ui, x + 4, y + 18, w - 30, 6, 3, Step);
            if(ui->nowItem->element->data->function != NULL && ui->nowItem->element->data->functionType == UI_DATA_FUNCTION_STEP_EXECUTE)ui->nowItem->element->data->function(ui);
            break;
        default:
            Draw_Scrollbar(ui, x + 4, y + 18, w - 30, 6, 3, 0);
            break;
        }
        if(ui->nowItem->element->data->name == NULL)ui->nowItem->element->data->name = "Null name";
        switch (ui->nowItem->element->data->dataType)
        {
        case UI_DATA_INT:
            snprintf(value, sizeof(value), "%s:%d ", ui->nowItem->element->data->name, *(int *)(ui->nowItem->element->data->ptr));
            break;
        case UI_DATA_FLOAT:
            snprintf(value, sizeof(value), "%s:%.2f ", ui->nowItem->element->data->name, *(float *)(ui->nowItem->element->data->ptr));
        default:
            break;
        }
        Disp_DrawStr(x + 4, y + 13, value);
        Disp_SendBuffer();
    }
}

typedef enum
{
    UI_PARAMETER_NORMAL,
    UI_PARAMETER_BLINK,
    UI_PARAMETER_SELECT
}PARAMETER_STATE;

uint8_t get_value_digit(ui_t *ui)
{
    uint8_t digit = 0;
    int value;
    if(abs(ui->nowItem->element->data->min) > abs(ui->nowItem->element->data->max))value = ui->nowItem->element->data->min;
    else value = ui->nowItem->element->data->max;
    digit = (int)log10f(value);
    return digit;
}

void ParameterSetting_Widget_Style2(ui_t *ui)
{
    #if ( UI_USE_FREERTOS == 1 )
    if(ui->nowItem->element->data->functionType == UI_DATA_FUNCTION_EXIT_EXECUTE)
    {
        if(ui->nowItem->element->data->dataRootMutex != NULL)xSemaphoreTake(*ui->nowItem->element->data->dataRootMutex, portMAX_DELAY);
        if(ui->nowItem->element->data->dataRootTask != NULL)vTaskSuspend(*ui->nowItem->element->data->dataRootTask);
        if(ui->nowItem->element->data->dataRootMutex != NULL)xSemaphoreGive(*ui->nowItem->element->data->dataRootMutex);
    }
    #endif
    const char *name = ui->nowItem->element->data->name;
    char value[24] = {0};
    char word[1] = {0};
    uint8_t digit = 0;
    static PARAMETER_STATE state = UI_PARAMETER_NORMAL;
    Disp_SetFont(ui->font.ptr);
    Disp_ClearBuffer();
    uint8_t color = 2;
    Disp_SetDrawColor(&color);
    if(name != NULL)Disp_DrawStr(0, ui->font.height, name);
    else Disp_DrawStr(0, ui->font.height, "Null Name");

    switch (ui->nowItem->element->data->dataType)
    {
    case UI_DATA_INT:
        snprintf(value, sizeof(value), "%d", *(int *)ui->nowItem->element->data->ptr);
        break;
    case UI_DATA_FLOAT:
        snprintf(value, sizeof(value), "%.2f", *(float *)ui->nowItem->element->data->ptr);
        break;
    case UI_DATA_STRING:
        strncpy(value, (char *)ui->nowItem->element->data->ptr, sizeof(value));
        break;
    default:
        break;
    }
    static uint16_t num = 0;
    uint8_t max_num = Get_StrLen(value) - 1; //排除回车
    word[0] = value[num];

    switch (state)
    {
    case UI_PARAMETER_NORMAL:
        digit = get_value_digit(ui);
        state = UI_PARAMETER_BLINK;
        break;
    case UI_PARAMETER_BLINK:
        switch (ui->action)
        {
        case UI_ACTION_UP:
            if(num > 0)num --;
            break;
        case UI_ACTION_DOWN:
            if(num < max_num)num ++;
            break;
        case UI_ACTION_ENTER:
            ui->action = UI_ACTION_NONE;
            state = UI_PARAMETER_SELECT;
            break;
        default:
            break;
        }
        break;
    case UI_PARAMETER_SELECT:
        switch (ui->action)
        {
        case UI_ACTION_UP:
            if(ui->nowItem->element->data->dataType != UI_DATA_STRING)
            {
                if(value[num] >= 0x30 && value[num] < 0x39)value[num] += 1;
            }
            else 
            {
                if(value[num] >= 0x21 && value[num] < 0x7e)value[num] += 1;
            }    
            break;
        case UI_ACTION_DOWN:
            if(ui->nowItem->element->data->dataType != UI_DATA_STRING)
            {
                if(value[num] > 0x30 && value[num] <= 0x39)value[num] -= 1;
            }
            else 
            {
                if(value[num] > 0x21 && value[num] <= 0x7e)value[num] -= 1;
            }    
            break;
        case UI_ACTION_ENTER:
            state = UI_PARAMETER_NORMAL;
            num = 0;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    Disp_DrawStr(0, ui->font.height*2 + 5, value);
    
    ui_piece_t Word_piece = {.box_x = 0, .box_y = ui->font.height*3 + 3};
    Draw_Piece(Arial_Bold_Italic_24_custom, word, &Word_piece);
    Disp_DrawBox(Word_piece.box_x, Word_piece.box_y, Word_piece.box_w, Word_piece.box_h);

    if(ui->nowItem->element->data->dataType == UI_DATA_INT)*(int *)ui->nowItem->element->data->ptr = atoi(value);
    if(ui->nowItem->element->data->dataType == UI_DATA_FLOAT)*(float *)ui->nowItem->element->data->ptr = atof(value);
    if(ui->nowItem->element->data->dataType == UI_DATA_STRING)strcpy((char *)ui->nowItem->element->data->ptr, value);

    snprintf(value, sizeof(value), "0x%02x", word[0]);
    Disp_DrawStr(30, ui->font.height*5, value);

    Disp_SendBuffer();
    // static uint8_t blink = 0;
    if(state == UI_PARAMETER_BLINK)
    {
        // blink ++;
        // if(blink > 10)
        // {
        //     blink = 0;
        //     Disp_DrawBox(ui->font.width*num, ui->font.height + 6, ui->font.width, ui->font.height);
        //     Disp_SendBuffer();
        // }
    }
}

void Switch_Widget(ui_t *ui)
{
    *(uint8_t *)ui->nowItem->element->data->ptr = ! *(uint8_t *)ui->nowItem->element->data->ptr; // 切换开关状态
    #if ( UI_USE_FREERTOS == 1 )
    if(ui->nowItem->element->data->dataRootTask != NULL)
    {
        switch (*(uint8_t *)ui->nowItem->element->data->ptr)
        {
        case UI_DATA_SWITCH_OFF:
            vTaskResume(*ui->nowItem->element->data->dataRootTask);
            break;
        case UI_DATA_SWITCH_ON:
            vTaskSuspend(*ui->nowItem->element->data->dataRootTask);
            break;
        default:
            break;
        }
    }
    #endif
    switch ((UI_DATA_SWITCH_STAT)(*(uint8_t *)ui->nowItem->element->data->ptr))
    {
    case UI_DATA_SWITCH_OFF:
        if(ui->nowItem->element->data->offFunction == NULL)
        {
            UI_LOG_WARN("offFunction is NULL, please check your code.");
            return ;
        }    
        ui->nowItem->element->data->offFunction(); // 执行关闭函数
        break;
    case UI_DATA_SWITCH_ON:
        if(ui->nowItem->element->data->onFunction == NULL)
        {
            UI_LOG_WARN("onFunction is NULL, please check your code.");
            return ;
        }    
        ui->nowItem->element->data->onFunction(); // 执行开启函数
        break;
    default:
        break;
    }
}
