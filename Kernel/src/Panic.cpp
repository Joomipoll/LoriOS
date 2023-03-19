#include <Video/Video.h>

#include <APIC.h>
#include <CString.h>
#include <IDT.h>
#include <Logging.h>

const video_mode_t vmt = Video::GetVideoMode();
const int pos = 5;

void PrintReason(const video_mode_t& v, int& pos, const char* reason)
{
    Video::DrawString(reason, vmt.width / 2 - strlen(reason) * 8 / 2, pos, 180, 0, 0);
    pos += 10;
}

void KernelPanic(const char** reasons, int reasonCount)
{
    asm volatile("cli");

    APIC::Local::SendIPI(0, ICR_DSH_OTHER, ICR_MESSAGE_TYPE_FIXED, IPI_HALT);

    Video::DrawRect(0, 0, vmt.width, vmt.height, 0, 0, 0);
    for (int i = 0; i < reasonCount; i++) {
        Video::DrawString(reasons[i], 5, pos, 255, 255, 255);
        pos += 10;
    }

    Video::DrawString("Fatal Error", vmt.width / 2 - strlen("Fatal Error") * 16 / 2, vmt.height / 2, 255, 255, 255, 3, 2);

    Video::DrawString("LoriOS has encountered a fatal error", 0, vmt.height - 200, 255, 255, 255);
    Video::DrawString("Work suspended", 0, vmt.height - 200 + 8, 255, 255, 255);

    if (Log::console) { Log::console->Update(); }

    asm volatile("hlt");
}
