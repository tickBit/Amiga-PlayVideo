#include <exec/types.h>
#include <exec/memory.h>
#include <exec/exec.h>

#include <intuition/intuition.h>
#include <intuition/screens.h>

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/dos.h>

#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <graphics/rastport.h>

#include <devices/timer.h>
#include <proto/timer.h>

#include <stdio.h>
#include <stdlib.h>

#include <libraries/picasso96.h>
#include <proto/picasso96.h>

// change these as you need them
#define FRAMES_PER_SECOND 7.0
#define WIDTH 512
#define HEIGHT 512

struct Library *DataTypesBase = NULL;
struct Library *IntuitionBase = NULL;
struct Library *GfxBase = NULL;
struct Library *P96Base = NULL;

struct MsgPort *TimerMP;
struct timerequest *TimerIO;
struct Device *TimerBase = NULL;

int main()
{
    struct Window *win = NULL;
    struct Screen *scr = NULL;
    struct IntuiMessage *msg = NULL;
    BPTR fh;
    UBYTE *frame_ = NULL;
    
    TimerMP = CreateMsgPort();
    TimerIO = (struct timerequest *)
        CreateIORequest(TimerMP, sizeof(struct timerequest));

    if (OpenDevice(TIMERNAME, UNIT_ECLOCK,
                (struct IORequest *)TimerIO, 0) == 0)
    {
        TimerBase = TimerIO->tr_node.io_Device;
    }
    
    /* --- open libs --- */
    IntuitionBase = OpenLibrary("intuition.library", 39);
    GfxBase       = OpenLibrary("graphics.library", 39);
    DataTypesBase = OpenLibrary("datatypes.library", 39);
    P96Base       = OpenLibrary("Picasso96API.library",2);
    
    if (!IntuitionBase || !GfxBase || !DataTypesBase || !P96Base)
        goto end;

    /* --- open window --- */
    win = OpenWindowTags(NULL,
        WA_Width, WIDTH,
        WA_Height, HEIGHT,
        WA_IDCMP, IDCMP_CLOSEWINDOW,
        WA_CloseGadget, TRUE,
        WA_SizeGadget, TRUE,
        WA_DepthGadget, TRUE,
        WA_DragBar, TRUE,
        TAG_DONE);

    if (!win)
        goto end;


    scr = LockPubScreen(NULL);

    // put here the video file to play
    // option r is the frame rate
    SystemTags(
        "ffmpeg -r 7 -i videos/video.mp4 "
        "-f rawvideo -pix_fmt bgra "
        "-vf scale=512:512 "
        "-r 7 output/video.raw",
        TAG_DONE
    );
    
    fh = Open("output/video.raw", MODE_OLDFILE);
    if (!fh) goto end;
    
    Seek(fh, 0, OFFSET_BEGINNING);

    // NOTICE THIS: frameSize WIDTH * HEIGHT * 4; * 4 is for
    // videoformats, that have R G B and A(lpha)
    int frameSize = WIDTH * HEIGHT * 4;
    
    frame_ = AllocVec(frameSize, MEMF_ANY);
    if (!frame_) goto end;
    
    struct RenderInfo ri;
    ri.Memory = frame_;
    ri.BytesPerRow = WIDTH * 4;
    ri.RGBFormat = p96GetBitMapAttr(win->RPort->BitMap, P96BMA_RGBFORMAT); // Get RGB format
    
    struct EClockVal start, now;
    ULONG ticksPerSecond;

    ticksPerSecond = ReadEClock(&start);

    ULONG currentFrame = 0;
    ULONG wantedFrame = 0;
    double startTicks;
    double nowTicks;
    double elapsedTicks;
    double elapsedSeconds;
    
    for (;;)
    {
        while ((msg = (struct IntuiMessage *)GetMsg(win->UserPort)))
        {
            ULONG cls = msg->Class;

            ReplyMsg((struct Message *)msg);

            if (cls == IDCMP_CLOSEWINDOW)
                goto end;

        }

        // timing
        ReadEClock(&now);

        startTicks =
            (start.ev_hi << 32) |
            start.ev_lo;

        nowTicks =
            (now.ev_hi << 32) |
            now.ev_lo;

        elapsedTicks = nowTicks - startTicks;

        elapsedSeconds = ((double)elapsedTicks) /
                        (double)ticksPerSecond;

        wantedFrame = (ULONG)(elapsedSeconds *
                            FRAMES_PER_SECOND);
        if (wantedFrame != currentFrame)
        {
            currentFrame = wantedFrame;
            // read video
            Seek(fh, currentFrame * frameSize, OFFSET_BEGINNING);
            LONG r = Read(fh, frame_, frameSize);

            if (r != frameSize)
            {
                Seek(fh, 0, OFFSET_BEGINNING); // loop video
                continue;
            }
        
            // draw to window
            p96WritePixelArray(
                &ri,
                0, 0,
                win->RPort,
                0, 0,
                WIDTH,
                HEIGHT
            );
        }
        
        WaitTOF();
       
    }

end:

    CloseDevice((struct IORequest *)TimerIO);
    DeleteIORequest((struct IORequest *)TimerIO);
    DeleteMsgPort(TimerMP);

    if (frame_) FreeVec(frame_);
    
    if (fh) Close(fh);
    
    if (win)
    {
        CloseWindow(win);
    }
    
    if (scr) UnlockPubScreen(NULL, scr);
    
    if (DataTypesBase)
        CloseLibrary(DataTypesBase);

    if (GfxBase)
        CloseLibrary(GfxBase);

    if (IntuitionBase)
        CloseLibrary(IntuitionBase);

    if (P96Base) CloseLibrary(P96Base);
    
    return 0;
}