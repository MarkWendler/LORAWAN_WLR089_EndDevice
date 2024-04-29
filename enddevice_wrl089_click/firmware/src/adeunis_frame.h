#ifndef _ADEUNIS_FRAME_H
#define _ADEUNIS_FRAME_H

#include <stdint.h>
#include <stdbool.h>
/*
Adeunis Frame Format based on 
https://www.adeunis.com/wp-content/uploads/2020/03/User_Guide_FTD_LoRaWAN_US902-928_V1.0.1.pdf

Byte N°         1       2 to 5          6 to 9          10
Description     Status  GPS Latitude    GPS Longitude   GPS Quality
Example         30      45 15 96 90     00 55 34 50     27

Status
0 Not used
0 Transmission triggered by the accelerometer
1 Transmission triggered by pressing pushbutton 1
1 Presence of GPS information
0 Not used
0 Not used
0 Not used
0 Not used


*/
typedef struct tag_adeunis
{
    union tag_status    {
        /* data */
        uint8_t byte;
        struct
        {
            uint8_t notused1 : 4u; //!< external MOSFET over current level
            uint8_t PRESENCE_GPS : 1u; //Presence of GPS information
            uint8_t TR_BUTTON : 1u; //Transmission triggered by pressing pushbutton
            uint8_t TR_ACCELERO : 1u; //Transmission triggered by the accelerometer
            uint8_t notused2 : 1u;
        };

    }status;
    
    union tag_gps_latitude { //BCD coded 005°53,450
        uint8_t bytes[4];
        uint32_t latitude;
        struct adeunis_gps_latitude
        {
            //byte 2
            uint8_t degree_tens : 4u;
            uint8_t degree_ones : 4u;
            //byte 3
            uint8_t minute_tens : 4u;
            uint8_t minute_ones : 4u;
            //byte 4
            uint8_t point_hundredth : 4u;
            uint8_t point_tenth : 4u;
            
            //byte 5
            uint8_t hemispheric : 1u; // 0 = North, 1 = South
            uint8_t point_tentousandth_notinuse : 3u; // not used, GPS is not that accurate
            uint8_t point_thousandth : 4u;
        }bcd;
        
    }gps_latitude;
    
    union tag_gps_longitude { //BCD coded 005°53,450
        uint8_t bytes[4];
        uint32_t longitude;
        struct adeunis_gps_longitude
        {
            //byte 6
            uint8_t degree_tens : 4u;
            uint8_t degree_ones : 4u;
            //byte 7
            uint8_t minute_tens : 4u;
            uint8_t minute_ones : 4u;
            //byte 8
            uint8_t point_hundredth : 4u;
            uint8_t point_tenth : 4u;
            
            //byte 9
            uint8_t hemispheric : 1u; // 0 = North, 1 = South
            uint8_t point_tentousandth_notinuse : 3u; // not used, GPS is not that accurate
            uint8_t point_thousandth : 4u;
        }bcd;
    }gps_longitude;

    union tag_gps_quality { //byte NO 10
        uint8_t byte;
        struct
        {
            uint8_t num_sattelites : 4u;    // Number of satellites       
            uint8_t quality : 4u;           // Reception scale
        };
    }gps_quality;
    
    union tag_frame_info_presence {
        uint8_t byte;
        struct
        {
            uint8_t rssi_snr : 1u; // Presence of RSSI and SNR information
            uint8_t battery_level : 1u; // Presence of battery level information
            uint8_t downlink_frame_counter : 1u; // Presence of Downlink frame counter
            uint8_t uplink_frame_counter : 1u; // Presence of Uplink frame counter
            uint8_t unused1 : 1u; // Not used
            uint8_t unused2 : 1u; // Not used
            uint8_t unused3 : 1u; // Not used
            uint8_t temperature : 1u; // Presence of temperature information
        };
    }frame_info_presence;

    //Temperature in °C (two’s complement) -128 ... +127
    int8_t temperature;             //0xF4 negative temperature (sign bit = 1) : Value T°C = (64+32+16+4)-128 = -12°C
    //Uplink frame counter
    uint8_t uplink_frame_counter;   //This counter is not the LoRaWAN stack frame counter, but an internal frame counter. Therefore, there is no correlation between
                                    //this counter and the one that you can view on the network.
    //Downlink frame counter
    uint8_t downlink_frame_counter; //This counter is not the LoRaWAN stack frame counter, but an internal frame counter. Therefore, there is no correlation between
                                    //this counter and the one that you can view on the network.
    //Battery level in mV
    uint16_t battery_level;
    //RSSI in dBm absolute value 0 ... 255
    uint8_t rssi;
    //SNR in dB Two's complement -127 ... +127
    int8_t snr;

} adeunis_frame_t;


#define ADEUNIS_SUCCESS 0
#define ADEUNIS_ERROR -1
#define ADEUNIS_NO_DATA -2

#endif /* _ADEUNIS_H */
