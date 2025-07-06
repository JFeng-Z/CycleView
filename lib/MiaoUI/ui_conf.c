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
#include "ui_conf.h"
#include "ui.h"
#include "dispDriver.h"
#include "image.h"
#include "custom.h"
#include "widget.h"
#include "version.h"
#include "dial_plate.h"
#include "TinyGPSPlus_link.h"
#include "bw_app_gps.h"
#include "bw_app_sdcard.h"
#include "sdcard.h"

/* Page */
ui_page_t Home_Page, System_Page, MPUTask_Page, GPSControl_Page, MicroSD_Page, GPSList_Page, CycleInfo_Page;
/* item */
ui_item_t HomeHead_Item, SystemHead_Item, System_Item, dial_plate_style1_Item, GPSControl_Item, MicroSD_Item, Github_Item, Version_Item, MPUTask_Item, Wave_Item;
ui_item_t MPUTaskHead_Item, MPUTaskLvel_Item, MPUTaskState_Item, SystemHeap_Item, Pitch_Item, Roll_Item, Yaw_Item;
ui_item_t SystemReset_Item, Contrast_Item, Power_Item, MenuColor_Item, CursorAni_Item, StringTest_Item;
ui_item_t GPSControlHead_Item, Satellites_Item, GPSSwitch_Item;
ui_item_t MicroSDHead_Item, SDCard_Item, Format_Item, FileList_Item, SDCardcapacity_Item, SDCardused_space_Item, SDCardfree_space_Item, SDCardproduct_Item;
ui_item_t FileListHead_Item;
ui_item_t CycleInfo_Item, CycleInfoHead_Item, TotalKilometers_Item, ThisKilometers_Item, Speed_Item, Altitude_Item, Latitude_Item, Longitude_Item, RecoardSwitch_Item;
/* Global Variable */
extern GPSData_t gps_data;

/**
 * 在此建立所需显示或更改的数据
 * 无参数
 * 无返回值
 */
void Create_Parameter(ui_t *ui)
{
    static int Contrast = 255;
    static ui_data_t Contrast_data;
    Contrast_data.name = "Contrast";
    Contrast_data.ptr = &Contrast;
    Contrast_data.function = SetContrast;
    Contrast_data.functionType = UI_DATA_FUNCTION_STEP_EXECUTE;
    Contrast_data.dataType = UI_DATA_INT;
    Contrast_data.actionType = UI_DATA_ACTION_RW;
    Contrast_data.max = 255;
    Contrast_data.min = 0;
    Contrast_data.step = 5;
    static ui_element_t Contrast_element;
    Contrast_element.data = &Contrast_data;
    Create_element(&Contrast_Item, &Contrast_element);

    #if ( UI_USE_FREERTOS == 1 )
    extern QueueHandle_t WaveTest_Queue;
    extern TaskHandle_t WaveTestTask_Handle;
    #endif
    extern int Wave_TestData;
    static ui_data_t Wave_data;
    Wave_data.name = "Wave";
    Wave_data.ptr = &Wave_TestData;
    Wave_data.dataType = UI_DATA_INT;
    Wave_data.max = 255;
    Wave_data.min = 0;
    #if ( UI_USE_FREERTOS == 1 )
    Wave_data.dataRootTask = &WaveTestTask_Handle;
    Wave_data.dataRootQueue = &WaveTest_Queue;
    #endif
    static ui_element_t Wave_element;
    Wave_element.data = &Wave_data;
    Create_element(&Wave_Item, &Wave_element);

    #if ( UI_USE_FREERTOS == 1 )
    extern TaskHandle_t MPU6050UpdateTask_Handle;
    static uint8_t PrintTask_State = true;
    static ui_data_t PrintTask_switch_data;
    PrintTask_switch_data.ptr = &PrintTask_State;
    PrintTask_switch_data.dataType = UI_DATA_SWITCH;
    PrintTask_switch_data.actionType = UI_DATA_ACTION_RW;
    PrintTask_switch_data.dataRootTask = &MPU6050UpdateTask_Handle;
    static ui_element_t PrintTask_element;
    PrintTask_element.data = &PrintTask_switch_data;
    Create_element(&MPUTaskState_Item, &PrintTask_element);

    extern SemaphoreHandle_t MPU6050Update_Mutex;
    static int PrintTask_Levl = 1;
    static ui_data_t PrintTask_Levl_data;
    PrintTask_Levl_data.actionType = UI_DATA_ACTION_RW;
    PrintTask_Levl_data.dataType = UI_DATA_INT;
    PrintTask_Levl_data.name = "PrintTask_Levl";
    PrintTask_Levl_data.ptr = &PrintTask_Levl;
    PrintTask_Levl_data.max = 10;
    PrintTask_Levl_data.min = 1;
    PrintTask_Levl_data.step = 1;
    PrintTask_Levl_data.functionType = UI_DATA_FUNCTION_EXIT_EXECUTE;
    PrintTask_Levl_data.function = TaskLvel_Setting;
    PrintTask_Levl_data.dataRootTask = &MPU6050UpdateTask_Handle;
    PrintTask_Levl_data.dataRootMutex = &MPU6050Update_Mutex;
    static ui_element_t PrintTask_Levl_element;
    PrintTask_Levl_element.data = &PrintTask_Levl_data;
    Create_element(&MPUTaskLvel_Item, &PrintTask_Levl_element);

    extern char FreeHeap[20];
    static ui_data_t SystemHeap_data;
    SystemHeap_data.name = "FreeHeap";
    SystemHeap_data.ptr = FreeHeap;
    SystemHeap_data.dataType = UI_DATA_STRING;
    SystemHeap_data.actionType = UI_DATA_ACTION_RO;
    static ui_element_t SystemHeap_element;
    SystemHeap_element.data = &SystemHeap_data;
    Create_element(&SystemHeap_Item, &SystemHeap_element);
    #endif

    static ui_data_t MenuColor_data;
    MenuColor_data.name = "MenuColor";
    MenuColor_data.ptr = &ui->bgColor;
    MenuColor_data.dataType = UI_DATA_SWITCH;
    MenuColor_data.actionType = UI_DATA_ACTION_RW;
    static ui_element_t MenuColor_element;
    MenuColor_element.data = &MenuColor_data;
    Create_element(&MenuColor_Item, &MenuColor_element);

    extern uint8_t gps_state;
    static ui_data_t GPSSwitch_data;
    GPSSwitch_data.name = "GPS Switch";
    GPSSwitch_data.ptr = &gps_state;
    GPSSwitch_data.offFunction = BW_GPS_DeInit;
    GPSSwitch_data.onFunction = BW_GPS_Init;
    GPSSwitch_data.dataType = UI_DATA_SWITCH;
    GPSSwitch_data.actionType = UI_DATA_ACTION_RW;
    static ui_element_t GPSSwitch_element;
    GPSSwitch_element.data = &GPSSwitch_data;
    Create_element(&GPSSwitch_Item, &GPSSwitch_element);

    static ui_data_t CursorAni_data;
    CursorAni_data.name = "CursorAni";
    CursorAni_data.ptr = &ui->animation.cursor_ani.kp;
    CursorAni_data.dataType = UI_DATA_FLOAT;
    CursorAni_data.actionType = UI_DATA_ACTION_RW;
    CursorAni_data.min = 0;
    CursorAni_data.max = 10;
    CursorAni_data.step = 0.1;
    static ui_element_t CursorAni_element;
    CursorAni_element.data = &CursorAni_data;
    Create_element(&CursorAni_Item, &CursorAni_element);

    float Pitch, Roll, Yaw;
    static ui_data_t Pitch_data;
    Pitch_data.name = "Pitch";
    Pitch_data.ptr = &Pitch;
    Pitch_data.dataType = UI_DATA_FLOAT;
    Pitch_data.actionType = UI_DATA_ACTION_RO;
    Pitch_data.max = 360;
    Pitch_data.min = -360;
    static ui_element_t Pitch_element;
    Pitch_element.data = &Pitch_data;
    Create_element(&Pitch_Item, &Pitch_element);

    static ui_data_t Roll_data;
    Roll_data.name = "Roll";
    Roll_data.ptr = &Roll;
    Roll_data.dataType = UI_DATA_FLOAT;
    Roll_data.actionType = UI_DATA_ACTION_RO;
    Roll_data.max = 360;
    Roll_data.min = -360;
    static ui_element_t Roll_element;
    Roll_element.data = &Roll_data;
    Create_element(&Roll_Item, &Roll_element);

    static ui_data_t Yaw_data;
    Yaw_data.name = "Yaw";
    Yaw_data.ptr = &Yaw;
    Yaw_data.dataType = UI_DATA_FLOAT;
    Yaw_data.actionType = UI_DATA_ACTION_RO;
    Yaw_data.max = 360;
    Yaw_data.min = -360;
    static ui_element_t Yaw_element;
    Yaw_element.data = &Yaw_data;
    Create_element(&Yaw_Item, &Yaw_element);

    extern uint32_t Satellites;
    static ui_data_t Satellites_Data;
    Satellites_Data.name = "Satellites";
    Satellites_Data.ptr = &Satellites;
    Satellites_Data.dataType = UI_DATA_INT;
    Satellites_Data.actionType = UI_DATA_ACTION_RO;
    Satellites_Data.max = 100;
    Satellites_Data.min = 0;
    static ui_element_t Satellites_element;
    Satellites_element.data = &Satellites_Data;
    Create_element(&Satellites_Item, &Satellites_element);

    extern uint8_t SD_Card_state;
    static ui_data_t SD_Card_state_data;
    SD_Card_state_data.ptr = &SD_Card_state;
    SD_Card_state_data.offFunction = BW_SDCard_DeInit;
    SD_Card_state_data.onFunction = BW_SDCard_Init;
    SD_Card_state_data.dataType = UI_DATA_SWITCH;
    SD_Card_state_data.actionType = UI_DATA_ACTION_RO;
    static ui_element_t SD_Card_state_element;
    SD_Card_state_element.data = &SD_Card_state_data;
    Create_element(&SDCard_Item, &SD_Card_state_element);

    extern float total_kilometers;
    static ui_data_t TotalKilometers_data;
    TotalKilometers_data.name = "Total Kilometers";
    TotalKilometers_data.ptr = &total_kilometers;
    TotalKilometers_data.dataType = UI_DATA_FLOAT;
    TotalKilometers_data.max = 99999;
    TotalKilometers_data.min = 0;
    TotalKilometers_data.actionType = UI_DATA_ACTION_RO;
    static ui_element_t TotalKilometers_element;
    TotalKilometers_element.data = &TotalKilometers_data;
    Create_element(&TotalKilometers_Item, &TotalKilometers_element);

    extern float this_kilometers;
    static ui_data_t ThisKilometers_data;
    ThisKilometers_data.name = "This Kilometers";
    ThisKilometers_data.ptr = &this_kilometers;
    ThisKilometers_data.max = 99999;
    ThisKilometers_data.min = 0;
    ThisKilometers_data.dataType = UI_DATA_FLOAT;
    ThisKilometers_data.actionType = UI_DATA_ACTION_RO;
    static ui_element_t ThisKilometers_element;
    ThisKilometers_element.data =&ThisKilometers_data;
    Create_element(&ThisKilometers_Item, &ThisKilometers_element);

    extern float Speed, Altitude, Longitude, Latitude;
    static ui_data_t Speed_data;
    Speed_data.name = "Speed";
    Speed_data.ptr = &Speed;
    Speed_data.max = 999;
    Speed_data.min = 0;
    Speed_data.dataType = UI_DATA_FLOAT;
    Speed_data.actionType = UI_DATA_ACTION_RO;
    static ui_element_t Speed_element;
    Speed_element.data =&Speed_data;
    Create_element(&Speed_Item, &Speed_element);

    static ui_data_t Altitude_data;
    Altitude_data.name = "Altitude";
    Altitude_data.ptr = &Altitude;
    Altitude_data.max = 9999;
    Altitude_data.min = 0;
    Altitude_data.dataType = UI_DATA_FLOAT;
    Altitude_data.actionType = UI_DATA_ACTION_RO;
    static ui_element_t Altitude_element;
    Altitude_element.data =&Altitude_data;
    Create_element(&Altitude_Item, &Altitude_element);

    static ui_data_t Latitude_data;
    Latitude_data.name = "Latitude";
    Latitude_data.ptr = &Latitude;
    Latitude_data.max = 90;
    Latitude_data.min = 0;
    Latitude_data.dataType = UI_DATA_FLOAT;
    Latitude_data.actionType = UI_DATA_ACTION_RO;
    static ui_element_t Latitude_element;
    Latitude_element.data =&Latitude_data;
    Create_element(&Latitude_Item, &Latitude_element);

    static ui_data_t Longitude_data;
    Longitude_data.name = "Longitude";
    Longitude_data.ptr = &Longitude;
    Longitude_data.max = 180;
    Longitude_data.min = 0;
    Longitude_data.dataType = UI_DATA_FLOAT;
    Longitude_data.actionType = UI_DATA_ACTION_RO;
    static ui_element_t Longitude_element;
    Longitude_element.data =&Longitude_data;
    Create_element(&Longitude_Item, &Longitude_element);

    static char StringTest[] = "192.168.1.11";
    static ui_data_t StringTest_data;
    StringTest_data.name = "IP Address";
    StringTest_data.ptr = StringTest;
    StringTest_data.dataType = UI_DATA_STRING;
    StringTest_data.actionType = UI_DATA_ACTION_RW;
    static ui_element_t StringTest_element;
    StringTest_element.data = &StringTest_data;
    Create_element(&StringTest_Item, &StringTest_element);

    extern sdcard_info_t sdcard_info;
    
    static ui_data_t SDCardcapacity_data;
    SDCardcapacity_data.name = "SDCard Capacity";
    SDCardcapacity_data.ptr = &sdcard_info.capacity;
    SDCardcapacity_data.dataType = UI_DATA_INT;
    SDCardcapacity_data.actionType = UI_DATA_ACTION_RO;
    SDCardcapacity_data.max = 9999999; // 最大容量
    SDCardcapacity_data.min = 0; // 最小容量
    static ui_element_t SDCardcapacity_element;
    SDCardcapacity_element.data = &SDCardcapacity_data;
    Create_element(&SDCardcapacity_Item, &SDCardcapacity_element);

    static ui_data_t SDCardused_space_data;
    SDCardused_space_data.name = "SDCard Used Space";
    SDCardused_space_data.ptr = &sdcard_info.used_space;
    SDCardused_space_data.dataType = UI_DATA_INT;
    SDCardused_space_data.actionType = UI_DATA_ACTION_RO;
    SDCardused_space_data.max = 9999999; // 最大已用空间
    SDCardused_space_data.min = 0; // 最小已用空间
    static ui_element_t SDCardused_space_element;
    SDCardused_space_element.data = &SDCardused_space_data;
    Create_element(&SDCardused_space_Item, &SDCardused_space_element);

    static ui_data_t SDCardfree_space_data;
    SDCardfree_space_data.name = "SDCard Free Space";
    SDCardfree_space_data.ptr = &sdcard_info.free_space;
    SDCardfree_space_data.dataType = UI_DATA_INT;
    SDCardfree_space_data.actionType = UI_DATA_ACTION_RO;
    SDCardfree_space_data.max = 9999999; // 最大可用空间
    SDCardfree_space_data.min = 0; // 最小可用空间
    static ui_element_t SDCardfree_space_element;
    SDCardfree_space_element.data = &SDCardfree_space_data;
    Create_element(&SDCardfree_space_Item, &SDCardfree_space_element);

    static ui_data_t SDCardproduct_data;
    SDCardproduct_data.name = "SDCard Product";
    SDCardproduct_data.ptr = sdcard_info.product;
    SDCardproduct_data.dataType = UI_DATA_STRING;
    SDCardproduct_data.actionType = UI_DATA_ACTION_RO;
    static ui_element_t SDCardproduct_element;
    SDCardproduct_element.data = &SDCardproduct_data;
    Create_element(&SDCardproduct_Item, &SDCardproduct_element);

    static uint8_t recoard_switch = false;
    static ui_data_t RecoardSwitch_data;
    RecoardSwitch_data.name = "Recoard Switch";
    RecoardSwitch_data.ptr = &recoard_switch;
    RecoardSwitch_data.dataType = UI_DATA_SWITCH;
    RecoardSwitch_data.actionType = UI_DATA_ACTION_RW;
    RecoardSwitch_data.offFunction = GPX_File_Recoard_Stop;
    RecoardSwitch_data.onFunction = GPX_File_Recoard_Start;
    static ui_element_t RecoardSwitch_element;
    RecoardSwitch_element.data = &RecoardSwitch_data;
    Create_element(&RecoardSwitch_Item, &RecoardSwitch_element);
}

/**
 * 在此建立所需显示或更改的文本
 * 无参数
 * 无返回值
 */
void Create_Text(ui_t *ui)
{
    static ui_text_t github_text;
    github_text.font = ui->font.ptr;
    github_text.fontHight = UI_FONT_HIGHT;
    github_text.fontWidth = UI_FONT_WIDTH;
    github_text.ptr = VERSION_PROJECT_LINK;
    static ui_element_t github_element;
    github_element.text = &github_text;
    Create_element(&Github_Item, &github_element);
}

/* micro item */
ui_micro_item_t gpx_file_micro_item[3];
const char *Cancel_text = "关闭";
const char *Delete_text = "删除";
const char *Parse_text = "解析";
ui_piece_t Cancel_piece = {.box_x = 20, .box_y = 25};
ui_piece_t Delete_piece = {.box_x = 54, .box_y = 25};
ui_piece_t Parse_piece = {.box_x = 88, .box_y = 25};

void Create_GPX_Item(char *item_name)
{
    ui_item_pt t_item;
    t_item = Dynamic_AddItem(item_name, UI_ITEM_LOOP_FUNCTION, NULL, &GPSList_Page, NULL, Micro_Action_Widget, NULL);
    Create_micro_item(Cancel_text, t_item, &gpx_file_micro_item[0], Cancel, NULL, &Cancel_piece);
    Create_micro_item(Delete_text, t_item, &gpx_file_micro_item[1], file_delete, NULL, &Delete_piece);
    Create_micro_item(Parse_text, t_item, &gpx_file_micro_item[2], file_parse, NULL, &Parse_piece);
}

/*
 * 菜单构建函数
 * 该函数不接受参数，也不返回任何值。
 * 功能：静态地构建一个菜单系统。
 */
void Create_MenuTree(ui_t *ui)
{
    AddPage("[主页]", &Home_Page, UI_PAGE_ICON, NULL);
        AddItem("[主页]", UI_ITEM_ONCE_FUNCTION, logo_allArray[0], &HomeHead_Item, &Home_Page, NULL, Draw_Home, NULL);
        AddItem(" +系统", UI_ITEM_PARENTS, logo_allArray[1], &System_Item, &Home_Page, &System_Page, NULL, NULL);
            AddPage("[系统]", &System_Page, UI_PAGE_TEXT, &Home_Page);
                AddItem("[系统]", UI_ITEM_RETURN, NULL, &SystemHead_Item, &System_Page, &Home_Page, NULL, NULL);
                AddItem(" -复位", UI_ITEM_ONCE_FUNCTION, NULL, &SystemReset_Item, &System_Page, NULL, system_reset, NULL);
                AddItem(" -对比度", UI_ITEM_DATA, NULL, &Contrast_Item, &System_Page, NULL, ParameterSetting_Widget_Style1, NULL);
                AddItem(" -息屏", UI_ITEM_LOOP_FUNCTION, NULL, &Power_Item, &System_Page, NULL, SetPowerSave, NULL);   
                AddItem(" -菜单颜色", UI_ITEM_DATA, NULL, &MenuColor_Item, &System_Page, NULL, NULL, NULL);  
                AddItem(" -光标动画", UI_ITEM_DATA, NULL, &CursorAni_Item, &System_Page, NULL, NULL, NULL);
                AddItem(" -文本测试", UI_ITEM_DATA, NULL, &StringTest_Item, &System_Page, NULL, NULL, NULL);
                #if ( UI_USE_FREERTOS == 1 )     
                AddItem(" -UI Stack", UI_ITEM_DATA, NULL, &SystemHeap_Item, &System_Page, NULL, NULL, NULL);
                #endif
                AddItem(" -IMU Task", UI_ITEM_PARENTS, NULL, &MPUTask_Item, &System_Page, &MPUTask_Page, NULL, NULL);
                    AddPage("[IMU Task]", &MPUTask_Page, UI_PAGE_TEXT, &System_Page);
                        AddItem("[IMU Task]", UI_ITEM_RETURN, NULL, &MPUTaskHead_Item, &MPUTask_Page, &System_Page, NULL, NULL);
                        #if ( UI_USE_FREERTOS == 1 )  
                        AddItem(" -Lvel", UI_ITEM_DATA, NULL, &MPUTaskLvel_Item, &MPUTask_Page, NULL, NULL, NULL);
                        AddItem(" -State", UI_ITEM_DATA, NULL, &MPUTaskState_Item, &MPUTask_Page, NULL, NULL, NULL);
                        #endif
                        AddItem(" -Pitch", UI_ITEM_DATA, NULL, &Pitch_Item, &MPUTask_Page, NULL, NULL, NULL);
                        AddItem(" -Roll", UI_ITEM_DATA, NULL, &Roll_Item, &MPUTask_Page, NULL, NULL, NULL);
                        AddItem(" -Yaw", UI_ITEM_DATA, NULL, &Yaw_Item, &MPUTask_Page, NULL, NULL, NULL);
        AddItem(" -GPS控制", UI_ITEM_PARENTS, logo_allArray[8], &GPSControl_Item, &Home_Page, &GPSControl_Page, NULL, NULL);
            AddPage("[GPS 控制]", &GPSControl_Page, UI_PAGE_TEXT, &Home_Page);
                AddItem("[GPS控制]", UI_ITEM_RETURN, NULL, &GPSControlHead_Item, &GPSControl_Page, &Home_Page, NULL, NULL);
                AddItem(" -GPS状态", UI_ITEM_DATA, NULL, &GPSSwitch_Item, &GPSControl_Page, NULL, NULL, NULL);  
                AddItem(" -卫星数", UI_ITEM_DATA, NULL, &Satellites_Item, &GPSControl_Page, NULL, NULL, NULL);
        AddItem(" -内存设备", UI_ITEM_PARENTS, logo_allArray[9], &MicroSD_Item, &Home_Page, &MicroSD_Page, NULL, NULL);
            AddPage("[内存设备]", &MicroSD_Page, UI_PAGE_TEXT, &Home_Page);
                AddItem("[内存设备]", UI_ITEM_RETURN, NULL, &MicroSDHead_Item, &MicroSD_Page, &Home_Page, NULL, NULL);
                AddItem(" -内存设备状态", UI_ITEM_DATA, NULL, &SDCard_Item, &MicroSD_Page, NULL, NULL, NULL);
                AddItem(" -设备类型", UI_ITEM_DATA, NULL, &SDCardproduct_Item, &MicroSD_Page, NULL, NULL, NULL);
                AddItem(" -总大小", UI_ITEM_DATA, NULL, &SDCardcapacity_Item, &MicroSD_Page, NULL, NULL, NULL);
                AddItem(" -已用空间", UI_ITEM_DATA, NULL, &SDCardused_space_Item, &MicroSD_Page, NULL, NULL, NULL);
                AddItem(" -可用空间", UI_ITEM_DATA, NULL, &SDCardfree_space_Item, &MicroSD_Page, NULL, NULL, NULL);
                AddItem(" -格式化", UI_ITEM_ONCE_FUNCTION, NULL, &Format_Item, &MicroSD_Page, NULL, sdcard_format, NULL);
                AddItem(" -GPX文件夹", UI_ITEM_PARENTS, NULL, &FileList_Item, &MicroSD_Page, &GPSList_Page, NULL, NULL);
                    AddPage("[GPX文件夹]", &GPSList_Page, UI_PAGE_TEXT, &MicroSD_Page);
                        AddItem("[GPX文件夹]", UI_ITEM_RETURN, NULL, &FileListHead_Item, &GPSList_Page, &MicroSD_Page, NULL, NULL);
        AddItem(" -表盘", UI_ITEM_LOOP_FUNCTION, logo_allArray[7], &dial_plate_style1_Item, &Home_Page, NULL, dial_plate_style1, &gps_data);
        AddItem(" -骑行数据", UI_ITEM_PARENTS, logo_allArray[10], &CycleInfo_Item, &Home_Page, &CycleInfo_Page, NULL, NULL);
            AddPage("[骑行数据]", &CycleInfo_Page, UI_PAGE_TEXT, &Home_Page);
                AddItem("[骑行数据]", UI_ITEM_RETURN, NULL, &CycleInfoHead_Item, &CycleInfo_Page, &Home_Page, NULL, NULL);
                AddItem(" -数据记录", UI_ITEM_DATA, NULL, &RecoardSwitch_Item, &CycleInfo_Page, &Home_Page, NULL, NULL);
                AddItem(" -总公里数", UI_ITEM_DATA, NULL, &TotalKilometers_Item, &CycleInfo_Page, &Home_Page, NULL, NULL);
                AddItem(" -本次公里数", UI_ITEM_DATA, NULL, &ThisKilometers_Item, &CycleInfo_Page, &Home_Page, NULL, NULL);
                AddItem(" -速度", UI_ITEM_DATA, NULL, &Speed_Item, &CycleInfo_Page, &Home_Page, NULL, NULL);
                AddItem(" -海拔", UI_ITEM_DATA, NULL, &Altitude_Item, &CycleInfo_Page, &Home_Page, NULL, NULL);
                AddItem(" -纬度", UI_ITEM_DATA, NULL, &Latitude_Item, &CycleInfo_Page, &Home_Page, NULL, NULL);
                AddItem(" -经度", UI_ITEM_DATA, NULL, &Longitude_Item, &CycleInfo_Page, &Home_Page, NULL, NULL);
        AddItem(" -Github", UI_ITEM_WORD, logo_allArray[3], &Github_Item, &Home_Page, NULL, NULL, NULL);
        AddItem(" -版本", UI_ITEM_ONCE_FUNCTION, logo_allArray[5], &Version_Item, &Home_Page, NULL, Show_Version, NULL);
        AddItem(" -波形", UI_ITEM_WAVE, logo_allArray[6], &Wave_Item, &Home_Page, NULL, NULL, NULL);
}

void MiaoUi_Setup(ui_t *ui)
{
    #if ( DYNAMIC_MANAGE == 1 )
    mem_init();
    #endif
    #if(UI_USE_FATFS == 1)
    #include "fileDriver.h"
    fileDriverInit();
    #endif
    Create_UI(ui, &HomeHead_Item); // 创建UI, 必须给定一个头项目
    #if(UI_USE_FATFS == 1)
    fileDriverDeInit();
    #endif
    Draw_Home(ui);
}
