#include "bw_app_button.h"
#include "bw_bsp_button_handle.h"
#include "multi_button.h"
#include "app_timer.h"

typedef enum
{
    APP_BUTTON_ACTION_NONE,
    APP_BUTTON_ACTION_UP,
    APP_BUTTON_ACTION_DOWN,
    APP_BUTTON_ACTION_ENTER,
} APP_BUTTON_ACTION;

#define BUTTON_UP_NUM       (0)
#define BUTTON_MID_NUM      (1)
#define BUTTON_DOWN_NUM     (2)


/* Global Variable */
static Button button[3];
static APP_BUTTON_ACTION Action;
_APP_TIMER_DEF(m_button_timer);

static void Callback_Button_Top_Handler(void* btn)
{
    Action = APP_BUTTON_ACTION_UP;
}

static void Callback_Button_Mid_Handler(void* btn)
{
    Action = APP_BUTTON_ACTION_ENTER;
}

static void Callback_Button_Down_Handler(void* btn)
{
    Action = APP_BUTTON_ACTION_DOWN;
}

static void Button_Timer_Handler(void * p_context)
{
    button_ticks();
}

void BW_Button_Init(void)
{
    bsp_button_handle_register(BUTTON_UP_NUM);
    bsp_button_handle_register(BUTTON_MID_NUM);
    bsp_button_handle_register(BUTTON_DOWN_NUM);
    bsp_button_handle_init(BUTTON_UP_NUM);
    bsp_button_handle_init(BUTTON_MID_NUM);
    bsp_button_handle_init(BUTTON_DOWN_NUM);

    app_timer_create(&m_button_timer, APP_TIMER_MODE_REPEATED, Button_Timer_Handler);
    app_timer_start(m_button_timer, APP_TIMER_TICKS(5), NULL);

    button_init(&button[0], bsp_button_handle_read_level, 0, 0);
    button_init(&button[1], bsp_button_handle_read_level, 0, 1);
    button_init(&button[2], bsp_button_handle_read_level, 0, 2);

    button_attach(&button[0], BTN_SINGLE_CLICK, Callback_Button_Top_Handler);
    button_attach(&button[0], BTN_LONG_PRESS_HOLD, Callback_Button_Top_Handler);
    button_attach(&button[1], BTN_SINGLE_CLICK, Callback_Button_Mid_Handler);
    button_attach(&button[1], BTN_LONG_PRESS_HOLD, Callback_Button_Mid_Handler);
    button_attach(&button[2], BTN_SINGLE_CLICK, Callback_Button_Down_Handler);
    button_attach(&button[2], BTN_LONG_PRESS_HOLD, Callback_Button_Down_Handler);

    button_start(&button[0]);
    button_start(&button[1]);
    button_start(&button[2]);
}

void BW_Button_DeInit(void)
{
    button_stop(&button[0]);
    button_stop(&button[1]);
    button_stop(&button[2]);

    app_timer_stop(m_button_timer);
    bsp_button_handle_deinit(BUTTON_UP_NUM);
    bsp_button_handle_deinit(BUTTON_MID_NUM);
    bsp_button_handle_deinit(BUTTON_DOWN_NUM);
}

uint8_t app_button_action_read(void)
{
    switch (Action)
    {
    case APP_BUTTON_ACTION_UP:
        Action = APP_BUTTON_ACTION_NONE;
        return APP_BUTTON_ACTION_UP;
    case APP_BUTTON_ACTION_DOWN:
        Action = APP_BUTTON_ACTION_NONE;
        return APP_BUTTON_ACTION_DOWN;
    case APP_BUTTON_ACTION_ENTER:
        Action = APP_BUTTON_ACTION_NONE;
        return APP_BUTTON_ACTION_ENTER;
    default:
        break;
    }
    return APP_BUTTON_ACTION_NONE;
}

