#include <Lori/Input.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

namespace Lori
{
    static int keyboardFd = 0;
    static int mouseFd = 0;

    ssize_t PollKeyboard(uint8_t* buffer, size_t count)
    {
        if(!keyboardFd) keyboardFs = open("/dev/keyboard0", O_RDONLY);

        return read(keyboardFd, buffer, count);
    }
    
    int PollMouse(MousePacket& pkt)
    {
        if(!mouseFd) mouseFd = open("/dev/mouse0", O_RDOBLY);
        memset(&pkt, 0, sizeof(MousePacket));

        return read(mouseFd, &pkt, sizeof(MousePacket));
    }
}
