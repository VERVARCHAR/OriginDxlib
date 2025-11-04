#ifndef _UTILS_HPP_
#define _UTILS_HPP_
#include "system/utils.hpp"
#endif

#ifndef _UI_HPP_
#define _UI_HPP_
#include "system/UI.hpp"
#endif

UI::UI()
{
}

UI::~UI()
{
}

void UI::getImage(bool * /*title*/)
{
    // 非同期ON → 一度だけ投入
    SetUseASyncLoadFlag(TRUE);
    titleHandle = LoadGraph(L"../../img/BackGround03.png");
    LoadDivGraph(L"../../img/Bombs.png", 16, 4, 4, 256, 256, bombsImageHandle);

    int shown_ms = 0;
    for (;;)
    {
        if (ProcessMessage() != 0)
            break;

        // ★毎フレーム：描画ステートを初期化しておく（外部の副作用を打ち消す）
        SetDrawScreen(DX_SCREEN_BACK);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        SetDrawBright(255, 255, 255);
        SetUseZBufferFlag(FALSE);
        SetWriteZBufferFlag(FALSE);
        // SetUseAlphaTest(FALSE);

        // ★ここで「クリア→描画→フリップ」
        ClearDrawScreen();

        int remain = GetASyncLoadNum();
        bool r1 = (CheckHandleASyncLoad(titleHandle) == 0);
        bool r2 = (CheckHandleASyncLoad(bombsImageHandle[0]) == 0);

        // ログ＆プログレス
        DrawBox(24, 24, 640, 140, GetColor(40, 40, 60), TRUE);
        DrawBox(24, 24, 640, 140, GetColor(120, 120, 160), FALSE);
        DrawString(32, 40, L"Loading...", GetColor(255, 255, 255));

        int ready = (r1 ? 1 : 0) + (r2 ? 1 : 0);
        int percent = ready * 50; // 2リソース=100%

        DrawBox(32, 90, 432, 110, GetColor(60, 60, 70), TRUE);
        DrawBox(32, 90, 32 + 4 * percent, 110, GetColor(180, 220, 255), TRUE);
        DrawFormatString(440, 88, GetColor(255, 255, 255), L"%3d%%  remain=%d", percent, remain);

        ScreenFlip();
        WaitTimer(16);
        shown_ms += 16;

        if (remain == 0 && r1 && r2 && shown_ms >= 500)
            break;
    }
    SetUseASyncLoadFlag(FALSE); // 以降は同期（投げた分は非同期のまま進む）
}

void UI::loadingScreen()
{
}

void UI::drawUI()
{
    DrawGraph(0, 0, titleHandle, TRUE);

    for (int i = 0; i < 16; i++)
    {
        DrawGraph(i * 300, 200, bombsImageHandle[i], TRUE);
    }
}

// PlayerStatus UI::getPlayerStatus()
// {
// }

// EnemyStatus UI::getEnemyStatus()
// {
//     return
// }
