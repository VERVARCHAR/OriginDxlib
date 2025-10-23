#include "DxLib.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    ChangeWindowMode(TRUE);
    SetDrawScreen(DX_SCREEN_BACK);
    SetWaitVSyncFlag(TRUE);

    if (DxLib_Init() == -1)
        return -1;

    while (ProcessMessage() == 0)
    {
        ClearDrawScreen();

        // ★ ここを L"..." に
        DrawString(16, 16, L"Hello DXLib", GetColor(255, 255, 255));
        DrawFormatString(16, 40, GetColor(255, 255, 0), L"FPS: %f", GetFPS());
        // ← 書式も L"..."

        ScreenFlip();
    }

    DxLib_End();
    return 0;
}
