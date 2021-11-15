#include "frontend.h"

uint8_t *frameBuffer;
int led_pcs;
void led_init(int pcs)
{
    led_pcs = pcs;
    frameBuffer = (uint8_t *)pvPortMalloc(pcs * 3 * sizeof(uint8_t));
    led_cleanFrameboofer();
    ws2812b.item[0].channel = 0;
    ws2812b.item[0].frameBufferPointer = frameBuffer;
    ws2812b.item[0].frameBufferSize = pcs*3;//WARNING. POSSIBLE PROBLEMS. NEED RESEARCH

    ws2812b_init();
}
void led_setPixel(int pos, uint8_t red, uint8_t green, uint8_t blue)
{
    frameBuffer[pos * 3] = green;
    frameBuffer[pos * 3 + 1] = red;
    frameBuffer[pos * 3 + 2] = blue;
}
void led_cleanFrameboofer()
{
    for(int i = 0; i < led_pcs*3; i++) frameBuffer[i] = 0;
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