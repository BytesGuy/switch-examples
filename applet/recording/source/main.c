// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include <switch.h>

//See also libnx applet.h. See applet.h for the requirements for using this.

// Define the desired framebuffer resolution (here we set it to 720p).
#define FB_WIDTH  1280
#define FB_HEIGHT 720

// Remove above and uncomment below for 1080p
//#define FB_WIDTH  1920
//#define FB_HEIGHT 1080

int main(int argc, char **argv)
{
    // Retrieve the default window
    NWindow* win = nwindowGetDefault();

    // Create a linear double-buffered framebuffer
    Framebuffer fb;
    framebufferCreate(&fb, win, FB_WIDTH, FB_HEIGHT, PIXEL_FORMAT_RGBA_8888, 2);
    framebufferMakeLinear(&fb);

    appletInitializeGamePlayRecording();//Normally this is only recording func you need to call.

    u32 cnt = 0;

    while(appletMainLoop())
    {
        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS) break; // break in order to return to hbmenu

        if (kDown & KEY_Y) {
            appletSetGamePlayRecordingState(0);//Disable recording.
        }
        else if (kDown & KEY_X) {
            appletSetGamePlayRecordingState(1);//Enable recording.
        }

        // Retrieve the framebuffer
        u32 stride;
        u32* framebuf = (u32*) framebufferBegin(&fb, &stride);

        if (cnt != 60)
            cnt ++;
        else
            cnt = 0;

        // Each pixel is 4-bytes due to RGBA8888.
        for (u32 y = 0; y < FB_HEIGHT; y ++)
        {
            for (u32 x = 0; x < FB_WIDTH; x ++)
            {
                u32 pos = y * stride / sizeof(u32) + x;
                framebuf[pos] = 0x01010101 * cnt * 4;//Set framebuf to different shades of grey.
            }
        }

        // We're done rendering, so we end the frame here.
        framebufferEnd(&fb);
    }

    framebufferClose(&fb);
    return 0;
}
