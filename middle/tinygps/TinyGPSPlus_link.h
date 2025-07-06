#ifndef _TINYGPSPLUS_LINK_H_
#define _TINYGPSPLUS_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// 定义GPS位置类型，包含纬度和经度
typedef struct {
    double lat;
    double lng;
    double ele;
} GPSLocation_t;

// 定义GPS时间类型，包含时、分、秒及百分之一秒
typedef struct {
    uint32_t value;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t centisecond;
} GPSTime_t;

// 定义GPS日期类型，包含年、月、日
typedef struct {
    uint32_t value;
    uint32_t age;
    uint16_t year;
    uint8_t month;
    uint8_t day;
} GPSDate_t;

// 定义GPS速度类型，包含不同的速度单位表示
typedef struct {
    int32_t value;
    double knots;
    double mph;
    double mps;
    double kmph;
} GPSSpeed_t;

// 定义GPS航向类型，包含角度表示
typedef struct {
    int32_t value;
    double deg;
} GPSCourse_t;

// 定义GPS海拔类型，包含不同的海拔单位表示
typedef struct {
    int32_t value;
    double meters;
    double miles;
    double kilometers;
    double feet;
} GPSAltitude_t;

// 定义GPS卫星数量类型
typedef struct {
    uint32_t value;
} GPSSatellites_t;

// 定义GPS水平稀释精度类型
typedef struct {
    int32_t value;
} GPSHdop_t;

// 定义GPS数据类型，综合包含位置、时间、日期、速度、航向、海拔、卫星数量和精度信息
typedef struct {
    GPSLocation_t location;
    GPSTime_t time;
    GPSDate_t date;
    GPSSpeed_t speed;
    GPSCourse_t course;
    GPSAltitude_t altitude;
    GPSSatellites_t satellites;
    GPSHdop_t hdop;
} GPSData_t;

bool gps_encode(char c);
void get_gps_data(GPSData_t *gps_data);
bool gps_location_isValid(void);
double get_distanceBetween(double lat1, double long1, double lat2, double long2);

#ifdef __cplusplus
}
#endif

#endif
