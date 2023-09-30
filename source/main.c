#include <3ds.h>
#include <stdio.h>
#include <string.h>

Result nfc_test() {
	Result ret = 0;
	u32 pos;
	u32 i;
	size_t tmpsize;
	FILE *f;
	bool ableToSave = false;

	u8 miiData[0x60];
	char tmpstr[64];
	NFC_TagState prevstate, curstate;
	NFC_AmiiboSettings amiibosettings;

	printf("NFC initialized successfully, starting scanning...\n");

	ret = nfcStartScanning(NFC_STARTSCAN_DEFAULTINPUT);
	if (R_FAILED(ret)) {
		printf("Failed to start scanning.\nMake sure your wireless communication is enabled!\n");
		return ret;
	}

	ret = nfcGetTagState(&curstate);
	if (R_FAILED(ret)) {
		printf("Failed to get NFC tag state.\n");
		nfcStopScanning();
		return ret;
	}

	prevstate = curstate;

	printf("Scan your amiibo to view the owner's Mii.\n");
	printf("Press B to stop scanning or Y to restart scanning.\n");

	while(1) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();

		if (kDown & KEY_B) {
			ret = 0xffffffff;
			break;
		}

		if (kDown & KEY_A && ableToSave) {
			printf("Requested the Mii to be saved.\n");

			memset(tmpstr, 0, sizeof(tmpstr));
			snprintf(tmpstr, sizeof(tmpstr)-1, "amiibo_mii_out.bin");
			f = fopen(tmpstr, "w");

			if (f) {
				tmpsize = fwrite(miiData, 1, sizeof(miiData), f);
				fclose(f);

				if (tmpsize != sizeof(miiData)) {
					printf("Failed to write all data to the file.\nOnly 0x%x of 0x%x bytes were written.\n", tmpsize, sizeof(miiData));
				} else {
					printf("Writing finished. Wrote to amiibo_mii_out.bin\n");
				}
			} else {
				printf("Failed to open the file for writing.\n");
			}
		}

		if (kDown & KEY_Y) {
			nfcStopScanning();
			ret = nfcStartScanning(NFC_STARTSCAN_DEFAULTINPUT);
			if (R_FAILED(ret)) {
				printf("Failed to start scanning.\nMake sure your wireless communication is enabled!\n");
				return ret;
			}
			printf("Scanning restarted...\nPress B to stop scanning or Y to restart scanning.\n");
			continue;
		}

		ret = nfcGetTagState(&curstate);
		if (R_FAILED(ret)) {
			printf("Failed to get the NFC tag state.\n");
			break;
		}

		if (curstate != prevstate) {
			prevstate = curstate;

			if (curstate == NFC_TagState_InRange) {
				printf("amiibo in range, please do not remove...\n");
				ableToSave = false;
				memset(&amiibosettings, 0, sizeof(NFC_AmiiboSettings));

				ret = nfcLoadAmiiboData();
				if (R_FAILED(ret)) {
					printf("Failed to get amiibo data.\nYou might have removed the amiibo too fast.\n");
					printf("Press Y to try again or B to quit.\n");
					continue;
				}

				memset(&amiibosettings, 0, sizeof(NFC_AmiiboSettings));

				ret = nfcGetAmiiboSettings(&amiibosettings);
				if (R_FAILED(ret)) {
					if (ret == NFC_ERR_AMIIBO_NOTSETUP) {
						printf("This amiibo wasn't setup by the amiibo Settings applet.\n");
					} else {
						printf("Failed to get amiibo settings.\n");
					}
					break;
				}

				memset(&miiData, 0, sizeof(amiibosettings.mii));
				printf("Owner Mii Data (length of 0x%02X):\n", sizeof(amiibosettings.mii));
				for (i = 0; i < 6; i++) {
					printf("%02lX | ", (0x10 * i));
					for (pos = 0; pos < 16; pos++) {
						printf("%02X", amiibosettings.mii[pos + (0x10 * (i))]);
					}
					printf("\n");
				}
				memcpy(&miiData, amiibosettings.mii, 0x60);

				ableToSave = true;
				printf("\nFinished reading the amiibo. You may remove the\namiibo from the scanner now.\n\nPress A to save the amiibo's Mii.\nPress Y to scan another or B to exit.\n");
			}
		}
	}

	nfcStopScanning();

	return ret;
}

int main() {
	Result ret = 0;
	bool exiting = false;

	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	printf("Xenona's amiibo Mii Extractor v0.2 [3DS]\nBuilt off of the libctru NFC example.\n");

	ret = nfcInit(NFC_OpType_NFCTag);
	if (R_FAILED(ret)) {
		printf("Failed to init NFC\n");
	} else {
		ret = nfc_test();
		nfcExit();
		exiting = ret == 0xffffffff;
	}

	printf("Press START or B to exit.\n");

	// Main loop
	if (!exiting) {
		while (aptMainLoop()) {
			gspWaitForVBlank();
			hidScanInput();

			u32 kDown = hidKeysDown();

			if (kDown & KEY_START || kDown & KEY_B) break;
		}
	}

	gfxExit();
	return 0;
}