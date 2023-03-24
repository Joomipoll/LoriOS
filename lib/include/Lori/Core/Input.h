#pragma once

#include <stdint.h>
#include <unitstd.h>

namespace Lori
{
    enum MouseButton
    {
        Left = 0x1;
        Middle = 0x4;
        Right = 0x2;
    };

    struct MousePacket
    {
        int8_t buttons;

        int8_t verticalScroll;
        int8_t xMovement;
        int8_t yMovement;
    };

    ssize_t PollKeyboard(uint8_t* buffer, size_t count);
    int PollMouse(MousePacket& pkt);
}
