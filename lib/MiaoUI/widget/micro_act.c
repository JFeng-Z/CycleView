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
#include "dispDriver.h"
#include "ui.h"
#include "ui_logger.h"

typedef enum
{
    SELECT_STAT_NONE,
    SELECT_STAT_SELECTED,
    SELECT_STAT_UNSELECTED
}SELECT_STAT;

void Micro_Action_Widget(ui_t *ui)
{
    static SELECT_STAT select_stat = SELECT_STAT_NONE;
    static ui_micro_item_pt select_micro_item;
    static int16_t x, y, w, h;
    ui_micro_item_pt t_micro_item = ui->nowItem->micro_item;
    uint8_t color = 2;
    
    switch (select_stat)
    {
    case SELECT_STAT_NONE:
        select_stat = SELECT_STAT_UNSELECTED;
        break;
    case SELECT_STAT_UNSELECTED:
        if(select_micro_item == NULL)select_micro_item = ui->nowItem->micro_item;
        Disp_ClearBuffer();

        while(t_micro_item->next != ui->nowItem->micro_item)
        {
            Draw_Piece(ui->font, t_micro_item->name, t_micro_item->piece);
            if(t_micro_item->next == NULL)UI_LOG_ERROR("micro_item->next = NULL");
            else t_micro_item = t_micro_item->next;
        }
        Draw_Piece(ui->font, t_micro_item->name, t_micro_item->piece);
        x = UI_Animation(select_micro_item->piece->box_x, x, &ui->animation.textPage_ani);
        y = UI_Animation(select_micro_item->piece->box_y, y, &ui->animation.textPage_ani);
        w = UI_Animation(select_micro_item->piece->box_w, w, &ui->animation.textPage_ani);
        h = UI_Animation(select_micro_item->piece->box_h, h, &ui->animation.textPage_ani);
        Disp_SetDrawColor(&color);
        Disp_DrawRBox(x, y, w, h, 1);
        switch (ui->action)
        {
        case UI_ACTION_UP:
            select_micro_item = select_micro_item->prev;
            break;
        case UI_ACTION_DOWN:
            select_micro_item = select_micro_item->next;
            break;
        case UI_ACTION_ENTER:
            select_stat = SELECT_STAT_SELECTED;
            ui->action = UI_ACTION_NONE;
            break;
        default:
            break;
        }
        break;
    case SELECT_STAT_SELECTED:
        if(select_micro_item->function != NULL)
        {
            if((select_micro_item->function)(ui))
            {
                select_stat = SELECT_STAT_NONE;
                select_micro_item = NULL;
                ui->action = UI_ACTION_ENTER;
            }
        }
        else 
        {
            select_stat = SELECT_STAT_NONE;
            select_micro_item = NULL;
            ui->action = UI_ACTION_ENTER;
        }
        break;
    default:
        break;
    }

    Disp_SendBuffer();
}
