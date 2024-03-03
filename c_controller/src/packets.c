#include <stdint.h>
#include "uart.h"

#define CAMERA_PACKET_ID 1
#define MOTOR_PACKET_ID 2

struct CameraData {
    uint16_t data[128];
};

void sendCameraData(struct CameraData data) {
    uart0_put(CAMERA_PACKET_ID);
    for (int i = 0; i < 128; i++) {
        senduart(data.data[i] >> 8);
        senduart(data.data[i] & 0xFF);
    }
}