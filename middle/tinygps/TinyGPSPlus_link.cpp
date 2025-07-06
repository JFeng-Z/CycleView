#include "TinyGPSPlus_link.h"
#include "TinyGPS++.h"

static TinyGPSPlus gps;

bool gps_encode(char c)
{
    return gps.encode(c);
}

void get_gps_data(GPSData_t *gps_data)
{
    gps_data->location.lat = gps.location.lat(); // Latitude in degrees (double)
    gps_data->location.lng = gps.location.lng(); // Longitude in degrees (double)
    gps_data->location.ele = gps.altitude.kilometers(); // Altitude in meters (double)
    gps_data->date.value = gps.date.value(); // Raw date in DDMMYY format (u32)
    gps_data->date.year = gps.date.year(); // Year (2000+) (u16)
    gps_data->date.month = gps.date.month(); // Month (1-12) (u8)
    gps_data->date.day = gps.date.day(); // Day (1-31) (u8)
    gps_data->date.age = gps.date.age();
    gps_data->time.value = gps.time.value(); // Raw time in HHMMSSCC format (u32)
    gps_data->time.hour = gps.time.hour(); // Hour (0-23) (u8)
    gps_data->time.minute = gps.time.minute(); // Minute (0-59) (u8)
    gps_data->time.second = gps.time.second(); // Second (0-59) (u8)
    gps_data->time.centisecond = gps.time.centisecond(); // 100ths of a second (0-99) (u8)
    gps_data->speed.value = gps.speed.value(); // Raw speed in 100ths of a knot (i32)
    gps_data->speed.knots = gps.speed.knots(); // Speed in knots (double)
    gps_data->speed.mph = gps.speed.mph(); // Speed in miles per hour (double)
    gps_data->speed.mps = gps.speed.mps(); // Speed in meters per second (double)
    gps_data->speed.kmph = gps.speed.kmph(); // Speed in kilometers per hour (double)
    gps_data->course.value = gps.course.value(); // Raw course in 100ths of a degree (i32)
    gps_data->course.deg = gps.course.deg(); // Course in degrees (double)
    gps_data->altitude.value = gps.altitude.value(); // Raw altitude in centimeters (i32)
    gps_data->altitude.meters = gps.altitude.meters(); // Altitude in meters (double)
    gps_data->altitude.miles = gps.altitude.miles(); // Altitude in miles (double)
    gps_data->altitude.kilometers = gps.altitude.kilometers(); // Altitude in kilometers (double)
    gps_data->altitude.feet = gps.altitude.feet(); // Altitude in feet (double)
    gps_data->satellites.value = gps.satellites.value(); // Number of satellites in use (u32)
    gps_data->hdop.value = gps.hdop.value(); // Horizontal Dim. of Precision (100ths-i32)
}

bool gps_location_isValid(void)
{
    return gps.location.isValid();
}

double get_distanceBetween(double lat1, double long1, double lat2, double long2)
{
    return gps.distanceBetween(lat1, long1, lat2, long2);
}