/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
#include "mockgpspayload.h"
#include "adeunis.h"

/* @param latitude (Decimal degrees) e.g. 47.451508-->4745150 as param (DMS 47° 27' 5.428'')
 * @param longitude (D° M.M')       e.g. 18.583482 -->1858348 as param (DMS 18° 35' 0.533')
*/
int32_t latitude = 4745150;
int32_t longitude = 1858348;
uint8_t counter = 0;

int8_t getMockGpsPayload(uint8_t *frame)
{
    adeunis_init();
    adeunis_set_button(true); // trigger by button
    adeunis_set_gps_coordinates(latitude, longitude, 2, 5);
    latitude += 10000; // mocking GPS movement
    longitude += 10000;
    adeunis_set_ulcounter(counter++);
    adeunis_set_temperature(235);//23.5C
    
    return adeunis_get_frame(frame);

}





/* *****************************************************************************
 End of File
 */
