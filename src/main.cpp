#include "system/utils.hpp"
#include "object/bomb.hpp"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    SetUp();

    BombManager bombs;
    int times = 0;

    while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && UpdateKey() == 0)
    {
        if (!Update())
        {
            break;
        }

        if (times % 60 == 0)
        {
            bombs.setBomb(bombs.getEmptyIndex(), 100, 100, 2, 2, 30, 10);
        }

        bombs.drawBombs();
        bombs.DEBUG_printAllBombs();
        times++;
        Draw();
        Wait();
    }
    DxLib_End();
    return 0;
}
