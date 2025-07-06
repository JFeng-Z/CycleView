#include "bw_system.h"
#include "bw_ble.h"

int main(void)
{
    BW_BLE_Init();
    BW_System_Init();
    for (;;)
    {
        BW_System_Task();
    }
}
