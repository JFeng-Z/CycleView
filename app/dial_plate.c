#include "dial_plate.h"
#include "stdlib.h"
#include "ui.h"
#include "dispDriver.h"
#include "fonts.h"
#include "TinyGPSPlus_link.h"
#include "dial_plate_icon.h"
#include "bw_app_gps.h"
#include "bw_app_battery.h"
#include "bw_app_sdcard.h"
#include "time.h"

extern struct tm tm;

char dial_plate_buffer[24] = {0};

inline void dial_plate_time(GPSData_t *gpsData)
{
    snprintf(dial_plate_buffer, sizeof(dial_plate_buffer), "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    Disp_SetFont(Arial_Italic_12_custom.ptr);
    Disp_DrawStr(15, 13, dial_plate_buffer);
}

inline void dial_plate_ble(void)
{
    Disp_DrawXBMP(110, 2, 10, 14, (uint8_t *)epd_bitmap_buletooth);
}

inline void dial_plate_lightning(void)
{
    Disp_DrawXBMP(15, 22, 16, 22, (uint8_t *)epd_bitmap_lightning);
}

inline void dial_plate_battery(void)
{
    extern uint8_t battery_level;
    Disp_DrawXBMP(86, 53, 16, 10, (uint8_t *)epd_bitmap_battery_full);
    Disp_SetFont(Arial_Bold_Italic_12_custom.ptr);
    snprintf(dial_plate_buffer, sizeof(dial_plate_buffer), "%d%%", battery_level);
    Disp_DrawStr(102, 62, dial_plate_buffer);
}

inline void dial_plate_speed(GPSData_t *gpsData)
{
    snprintf(dial_plate_buffer, sizeof(dial_plate_buffer), "%.2fKm/h", gpsData->speed.kmph);
    Disp_SetFont(Arial_Bold_Italic_18_custom.ptr);
    Disp_DrawStr(34, 23+18, dial_plate_buffer);
}

inline void dial_plate_trp(GPSData_t *gpsData)
{
    extern float total_kilometers;
    snprintf(dial_plate_buffer, sizeof(dial_plate_buffer), "%.2fkm", total_kilometers);
    Disp_SetFont(Arial_Bold_Italic_12_custom.ptr);
    Disp_SetFontDirection(1);
    Disp_DrawStr(0, 12, dial_plate_buffer);
    Disp_SetFontDirection(0);
}

inline void dial_plate_North(GPSData_t *gpsData)
{
    snprintf(dial_plate_buffer, sizeof(dial_plate_buffer), "N%.4f°", gpsData->location.lat);
    Disp_SetFont(Arial_Italic_12_custom.ptr);
    Disp_SetFontDirection(1);
    Disp_DrawStr(0, 0, dial_plate_buffer);
    Disp_SetFontDirection(0);
}

inline void dial_plate_east(GPSData_t *gpsData)
{
    snprintf(dial_plate_buffer, sizeof(dial_plate_buffer), "E%.4f°", gpsData->location.lng);
    Disp_SetFont(Arial_Italic_12_custom.ptr);
    Disp_SetFontDirection(1);
    Disp_DrawStr(116, 0, dial_plate_buffer);
    Disp_SetFontDirection(0);
}

inline void dial_plate_slope(void)
{
    extern int8_t slope;
    snprintf(dial_plate_buffer, sizeof(dial_plate_buffer), "%d%%", slope);
    Disp_SetFont(Arial_Bold_Italic_12_custom.ptr);
    Disp_DrawStr(80, 13, dial_plate_buffer);
}

inline void dial_plate_heart_rate(void)
{
    extern uint16_t heart_rate;
    snprintf(dial_plate_buffer, sizeof(dial_plate_buffer), "%d", heart_rate);
    Disp_SetFont(Arial_Bold_Italic_12_custom.ptr);
    Disp_DrawStr(24, 63, dial_plate_buffer);
    Disp_DrawXBMP(12, 52, 12, 11, (uint8_t *)epd_bitmap_favourite_stroke_rounded);
}

inline void dial_plate_cadence(void)
{
    extern uint8_t cadence;
    snprintf(dial_plate_buffer, sizeof(dial_plate_buffer), "%d", cadence);
    Disp_SetFont(Arial_Bold_Italic_12_custom.ptr);
    Disp_DrawStr(60, 63, dial_plate_buffer);
    Disp_DrawXBMP(48, 52, 12, 11, (uint8_t *)epd_bitmap_atom_01_stroke_rounded);
}

void dial_plate_style1(ui_t *ui)
{
    // static bool flag = false;
    // if(flag == false)
    // {
    //     BW_SDCard_Init();
    //     BW_GPS_Init();
    //     Begin_Recoard();
    //     gps_data_check_timer_init();
    //     flag = true;
    // }
    Disp_ClearBuffer();
    dial_plate_time((GPSData_t *)ui->nowItem->data);
    dial_plate_ble();
    dial_plate_lightning();
    dial_plate_battery();
    dial_plate_slope();
    dial_plate_speed((GPSData_t *)ui->nowItem->data);
    // dial_plate_North((GPSData_t *)ui->nowItem->data);
    // dial_plate_east((GPSData_t *)ui->nowItem->data);
    dial_plate_trp((GPSData_t *)ui->nowItem->data);
    dial_plate_heart_rate();
    dial_plate_cadence();
    Disp_SendBuffer();
    // write_gpx();
    // if(ui->action == UI_ACTION_ENTER)
    // {
        // if(End_Recoard() == 0)
        // Create_GPX_File_Item();
        // gps_data_check_timer_deinit();
        // BW_GPS_DeInit();
        // BW_SDCard_DeInit();
        // flag = false;
    // }
}

// void dial_plate_style2(ui_t *ui)
// {
//     static bool flag = false;
//     if(flag == false)
//     {
//         BW_SDCard_Init();
//         BW_Battery_Init();
//         BW_GPS_Init();
//         Begin_Recoard(ui);
//         gps_data_check_timer_init();
//         flag = true;
//     }
//     Disp_ClearBuffer();
//     dial_plate_time((GPSData_t *)ui->nowItem->data);
//     dial_plate_ble();
//     dial_plate_lightning();
//     dial_plate_battery();
//     dial_plate_slope();
//     dial_plate_speed((GPSData_t *)ui->nowItem->data);
//     dial_plate_North((GPSData_t *)ui->nowItem->data);
//     dial_plate_east((GPSData_t *)ui->nowItem->data);
//     dial_plate_trp((GPSData_t *)ui->nowItem->data);
//     Disp_SendBuffer();
//     write_gpx(ui);
//     if(ui->action == UI_ACTION_ENTER)
//     {
//         if(End_Recoard(ui) == 0)
//         Create_GPX_File_Item();
//         gps_data_check_timer_deinit();
//         BW_Battery_DeInit();
//         BW_GPS_DeInit();
//         BW_SDCard_DeInit();
//         flag = false;
//     }
// }

// void Draw_Home(ui_t *ui)
// {
    // int16_t y = 128;
    // uint8_t state = 0;
    // uint8_t color = 1;
    // while (1)
    // {
    //     if(indevScan() == UI_ACTION_ENTER)return;

    //     Disp_ClearBuffer();
    //     Disp_SetDrawColor(&color);

    //     switch (state)
    //     {
    //     case 0:
    //         ui->dialog.nowWide = (uint16_t)easeInOutCirc(ui->dialog.times, 0, 30, 20);
    //         ui->dialog.times++;
    //         if(ui->dialog.times == 20)
    //         {
    //             state = 1;
    //         }
    //         break;
    //     case 1:
    //         ui->dialog.times = 0;
    //         y = (int16_t )UI_Animation(26.0, (float )y, &ui->animation.textPage_ani);
    //         Disp_DrawXBMP(34, y, 57, 14, UI_NAME_LOGO);
    //         if(y == 26)
    //         {
    //             state = 2;
    //             y = 0;
    //         }
    //         break;
    //     case 2:
    //         y = (int16_t )UI_Animation(14.0, (float )y, &ui->animation.textPage_ani);
    //         Disp_DrawXBMP(34, 26, 57, 14, UI_NAME_LOGO);
    //         Disp_SetFont(font_home_h6w4.ptr);
    //         Disp_DrawStr(y, 58, VERSION_PROJECT_LINK);
    //         if(y == 12)
    //         {
    //             state = 3;
    //             y = 128;
    //         }
    //         break;
    //     case 3:
    //         y = (int16_t )UI_Animation(0.0, (float )y, &ui->animation.textPage_ani);
    //         Disp_DrawXBMP(34, 26, 57, 14, UI_NAME_LOGO);
    //         Disp_SetFont(font_home_h6w4.ptr);
    //         Disp_DrawStr(14, 58, VERSION_PROJECT_LINK);
    //         Disp_SetFont(UI_FONT);
    //         Disp_DrawStr(y, 16, "Version:");
    //         if(y == 0)
    //         {
    //             state = 4;
    //             y = 128;
    //         }
    //         break;
    //     case 4:
    //         y = (int16_t )UI_Animation(54.0, (float )y, &ui->animation.textPage_ani);
    //         Disp_DrawXBMP(34, 26, 57, 14, UI_NAME_LOGO);
    //         Disp_SetFont(font_home_h6w4.ptr);
    //         Disp_DrawStr(14, 58, VERSION_PROJECT_LINK);
    //         Disp_SetFont(UI_FONT);
    //         Disp_DrawStr(0, 16, "Version:");
    //         Disp_DrawStr(y, 16, "1");
    //         if(y == 54)
    //         {
    //             state = 5;
    //             y = 128;
    //         }
    //         break;
    //     case 5:
    //         y = (int16_t )UI_Animation(54.0, (float )y, &ui->animation.textPage_ani);
    //         Disp_DrawXBMP(34, 26, 57, 14, UI_NAME_LOGO);
    //         Disp_SetFont(font_home_h6w4.ptr);
    //         Disp_DrawStr(14, 58, VERSION_PROJECT_LINK);
    //         Disp_SetFont(UI_FONT);
    //         Disp_DrawStr(0, 16, "Version:");
    //         Disp_DrawStr(54, 16, "1");
    //         Disp_DrawStr(y+6, 16, ".");
    //         if(y == 54)
    //         {
    //             state = 6;
    //             y = 128;
    //         }
    //         break;
    //     case 6:
    //         y = (int16_t )UI_Animation(54.0, (float )y, &ui->animation.textPage_ani);
    //         Disp_DrawXBMP(34, 26, 57, 14, UI_NAME_LOGO);
    //         Disp_SetFont(font_home_h6w4.ptr);
    //         Disp_DrawStr(14, 58, VERSION_PROJECT_LINK);
    //         Disp_SetFont(UI_FONT);
    //         Disp_DrawStr(0, 16, "Version:");
    //         Disp_DrawStr(54, 16, "1");
    //         Disp_DrawStr(60, 16, ".");
    //         Disp_DrawStr(y+12, 16, "3");
    //         if(y == 54)
    //         {
    //             state = 7;
    //         }
    //         break;
    //     case 7:
    //         return;
    //     default:
    //         break;
    //     }
    //     Disp_DrawLine(64, 45, 64 - ui->dialog.nowWide, 45);
    //     Disp_DrawLine(64, 45, 64 + ui->dialog.nowWide, 45);

    //     Disp_SendBuffer();
    // }
// }