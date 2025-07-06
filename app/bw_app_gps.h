#ifndef _BW_APP_GPS_H_
#define _BW_APP_GPS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "TinyGPSPlus_link.h"
#include "ui_conf.h"

void BW_GPS_Register(void);
void BW_GPS_Init(void);
void BW_GPS_DeInit(void);
uint32_t Get_GPS_Unix_Time(void);
void write_gpx(void);
void GPS_List_Load(void);
void GPX_File_Recoard_Start(void);
void GPX_File_Recoard_Stop(void);
void Create_GPX_File_Item(void);
void gps_data_check_timer_init(void);
void gps_data_check_timer_deinit(void);
bool Cancel(ui_t *ui);
bool file_parse(ui_t *ui);
void gpx_file_write_task(void);

#ifdef __cplusplus
}
#endif

#endif
