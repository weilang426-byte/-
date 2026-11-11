#include "BlueTooth.h"
#include <math.h>
#include <errno.h>

/* Visible from VS Code: invalid packets and attempts to tune while armed. */
volatile uint32_t bluetooth_rejected_packets;
void BlueTooth_ReceiveStart(void)
{
    HAL_UART_Receive_IT(&huart2, (uint8_t *)&rx_data, 1);
}

static int ParseNumber(const char *text, float *value)
{
    if (text == NULL || *text == '\0') return 0;
    char *end;
    errno = 0;
    float parsed = strtof(text, &end);
    if (end == text || *end != '\0' || errno == ERANGE || !isfinite(parsed)) return 0;
    *value = parsed;
    return 1;
}

void BlueTooth_Receive(void)
{
    if (!rx_flag) return;
    /* ISR keeps the completed packet immutable until this flag is cleared. */
    char packet[BLUETOOTH_PACKET_LENGTH];
    memcpy(packet, buffer, sizeof(packet));
    packet[sizeof(packet) - 1] = '\0';
    __DMB();
    rx_flag = 0;
    char *fields[6] = {packet};
    unsigned count = 1;
    for (char *p = packet; *p; ++p) {
        if (*p == ',') {
            *p = '\0';
            if (count >= 6) goto rejected;
            fields[count++] = p + 1;
        }
    }
    for (unsigned i = 0; i < count; ++i)
        if (*fields[i] == '\0') goto rejected;

    if (strcmp(fields[0], "slider") == 0) {
        float value;
        if (count != 3 || PID_angle_flag || !ParseNumber(fields[2], &value))
            goto rejected;
        /* Bounds prevent accidental 100x-scale gains. Not tuned stability limits. */
        if (strcmp(fields[1], "Kp") == 0 && value >= 0.0f && value <= 50.0f)
            PID_angle.Kp = value;
        else if (strcmp(fields[1], "Ki") == 0 && value >= 0.0f && value <= 50.0f)
            PID_angle.Ki = value;
        else if (strcmp(fields[1], "Kd") == 0 && value >= 0.0f && value <= 1.0f)
            PID_angle.Kd = value;
        else goto rejected;
    } else if (strcmp(fields[0], "joystick") == 0) {
        float values[4];
        if (count != 5) goto rejected;
        for (unsigned i = 0; i < 4; ++i) {
            if (!ParseNumber(fields[i + 1], &values[i]) ||
                values[i] < -100.0f || values[i] > 100.0f) goto rejected;
        }
        PID_joystick.LeftX = (int16_t)values[0];
        PID_joystick.LeftY = (int16_t)values[1];
        PID_joystick.RightX = (int16_t)values[2];
        PID_joystick.RightY = (int16_t)values[3];
    } else if (strcmp(fields[0], "key") != 0 || count != 3) goto rejected;
    return;
rejected:
    bluetooth_rejected_packets++;
}
