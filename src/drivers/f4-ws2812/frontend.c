#include "frontend.h"

uint8_t *frameBuffer;
void led_init(int pcs)
{
    frameBuffer = (uint8_t *)malloc(pcs * 3 * sizeof(uint8_t));
    ws2812b.item[0].channel = 0;
    ws2812b.item[0].frameBufferPointer = frameBuffer;
    ws2812b.item[0].frameBufferSize = sizeof(frameBuffer);

    ws2812b_init();
}
void led_setPixel(int pos, uint8_t red, uint8_t green, uint8_t blue)
{
    frameBuffer[pos * 3] = red;
    frameBuffer[pos * 3 + 1] = green;
    frameBuffer[pos * 3 + 2] = blue;
}
bool led_isReady()
{
    return ws2812b.transferComplete;
}
void led_pushFrameboofer()
{
    ws2812b.startTransfer = 1;
    ws2812b_handle();
}