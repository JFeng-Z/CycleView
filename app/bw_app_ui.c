#include "bw_app_ui.h"
#include "ui.h"
#include "ui_conf.h"

ui_t ui;

void BW_UI_Init(void)
{
    MiaoUi_Setup(&ui);
}

void BW_UI_Task(void)
{
    ui_loop(&ui);
}