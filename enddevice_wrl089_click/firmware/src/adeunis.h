#ifndef _ADEUNIS_H
#define _ADEUNIS_H

#include <stdint.h>
#include <stdbool.h>

void adeunis_init(void);
int8_t adeunis_set_gps_coordinates(uint32_t latitude, uint32_t longitude, uint8_t reception_scale, uint8_t num_of_sattelites);
int8_t adeunis_set_temperature(int16_t temperature);
int8_t adeunis_set_ulcounter(uint32_t uplink_frame_counter);
int8_t adeunis_set_downlink_frame_counter(uint32_t downlink_frame_counter);
int8_t adeunis_set_battery_level(uint8_t battery_level);
int8_t adeunis_set_rssi_snr(int8_t rssi, int8_t snr);

int8_t adeunis_get_frame(uint8_t *frame);

#endif /* _ADEUNIS_H */
