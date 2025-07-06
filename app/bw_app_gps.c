#include "bw_app_gps.h"
#include "bw_bsp_gps_handle.h"
#include "bw_app_utf8_api.h"
#include "TinyGPSPlus_link.h"
#include "math.h"
#include "GPX_Link.h"
#include "ff.h"
#include "string.h"
#include "nrf_log.h"
#include "stdlib.h"
#include "sdcard.h"
#include "ui.h"
#include "widget.h"
#include "app_timer.h"
#include "bw_app_sdcard.h"
#include "time.h"
#include "fileDriver.h"
#include "GPX_Parse.h"
#include "dispDriver.h"

#define GPS_LIST_PATH "0:/[主页]/[内存设备]/[GPX文件夹]"
#define TotalKilometers_PATH "0:/[主页]/[骑行数据]"

typedef struct
{
    uint32_t      file_size;
    uint32_t      file_offset;
    char          file_name[_MAX_LFN];
}bw_gps_t;

GPSData_t gps_data;
uint8_t gps_state;
uint8_t gps_data_state;
int8_t slope;
uint32_t Satellites;
float Speed, Speed_last, Altitude, Longitude, Latitude;
GPSLocation_t gps_location_last;
bsp_gps_handle_t *gps_handle;

_APP_TIMER_DEF(m_gps_timer);
static uint8_t gps_data_check_state;

static void gps_timer_handler(void * p_context)
{
    gps_data_check_state = 1;
}

static int8_t app_gps_data_update(void *data)
{
    if(data == NULL)return -1;
    if(gps_encode(*(char* )data))
    {
        gps_location_last.lat = gps_data.location.lat;
        gps_location_last.lng = gps_data.location.lng;
        gps_location_last.ele = gps_data.location.ele;
        get_gps_data(&gps_data);
        Speed = gps_data.speed.kmph;
        if(Speed - Speed_last >20)
        {
            Speed = Speed_last; // 防止GPS数据异常导致速度突增
        }
        Speed = Speed*0.8f + Speed_last*0.2f; // 平滑处理
        Speed_last = Speed;
        Altitude = gps_data.location.ele;
        Latitude = gps_data.location.lat;
        Longitude = gps_data.location.lng;
        Satellites = gps_data.satellites.value;
        gps_data.time.hour += 8;
        gps_data_state = 1;
    }
    return 0;
}

void BW_GPS_Register(void)
{
    gps_handle = bsp_gps_handle_register(app_gps_data_update);
    app_timer_create(&m_gps_timer, APP_TIMER_MODE_REPEATED, gps_timer_handler);
}

void BW_GPS_Init(void)
{
    int8_t ret = bsp_gps_handle_init();
    if(ret != 0)
    {
        NRF_LOG_ERROR("BW GPS Init Failed");
        return ;
    }
    NRF_LOG_INFO("BW GPS Inited");
    gps_state = 1;
}

void BW_GPS_DeInit(void)
{
    int8_t ret = bsp_gps_handle_deinit();
    if(ret != 0)
    {
        NRF_LOG_ERROR("BW GPS DeInit Failed");
        return ;
    }
    NRF_LOG_INFO("BW GPS DeInited");
    gps_state = 0;
}

extern ui_page_t GPSList_Page; 

/**
 * @brief Calculates the slope percentage between two points.
 *
 * This function computes the slope as a percentage, given the horizontal distance
 * and the height difference between two points. The result is cast to an int8_t.
 *
 * @param distance         The horizontal distance between two points (in meters).
 * @param Height_difference The vertical height difference between the two points (in meters).
 *
 * @return The slope as a percentage (int8_t). Positive values indicate an ascent,
 *         negative values indicate a descent.
 */
static int8_t get_slope(double distance, double Height_difference)
{
    return (int8_t)((Height_difference / distance) * 100);
}

void convertToScreen(float lon, float lat,
                     float minLon, float maxLon,
                     float minLat, float maxLat,
                     int *x, int *y) 
{
    const int screenWidth = 128;
    const int screenHeight = 64;

    // 计算地理范围差（转换为秒）
    float deltaLon = (maxLon - minLon) * 3600;
    float deltaLat = (maxLat - minLat) * 3600;

    // 计算每像素对应的秒数（比例因子）
    float scaleX = deltaLon / screenWidth;
    float scaleY = deltaLat / screenHeight;

    // 计算当前点与最小经纬度的差值（秒）
    float deltaLonSec = (lon - minLon) * 3600;
    float deltaLatSec = (maxLat - lat) * 3600; // 反转Y轴

    // 转换为屏幕坐标
    *x = (int)(deltaLonSec / scaleX);
    *y = (int)(deltaLatSec / scaleY);

    // 约束坐标在屏幕范围内（0-127, 0-63）
    *x = (*x < 0) ? 0 : (*x >= screenWidth ? screenWidth - 1 : *x);
    *y = (*y < 0) ? 0 : (*y >= screenHeight ? screenHeight - 1 : *y);
}

static char gpx_heard_buffer[1024];
static FIL gpx_file;
static char gps_path[_MAX_LFN];

static char lat_str[32], lng_str[32], ele_str[32], time_str[32], speed_str[16];
float total_kilometers, this_kilometers, distance;
extern struct tm tm;

uint32_t Get_GPS_Unix_Time(void)
{
    struct tm *tm_info;
    time_t current_time = mktime(&tm);
    tm_info = localtime(&current_time);
    return (uint32_t)mktime(tm_info);
}

void GPX_File_Recoard_Start(void)
{
    FRESULT sult;
    char *p;
    BW_SDCard_Init();
    BW_GPS_Init();
    gps_data_check_timer_init();
    gpx_setMetaDesc("GPS_Recoard");
    snprintf(time_str, sizeof(time_str), "%d%d%d %d-%d-%d", gps_data.date.year, gps_data.date.month, gps_data.date.day, gps_data.time.hour, gps_data.time.minute, gps_data.time.second);
    gpx_setName(time_str);
    gpx_setDesc("GPS Recoard");
    gpx_setSrc("Bike Watch");
    snprintf(gps_path, sizeof(gps_path), ""GPS_LIST_PATH"/%s.gpx", time_str);
    NRF_LOG_INFO("Begin Recoard gps_path: %s", gps_path);
    sult = f_open(&gpx_file, gps_path, FA_CREATE_ALWAYS | FA_WRITE);
    if (sult == FR_OK)
    {
        p = gpx_getOpen();
        strcat(gpx_heard_buffer, p);
        p = gpx_getMetaData();
        strcat(gpx_heard_buffer, p);
        p = gpx_getTrakOpen();
        strcat(gpx_heard_buffer, p);
        p = gpx_getInfo();
        strcat(gpx_heard_buffer, p);
        p = gpx_getTrakSegOpen();
        strcat(gpx_heard_buffer, p);
        sult = f_write(&gpx_file, gpx_heard_buffer, strlen(gpx_heard_buffer), NULL);
        if(sult != FR_OK)NRF_LOG_INFO("f_write %s sult: %d", gpx_heard_buffer, sult);
        memset(gpx_heard_buffer, 0, sizeof(gpx_heard_buffer));
        Create_GPX_File_Item();
    }
    else NRF_LOG_INFO("f_open sult: %d", sult);
}

void gps_data_check_timer_init(void)
{
    app_timer_start(m_gps_timer, APP_TIMER_TICKS(1000), NULL);
}

void gps_data_check_timer_deinit(void)
{
    app_timer_stop(m_gps_timer);
}

void write_gpx(void)
{
    FRESULT sult;
    char *p;
    uint32_t size;
    double Height_diff;
    if (gps_data_state == 1 && gps_data_check_state == 1 && gps_data.location.lat != 0 && gps_data.location.lng != 0)
    {
        distance = (float )get_distanceBetween(gps_data.location.lat, gps_data.location.lng, gps_location_last.lat, gps_location_last.lng);
        this_kilometers += distance;
        total_kilometers += distance;
        Height_diff = gps_data.location.ele - gps_location_last.ele;
        slope = get_slope(distance, Height_diff);
        snprintf(time_str, sizeof(time_str), "%d-%d-%dT%d:%d:%dZ", gps_data.date.year, gps_data.date.month, gps_data.date.day, gps_data.time.hour, gps_data.time.minute, gps_data.time.second);
        snprintf(lat_str, sizeof(lat_str), "%.6f", gps_data.location.lat);
        snprintf(lng_str, sizeof(lng_str), "%.6f", gps_data.location.lng);
        snprintf(ele_str, sizeof(ele_str), "%.2f", gps_data.altitude.meters);
        snprintf(speed_str, sizeof(speed_str), "%.2f", gps_data.speed.kmph);
        gpx_setEle(ele_str);
        gpx_setSpeed(speed_str);
        gpx_setTime(time_str);
        p = gpx_getPt(lat_str, lng_str);
        size = strlen(p);
        sult = f_write(&gpx_file, p, size, NULL);
        if(sult != FR_OK)NRF_LOG_INFO("f_write %s sult: %d", p, sult);
        gps_data_state = 0;
        gps_data_check_state = 0;
    }
}

void gpx_file_write_task(void)
{
    write_gpx();
}

static void cycle_info_update(void)
{
    FIL total_kilometers_file;
    FRESULT sult;
    sult = f_open(&total_kilometers_file, ""TotalKilometers_PATH"/ -总公里数", FA_CREATE_ALWAYS | FA_WRITE);
    if (sult == FR_OK)
    {
        sult = f_write(&total_kilometers_file, &total_kilometers, sizeof(float), NULL);
        if(sult != FR_OK)
        {
            NRF_LOG_INFO("Total Kilometers Write Failed");
        }
        sult = f_close(&total_kilometers_file);
        if(sult == FR_OK)
        {
            NRF_LOG_INFO("Total Kilometers updated");
        }
        else 
        {
            NRF_LOG_INFO("Total Kilometers update failed");
        }
    }
    else NRF_LOG_INFO("Total Kilometers file not found");
}

void GPX_File_Recoard_Stop(void)
{
    if(SDcard_State_Get() == 0)
    {
        NRF_LOG_INFO("SDCard not ready, cannot recoard");
        return ;
    }    
    FRESULT sult;
    strcat(gpx_heard_buffer, gpx_getTrakSegClose());
    strcat(gpx_heard_buffer, gpx_getTrakClose());
    strcat(gpx_heard_buffer, gpx_getClose());
    f_write(&gpx_file, gpx_heard_buffer, strlen(gpx_heard_buffer), NULL);
    memset(gpx_heard_buffer, 0, sizeof(gpx_heard_buffer));
    sult = f_close(&gpx_file);
    if(sult == FR_OK)
    {
        NRF_LOG_INFO("gpx file recorded");
    }
    else 
    {
        NRF_LOG_INFO("gpx file recorded failed sult: %d", sult);
    }
    this_kilometers = 0;
    cycle_info_update();

    gps_data_check_timer_deinit();

    BW_GPS_DeInit();
    BW_SDCard_DeInit();
}

bool Cancel(ui_t *ui)
{
    UI_LOG_INFO("Cancel %s", ui->nowItem->itemName);
    return true;
}

#define GPX_PARSE_BUFFER_SIZE 512

static char parse_buf[GPX_PARSE_BUFFER_SIZE];

typedef enum
{
    GPX_PARSE_NONE = 0,
    GPX_PARSE_START,
    GPS_PARSE_LIMIT,
    GPX_PARSE_POINT,
    GPX_PARSE_END
} gpx_parse_state_t;

typedef struct 
{
    FIL *fp;
    char *path_buffer;
    gpx_parse_state_t state;
    char *parse_buffer;
    float min_lat;
    float min_lng;
    float max_lat;
    float max_lng;
    uint32_t file_offset;
    int point_x;
    int point_y;
    int point_last_x;
    int point_last_y;
} gpx_parse_info_t;

static bool gpx_file_parse_start(ui_t *ui, gpx_parse_info_t *info)
{
    BW_SDCard_Init();
    filePathSearch(ui->nowItem, info->parse_buffer);
    if(f_open(info->fp, info->parse_buffer, FA_READ) != FR_OK)
    {
        NRF_LOG_INFO("f_open failed");
        return false;
    }
    Disp_ClearBuffer();
    uint8_t color = 1;
    Disp_SetDrawColor(&color);
    BW_Draw_Parse_Cue();
    return true;
}

static bool gpx_file_parse_limit_calculate(gpx_parse_info_t *info)
{
    float lat = 0, lng = 0;
    uint32_t file_size = info->fp->obj.objsize;
    uint32_t start_pos, end_pos, file_offset_temp = 0;  
    for(uint32_t i = 0; i < 20; i++)
    {
        start_pos = f_tell(info->fp); // Get current file position
        if(f_gets(info->parse_buffer, GPX_PARSE_BUFFER_SIZE, info->fp) == NULL)
        {
            f_lseek(info->fp, 0); // Reset file pointer to the beginning
            Disp_ClearBuffer();
            return true;
        }    
        end_pos = f_tell(info->fp); // Get file position after reading
        file_offset_temp = end_pos - start_pos; // Calculate the length of the string read
        info->file_offset += file_offset_temp; // Update file offset
        if(strstr(info->parse_buffer, "<trkpt"))break;
    }
    if(gpx_parse(info->parse_buffer, file_offset_temp, &lat, &lng) == 0)
    {
        if(lat > info->max_lat)info->max_lat = lat;
        if(lat < info->min_lat)info->min_lat = lat;
        if(lng > info->max_lng)info->max_lng = lng;
        if(lng < info->min_lng)info->min_lng = lng;
    }
    Draw_Scrollbar_Custom(19, 40, 90, 6, 3, info->file_offset, 0, file_size);
    Disp_SendBuffer();
    return false;
}

static bool gpx_file_parse_point_draw(gpx_parse_info_t *info)
{
    float lat = 0, lng = 0;
    for(uint32_t i = 0; i < 20; i++)
    {
        if(f_gets(info->parse_buffer, GPX_PARSE_BUFFER_SIZE, info->fp) == NULL)
        {
            info->point_x = -1;
            info->point_y = -1;
            info->point_last_x = -1;
            info->point_last_y = -1;
            f_lseek(info->fp, 0); // Reset file pointer to the beginning
            return true;
        }    
        if(strstr(info->parse_buffer, "<trkpt"))break;
    }
    if(gpx_parse(info->parse_buffer, strlen(info->parse_buffer), &lat, &lng) == 0)
    {
        convertToScreen(lng, lat, info->min_lng, info->max_lng, info->min_lat, info->max_lat, &info->point_x, &info->point_y);
        if(info->point_x == info->point_last_x && info->point_y == info->point_last_y)
        {
            return false; // 如果点没有变化，则不绘制
        }
        if(info->point_last_x != -1 && info->point_last_y != -1)
        Disp_DrawLine(info->point_last_x, info->point_last_y, info->point_x, info->point_y);
        // Disp_DrawPixel(info->point_x, info->point_y);
        info->point_last_x  = info->point_x;
        info->point_last_y  = info->point_y;
        Disp_SendBuffer();
    }
    return false;
}

static bool gpx_file_parse_end(gpx_parse_info_t *info)
{
    f_close(info->fp);
    return true;
}

bool file_parse(ui_t *ui)
{
    static gpx_parse_info_t gpx_file_parse_info = {
        .fp = &gpx_file,
        .path_buffer = gps_path,
        .state = GPX_PARSE_START,
        .parse_buffer = parse_buf,
        .file_offset = 0,
        .min_lat = 999,
        .min_lng = 999,
        .max_lat = 0,
        .max_lng = 0,
        .point_x = -1,
        .point_y = -1,
        .point_last_x = -1,
        .point_last_y = -1
    };
    switch (gpx_file_parse_info.state)
    {
    case GPX_PARSE_START:
        if(gpx_file_parse_start(ui, &gpx_file_parse_info))
        {
            gpx_file_parse_info.state = GPS_PARSE_LIMIT;
        }
        break;
    case GPS_PARSE_LIMIT:
        if(gpx_file_parse_limit_calculate(&gpx_file_parse_info))
        {
            gpx_file_parse_info.state = GPX_PARSE_POINT;
        }
        break;
    case GPX_PARSE_POINT:
        if(gpx_file_parse_point_draw(&gpx_file_parse_info))
        {
            gpx_file_parse_info.state = GPX_PARSE_END;
        }
        break;
    case GPX_PARSE_END:
        if(gpx_file_parse_end(&gpx_file_parse_info))
        {
            gpx_file_parse_info.state = GPX_PARSE_NONE;
            BW_SDCard_DeInit();
        }
        break;
    case GPX_PARSE_NONE:
        break;
    default:
        break;
    }
    if(ui->action == UI_ACTION_ENTER)
    {
        gpx_file_parse_end(&gpx_file_parse_info);
        BW_SDCard_DeInit();
        gpx_file_parse_info.file_offset = 0;
        gpx_file_parse_info.min_lat = 999;
        gpx_file_parse_info.min_lng = 999;
        gpx_file_parse_info.max_lat = 0;
        gpx_file_parse_info.max_lng = 0;
        gpx_file_parse_info.point_x = -1;
        gpx_file_parse_info.point_y = -1;
        gpx_file_parse_info.point_last_x = -1;
        gpx_file_parse_info.point_last_y = -1;
        gpx_file_parse_info.state = GPX_PARSE_START; // 重置状态
        ui->action = UI_ACTION_NONE;
        return true; // 退出解析
    }
    return false; // 继续解析
}

void Create_GPX_File_Item(void)
{
    Create_GPX_Item(&gps_path[sizeof(GPS_LIST_PATH)]);
}

void gpx_file_list(void)
{
    FRESULT ff_result;
    DIR dir;
    FILINFO file_info;
    ff_result = f_findfirst(&dir, &file_info, GPS_LIST_PATH, "*.gpx");
    if (ff_result)
    {
        NRF_LOG_INFO("no find gpx file. %d", ff_result);
        return;
    }
    while (ff_result == FR_OK && file_info.fname[0])
    {
        Create_GPX_Item(file_info.fname);
        ff_result = f_findnext(&dir, &file_info);
    }
    f_closedir(&dir);
}

void GPS_List_Load(void)
{
    BW_SDCard_Init();
    gpx_file_list();
    BW_SDCard_DeInit();
}

DWORD get_fattime(void)
{
    DWORD data_time;

    data_time = gps_data.date.year - 1980;
    data_time = data_time << 25;
    data_time |= gps_data.date.month << 21;
    data_time |= gps_data.date.day << 16;
    data_time |= gps_data.time.hour << 11;
    data_time |= gps_data.time.minute << 5;
    data_time |= gps_data.time.second;
    
    return data_time;
}
