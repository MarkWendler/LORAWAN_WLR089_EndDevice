/**
* \file  enddevice_demo.c
*
* \brief End-Device demo application source
*
*/
#include "adeunis.h"
#include "adeunis_frame.h"


adeunis_frame_t adeunis_frame;

void adeunis_init(void)
{
    adeunis_frame.status.byte = 0x00;               // no data by default
    adeunis_frame.gps_latitude.latitude = 0x00;
    adeunis_frame.gps_longitude.longitude = 0x00;
    adeunis_frame.frame_info_presence.byte = 0x00; // no data by default
}

/**
 * 
 * @param latitude (Decimal degrees) e.g. 47.451508-->4745150 as param (DMS 47° 27' 5.428'')
 * @param longitude (D° M.M')       e.g. 18.583482 -->1858348 as param (DMS 18° 35' 0.533')
 * @param reception_scale
 * @param num_of_sattelites
 * @return 
 */
int8_t adeunis_set_gps_coordinates(uint32_t latitude, uint32_t longitude, uint8_t reception_scale, uint8_t num_of_sattelites) 
{
    uint32_t temp;
    
    adeunis_frame.gps_latitude.bcd.hemispheric = 0;
    if (latitude > 0x7FFFFFFF)
    {
        adeunis_frame.gps_latitude.bcd.hemispheric = 1;
    }
    adeunis_frame.gps_latitude.bcd.degree_tens = (latitude / 1000000) % 10;
    adeunis_frame.gps_latitude.bcd.degree_ones = (latitude / 100000) % 10;
    //Decimal degrees to Decimal minutes conversion
    temp = latitude;
    temp = temp - adeunis_frame.gps_latitude.bcd.degree_tens*1000000;
    temp = temp - adeunis_frame.gps_latitude.bcd.degree_ones*100000;
    adeunis_frame.gps_latitude.bcd.minute_tens = ((temp*6) / 10000) % 10;
    adeunis_frame.gps_latitude.bcd.minute_ones = ((temp*6) / 1000) % 10;
    adeunis_frame.gps_latitude.bcd.point_tenth = ((temp*6) / 100) % 10;
    adeunis_frame.gps_latitude.bcd.point_hundredth = ((temp*6) / 10) % 10;
    adeunis_frame.gps_latitude.bcd.point_thousandth = (temp*6)  % 10;
    //adeunis_frame.gps_latitude.bcd.point_thousandth = latitude % 10; not implemented
 
    adeunis_frame.gps_longitude.bcd.hemispheric = 0;
    if (longitude > 0x7FFFFFFF)
    {
        adeunis_frame.gps_longitude.bcd.hemispheric = 1;
    }
    
    adeunis_frame.gps_latitude.bcd.degree_tens = (longitude / 1000000) % 10;
    adeunis_frame.gps_latitude.bcd.degree_ones = (longitude / 100000) % 10;
        //Decimal degrees to Decimal minutes conversion
    temp = latitude;
    temp = temp - adeunis_frame.gps_longitude.bcd.degree_tens*1000000;
    temp = temp - adeunis_frame.gps_longitude.bcd.degree_ones*100000;
    adeunis_frame.gps_longitude.bcd.minute_tens = ((temp*6) / 10000) % 10;
    adeunis_frame.gps_longitude.bcd.minute_ones = ((temp*6) / 1000) % 10;
    adeunis_frame.gps_longitude.bcd.point_tenth = ((temp*6) / 100) % 10;
    adeunis_frame.gps_longitude.bcd.point_hundredth = ((temp*6) / 10) % 10;
    adeunis_frame.gps_longitude.bcd.point_thousandth = (temp*6)  % 10;
    //adeunis_frame.gps_longitude.bcd.point_thousandth = longitude % 10; not implemented    

    adeunis_frame.gps_quality.quality = reception_scale;
    adeunis_frame.gps_quality.num_sattelites = num_of_sattelites;
    
    adeunis_frame.status.PRESENCE_GPS = 1;
    
    return ADEUNIS_SUCCESS;
}

int8_t adeunis_set_temperature(int16_t temperature)
{
    adeunis_frame.temperature = temperature;
    adeunis_frame.frame_info_presence.temperature = 1;
    
    return ADEUNIS_SUCCESS;
}

int8_t adeunis_set_button(bool button)
{
    adeunis_frame.status.TR_BUTTON = button;
    
    return ADEUNIS_SUCCESS;
}

int8_t adeunis_set_accelero(bool accelero)
{
    adeunis_frame.status.TR_ACCELERO = accelero;
    
    return ADEUNIS_SUCCESS;
}

int8_t adeunis_set_ulcounter(uint32_t ulcounter) { 
    adeunis_frame.uplink_frame_counter = ulcounter; 
    adeunis_frame.frame_info_presence.uplink_frame_counter = true;

    return ADEUNIS_SUCCESS;
}

int8_t adeunis_set_downlink_frame_counter(uint32_t dlcounter) {
    adeunis_frame.downlink_frame_counter = dlcounter;
    adeunis_frame.frame_info_presence.downlink_frame_counter = true;

    return ADEUNIS_SUCCESS;
}

int8_t adeunis_set_battery(uint8_t battery)
{
    adeunis_frame.battery_level = battery;
    adeunis_frame.frame_info_presence.battery_level = true;
    
    return ADEUNIS_SUCCESS;
}

int8_t adeunis_set_rssi(int8_t rssi, int8_t snr)
{
    adeunis_frame.rssi = rssi;
    adeunis_frame.snr = snr;
    adeunis_frame.frame_info_presence.rssi_snr = true;
    
    return ADEUNIS_SUCCESS;
}

/**
 * \brief Get the frame to send
 * \param frame Pointer to the frame to send min 16 bytes
 * \return 0 invalid, -1 if no data, >0 size of the frame
*/
int8_t adeunis_get_frame(uint8_t *frame){
    uint8_t i = 0;
// Frame 1
// Byte     NÂ°  1       2 to 5          6 to 9          10
// Description  Status  GPS Latitude    GPS Longitude   GPS Quality    
    if(adeunis_frame.status.byte == 0){
        return ADEUNIS_NO_DATA;
    }
    frame[i++] = adeunis_frame.status.byte;
    if (adeunis_frame.status.PRESENCE_GPS == 1)
    {
        frame[i++] = adeunis_frame.gps_latitude.bytes[0];
        frame[i++] = adeunis_frame.gps_latitude.bytes[1];
        frame[i++] = adeunis_frame.gps_latitude.bytes[2];
        frame[i++] = adeunis_frame.gps_latitude.bytes[3];
        frame[i++] = adeunis_frame.gps_longitude.bytes[0];
        frame[i++] = adeunis_frame.gps_longitude.bytes[1];
        frame[i++] = adeunis_frame.gps_longitude.bytes[2];
        frame[i++] = adeunis_frame.gps_longitude.bytes[3];
        frame[i++] = adeunis_frame.gps_quality.byte;
    }
    
// Frame 2
// Byte NÂ°      1       2           3           4           5 to 6  7       8
// Description  Status  Temperature UL counter  DL counter  Battery RSSI    SNR
    frame[i++] = adeunis_frame.frame_info_presence.byte;
    if (adeunis_frame.frame_info_presence.temperature == 1)
    {
        frame[i++] = adeunis_frame.temperature;
    }
    if (adeunis_frame.frame_info_presence.uplink_frame_counter == 1)
    {
        frame[i++] = adeunis_frame.uplink_frame_counter;
    }
    if (adeunis_frame.frame_info_presence.downlink_frame_counter == 1)
    {
        frame[i++] = adeunis_frame.downlink_frame_counter;
    }
    if (adeunis_frame.frame_info_presence.battery_level == 1)
    {
        frame[i++] = (uint8_t)(adeunis_frame.battery_level>>8);
        frame[i++] = (uint8_t)(adeunis_frame.battery_level);
    }
    if (adeunis_frame.frame_info_presence.rssi_snr == 1)
    {
        frame[i++] = adeunis_frame.rssi;
        frame[i++] = adeunis_frame.snr;
    }
    return i;
}




