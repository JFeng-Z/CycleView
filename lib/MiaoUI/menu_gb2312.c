#include "menu_gb2312.h"

/* Page */
extern ui_page_t Home_Page, System_Page, MPUTask_Page, GPSControl_Page, MicroSD_Page, GPSList_Page, CycleInfo_Page;
/* item */
extern ui_item_t HomeHead_Item, SystemHead_Item, System_Item, dial_plate_style1_Item, GPSControl_Item, MicroSD_Item, Github_Item, Version_Item, MPUTask_Item, Wave_Item;
extern ui_item_t MPUTaskHead_Item, MPUTaskLvel_Item, MPUTaskState_Item, SystemHeap_Item, Pitch_Item, Roll_Item, Yaw_Item;
extern ui_item_t SystemReset_Item, Contrast_Item, Power_Item, MenuColor_Item, CursorAni_Item, StringTest_Item;
extern ui_item_t GPSControlHead_Item, Satellites_Item, GPSSwitch_Item;
extern ui_item_t MicroSDHead_Item, SDCard_Item, Format_Item, FileList_Item, SDCardcapacity_Item, SDCardused_space_Item, SDCardfree_space_Item, SDCardproduct_Item;
extern ui_item_t FileListHead_Item;
extern ui_item_t CycleInfo_Item, CycleInfoHead_Item, TotalKilometers_Item, ThisKilometers_Item, Speed_Item, Altitude_Item, Latitude_Item, Longitude_Item, RecoardSwitch_Item;

void AddItem_GB2312(const char *GB2312_name, ui_item_pt item)
{
    if(item == NULL) return;
    item->gb2312_name = GB2312_name;
}

void AddPage_GB2312(const char *GB2312_name, ui_page_pt page)
{
    if (page == NULL) return;
    page->gb2312_name = GB2312_name; // Assuming item has gb2312_name
}

void Create_MenuTree_GB2312(void)
{
    AddPage_GB2312("[主页]", &Home_Page);
        AddItem_GB2312("[主页]", &HomeHead_Item);
        AddItem_GB2312(" +系统", &System_Item);
            AddPage_GB2312("[系统]", &System_Page);
                AddItem_GB2312("[系统]", &SystemHead_Item);
                AddItem_GB2312(" -复位", &SystemReset_Item);
                AddItem_GB2312(" -对比度", &Contrast_Item);
                AddItem_GB2312(" -息屏", &Power_Item);   
                AddItem_GB2312(" -菜单颜色", &MenuColor_Item);  
                AddItem_GB2312(" -光标动画", &CursorAni_Item);
                AddItem_GB2312(" -文本测试", &StringTest_Item);
                #if ( UI_USE_FREERTOS == 1 )     
                AddItem_GB2312(" -UI Stack", &SystemHeap_Item);
                #endif
                AddItem_GB2312(" -IMU Task", &MPUTask_Item);
                    AddPage_GB2312("[IMU Task]", &MPUTask_Page);
                        AddItem_GB2312("[IMU Task]", &MPUTaskHead_Item);
                        #if ( UI_USE_FREERTOS == 1 )  
                        AddItem_GB2312(" -Lvel", &MPUTaskLvel_Item);
                        AddItem_GB2312(" -State", &MPUTaskState_Item);
                        #endif
                        AddItem_GB2312(" -Pitch", &Pitch_Item);
                        AddItem_GB2312(" -Roll", &Roll_Item);
                        AddItem_GB2312(" -Yaw", &Yaw_Item);
        AddItem_GB2312(" -GPS控制", &GPSControl_Item);
            AddPage_GB2312("[GPS 控制]", &GPSControl_Page);
                AddItem_GB2312("[GPS控制]", &GPSControlHead_Item);
                AddItem_GB2312(" -GPS开关", &GPSSwitch_Item);  
                AddItem_GB2312(" -卫星数", &Satellites_Item);
        AddItem_GB2312(" -内存设备", &MicroSD_Item);
            AddPage_GB2312("[内存设备]", &MicroSD_Page);
                AddItem_GB2312("[内存设备]", &MicroSDHead_Item);
                AddItem_GB2312(" -内存设备状态", &SDCard_Item);
                AddItem_GB2312(" -设备类型", &SDCardproduct_Item);
                AddItem_GB2312(" -总大小", &SDCardcapacity_Item);
                AddItem_GB2312(" -已用空间", &SDCardused_space_Item);
                AddItem_GB2312(" -可用空间", &SDCardfree_space_Item);
                AddItem_GB2312(" -格式化", &Format_Item);
                AddItem_GB2312(" -GPX文件夹", &FileList_Item);
                    AddPage_GB2312("[GPX文件夹]", &GPSList_Page);
                        AddItem_GB2312("[GPX文件夹]", &FileListHead_Item);
        AddItem_GB2312(" -表盘", &dial_plate_style1_Item);
        AddItem_GB2312(" -骑行数据", &CycleInfo_Item);
            AddPage_GB2312("[骑行数据]", &CycleInfo_Page);
                AddItem_GB2312("[骑行数据]", &CycleInfoHead_Item);
                AddItem_GB2312(" -数据记录", &RecoardSwitch_Item);
                AddItem_GB2312(" -总公里数", &TotalKilometers_Item);
                AddItem_GB2312(" -本次公里数", &ThisKilometers_Item);
                AddItem_GB2312(" -速度", &Speed_Item);
                AddItem_GB2312(" -海拔", &Altitude_Item);
                AddItem_GB2312(" -纬度", &Latitude_Item);
                AddItem_GB2312(" -经度", &Longitude_Item);
        AddItem_GB2312(" -Github", &Github_Item);
        AddItem_GB2312(" -版本", &Version_Item);
        AddItem_GB2312(" -波形", &Wave_Item);
}