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
    AddPage_GB2312("[��ҳ]", &Home_Page);
        AddItem_GB2312("[��ҳ]", &HomeHead_Item);
        AddItem_GB2312(" +ϵͳ", &System_Item);
            AddPage_GB2312("[ϵͳ]", &System_Page);
                AddItem_GB2312("[ϵͳ]", &SystemHead_Item);
                AddItem_GB2312(" -��λ", &SystemReset_Item);
                AddItem_GB2312(" -�Աȶ�", &Contrast_Item);
                AddItem_GB2312(" -Ϣ��", &Power_Item);   
                AddItem_GB2312(" -�˵���ɫ", &MenuColor_Item);  
                AddItem_GB2312(" -��궯��", &CursorAni_Item);
                AddItem_GB2312(" -�ı�����", &StringTest_Item);
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
        AddItem_GB2312(" -GPS����", &GPSControl_Item);
            AddPage_GB2312("[GPS ����]", &GPSControl_Page);
                AddItem_GB2312("[GPS����]", &GPSControlHead_Item);
                AddItem_GB2312(" -GPS����", &GPSSwitch_Item);  
                AddItem_GB2312(" -������", &Satellites_Item);
        AddItem_GB2312(" -�ڴ��豸", &MicroSD_Item);
            AddPage_GB2312("[�ڴ��豸]", &MicroSD_Page);
                AddItem_GB2312("[�ڴ��豸]", &MicroSDHead_Item);
                AddItem_GB2312(" -�ڴ��豸״̬", &SDCard_Item);
                AddItem_GB2312(" -�豸����", &SDCardproduct_Item);
                AddItem_GB2312(" -�ܴ�С", &SDCardcapacity_Item);
                AddItem_GB2312(" -���ÿռ�", &SDCardused_space_Item);
                AddItem_GB2312(" -���ÿռ�", &SDCardfree_space_Item);
                AddItem_GB2312(" -��ʽ��", &Format_Item);
                AddItem_GB2312(" -GPX�ļ���", &FileList_Item);
                    AddPage_GB2312("[GPX�ļ���]", &GPSList_Page);
                        AddItem_GB2312("[GPX�ļ���]", &FileListHead_Item);
        AddItem_GB2312(" -����", &dial_plate_style1_Item);
        AddItem_GB2312(" -��������", &CycleInfo_Item);
            AddPage_GB2312("[��������]", &CycleInfo_Page);
                AddItem_GB2312("[��������]", &CycleInfoHead_Item);
                AddItem_GB2312(" -���ݼ�¼", &RecoardSwitch_Item);
                AddItem_GB2312(" -�ܹ�����", &TotalKilometers_Item);
                AddItem_GB2312(" -���ι�����", &ThisKilometers_Item);
                AddItem_GB2312(" -�ٶ�", &Speed_Item);
                AddItem_GB2312(" -����", &Altitude_Item);
                AddItem_GB2312(" -γ��", &Latitude_Item);
                AddItem_GB2312(" -����", &Longitude_Item);
        AddItem_GB2312(" -Github", &Github_Item);
        AddItem_GB2312(" -�汾", &Version_Item);
        AddItem_GB2312(" -����", &Wave_Item);
}