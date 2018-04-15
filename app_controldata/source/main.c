#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <switch.h>

//This example shows how to get nsApplicationControlData for a title, which contains nacp/icon. See libnx ns.h.
//TODO: libnx ns impl doesn't support >=3.0.0 currently.

int main(int argc, char **argv)
{
    Result rc=0;

    u64 titleID=0x01007ef00011e000;//titleID for use with nsGetApplicationControlData, in this case BOTW.
    nsApplicationControlData *buf=NULL;
    size_t outsize=0;

    char name[0x201];

    gfxInitDefault();
    consoleInit(NULL);

    buf = (nsApplicationControlData*)malloc(sizeof(nsApplicationControlData));
    if (buf==NULL) {
        rc = MAKERESULT(Module_Libnx, LibnxError_OutOfMemory);
        printf("Failed to alloc mem.\n");
    }
    else {
        memset(buf, 0, sizeof(nsApplicationControlData));
    }

    if (R_SUCCEEDED(rc)) {
        rc = nsInitialize();
        if (R_FAILED(rc)) {
            printf("nsInitialize() failed: 0x%x\n", rc);
        }
    }

    if (R_SUCCEEDED(rc)) {
        rc = nsGetApplicationControlData(1, titleID, buf, sizeof(nsApplicationControlData), &outsize);
        if (R_FAILED(rc)) {
            printf("nsGetApplicationControlData() failed: 0x%x\n", rc);
        }

        if (outsize < sizeof(buf->nacp)) {
            rc = -1;
            printf("Outsize is too small: 0x%lx.\n", outsize);
        }

        if (R_SUCCEEDED(rc)) {
            //TODO: Determine which language to use instead of using hard-coded index 0.
            memset(name, 0, sizeof(name));
            strncpy(name, buf->nacp.lang[0].name, sizeof(buf->nacp.lang[0].name));//Don't assume the nacp string is NUL-terminated for safety.

            printf("Name: %s\n", name);//Note that the print-console doesn't support UTF-8. The name is UTF-8, so this will only display properly if there isn't any non-ASCII characters. To display it properly, a print method which supports UTF-8 should be used instead.

            //You could also load the JPEG icon from buf->nacp.icon. The icon size can be calculated with: iconsize = outsize - sizeof(buf->nacp);
        }

        nsExit();
    }

    free(buf);

    // Main loop
    while(appletMainLoop())
    {
        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
        u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS) break; // break in order to return to hbmenu

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }

    gfxExit();
    return 0;
}

