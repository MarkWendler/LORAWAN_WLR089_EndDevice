#include "utilities.h"


/* Status bit-mask for LoRaWAN Join indicator */
#define DEMOAPP_LORAWANJOINED_BITMASK   (0x00000001)

#if (ENABLE_PDS == 1)

bool bIsJoined = false;

void DEMO_ProcessRestorePrompt(void)
{
	StackRetStatus_t xStat = LORAWAN_SUCCESS;
	uint8_t ucPrevBand = 0xff;
	uint8_t ucCurChoice = 0xff;
	bool bJoinBackoffEn = false;

	PDS_RestoreAll();
	LORAWAN_GetAttr(ISMBAND, NULL, &ucPrevBand);

	xStat = LORAWAN_Reset(APP_ISMBAND);


	/*
	 * Disabled Join back-off in Demo application
	 * Needs to be enabled in Production Environment Ref Section
	 */
	LORAWAN_SetAttr(JOIN_BACKOFF_ENABLE, &bJoinBackoffEn);

	if ((xStat == LORAWAN_SUCCESS)) {
		uint32_t ulJoinStatus = 0;
		PDS_RestoreAll();
		LORAWAN_GetAttr(LORAWAN_STATUS, NULL, &ulJoinStatus);
		printf("Successfully restored settings from PDS\r\n");

		if (ulJoinStatus & DEMOAPP_LORAWANJOINED_BITMASK) {
			bIsJoined = true;
			LED_BLUE_ON;
			printf("Device has joined already\r\n");
		} else {
			bIsJoined = false;
			printf("Device has not joined previously\r\n");
		}
		printf("Band by config header");

		DEMO_PrintConfig();

#if (CERT_APP == 1)
		if (mlsAppIsCertModeEnabled) {
			if (CERT_APP_ACTIVATION_TYPE == LORAWAN_ABP) {
				DEMO_SendMessage();
			} else {
				DEMO_Join(LORAWAN_OTAA);
			}

		} else
#endif /* #if (CERT_APP == 1) */
		{
			DEMO_RunTask();
		}
	} else {
		printf("Failed to restore settings from PDS\r\n");
		DEMO_RunTask();
	}
}
#endif

//------------------------------------------------------------------------------

#ifdef CRYPTO_DEV_ENABLED

void DEMO_PrintECCInfo(void)
{
	ATCA_STATUS xStat;
	uint8_t pucSerialNumber[9];
	uint8_t pucInfo[2];
	uint8_t ucTKMInfo[10];
	int lSlot = 10;
	int lOffset = 70;
	uint8_t pucAppEUI[8];
	uint8_t pucDevEUIASCII[16];
	uint8_t pucDevEUIDecoded[8];
	size_t xBinSize = sizeof(pucDevEUIDecoded);

	delay_ms(5);
	(void) xStat;

	/* read the serial number */
	xStat = atcab_read_serial_number(pucSerialNumber);
	printf("\r\n------------------------------\r\n");

	/* read the SE_INFO */
	xStat = atcab_read_bytes_zone(ATCA_ZONE_DATA, lSlot, lOffset, pucInfo, sizeof(pucInfo));

	/* Read the CustomDevEUI */
	xStat = atcab_read_bytes_zone(ATCA_ZONE_DATA, DEV_EUI_SLOT, 0, pucDevEUIASCII, 16);
	atcab_hex2bin((char*) pucDevEUIASCII, strlen((char*) pucDevEUIASCII), pucDevEUIDecoded, &xBinSize);

	/* Print DevEUI */
	printf("DEV EUI:  ");
#if (SERIAL_NUM_AS_DEV_EUI == 1)
	MlsAppPrintArrayU8(pucSerialNumber, sizeof(pucSerialNumber) - 1);
#else
	MlsAppPrintArrayU8(pucDevEUIDecoded, sizeof(pucDevEUIDecoded));
#endif

	/* Read the AppEUI */
	xStat = atcab_read_bytes_zone(ATCA_ZONE_DATA, APP_EUI_SLOT, 0, pucAppEUI, 8);
	printf("APP EUI:  ");
	MlsAppPrintArrayU8(pucAppEUI, sizeof(pucAppEUI));

	/* assemble full TKM_INFO */
	memcpy(ucTKMInfo, pucInfo, 2);
	memcpy(&ucTKMInfo[2], pucSerialNumber, 8);

	/* ucTKMInfo[] now contains the assembled ucTKMInfo data */
	printf("TKM INFO: ");
	MlsAppPrintArrayU8(ucTKMInfo, sizeof(ucTKMInfo));
	printf("------------------------------\r\n");
}
#endif /* #ifdef CRYPTO_DEV_ENABLED */
//------------------------------------------------------------------------------

#ifdef CONF_PMM_ENABLE

uint32_t ulSleptDuration = UINT32_MAX;

void DEMO_WakeupNotification(uint32_t sleptDuration)
{
	ulSleptDuration = sleptDuration;

	DEMO_RunTask();
}
#endif

void DEMO_JoinNotification(StackRetStatus_t xStatus)
{
	if (xStatus != LORAWAN_SUCCESS) {
		LED_RED_OFF;

	} else {
		LED_RED_ON;
		printf("LORAWAN JOIN FAILED");
	}
}