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

void UI::getImage()
{
    // 非同期ON → 一度だけ投入
    SetUseASyncLoadFlag(TRUE);
    titleHandle = LoadGraph(L"../../img/BackGround03.png");
    LoadDivGraph(L"../../img/little_bomb.png", 14, 14, 1, 24, 24, bombsImageHandle);
    LoadDivGraph(L"../../img/large_bomb.png", 14, 3, 4, 48, 48, &bombsImageHandle[14]);
    logoHandle = LoadGraph(L"../../img/logos.png");
    SetUseASyncLoadFlag(FALSE);
}

void loadTitleImages()
{
}

void UI::loadingScreen()
{

    int remain = GetASyncLoadNum();
    // bool r1 = (CheckHandleASyncLoad(titleHandle) == 0);
    // bool r2 = (CheckHandleASyncLoad(bombsImageHandle[15]) == 0);

    // ログ＆プログレス
    // DrawBox(24, 24, 640, 140, GetColor(40, 40, 60), TRUE);
    // DrawBox(24, 24, 640, 140, GetColor(120, 120, 160), FALSE);
    // DrawString(32, 40, L"Loading...", GetColor(255, 255, 255));

    // int ready = (r1 ? 1 : 0) + (r2 ? 1 : 0);
    // int percent = ready * 50; // 2リソース=100%

    // DrawBox(32, 90, 432, 110, GetColor(60, 60, 70), TRUE);
    // DrawBox(32, 90, 32 + 4 * percent, 110, GetColor(180, 220, 255), TRUE);
    // DrawFormatString(440, 88, GetColor(255, 255, 255), L"%3d%%  remain=%d", percent, remain);

    // std::wstring loadStr = "少女祈祷中";
    // printfDx(loadStr.c_str());
    DrawFormatString(WINDOW_WIDTH - 100, WINDOW_HEIGHT - 100, GetColor(255, 255, 255), L"少女祈祷中");
    // TODO この処理行ける???
    // DrawFormatString(WINDOW_WIDTH - 100, WINDOW_HEIGHT - 50, GetColor(255, 255, 255), for (int i = 0; i < (int)(minLoadingTime / 10) % 30) L".");

    minLoadingTime++;
}

void UI::drawUI(StageInfo stageInfo)
{
    // Play Screen
    DrawGraph(0, 0, titleHandle, TRUE);
    DrawBox(30, 30, (int)(WINDOW_WIDTH * 0.6) - 30, WINDOW_HEIGHT - 30, GetColor(0, 0, 0), TRUE);

    // TODO
    // Score Screen
    DrawFormatString(WINDOW_WIDTH - 400, 50, GetColor(255, 255, 255), L"%d", stageInfo.difficulty);
    DrawFormatString(WINDOW_WIDTH - 400, 100, GetColor(255, 255, 255), L"score\t: %d", stageInfo.score);
    DrawFormatString(WINDOW_WIDTH - 400, 150, GetColor(255, 255, 255), L"残機\t: %d", stageInfo.nowStatus.lives);
    DrawFormatString(WINDOW_WIDTH - 400, 200, GetColor(255, 255, 255), L"ボム\t: %d", stageInfo.nowStatus.spels);

    DrawGraph(WINDOW_WIDTH - 400, WINDOW_HEIGHT - 400, logoHandle, TRUE);
}

void UI::talkUI(string talkString, int talkWho)
{
    if (talkWho == 0)
    {
        DrawBox(330, 200, (int)(WINDOW_WIDTH * 0.6) - 30, 500, GetColor(255, 0, 0), TRUE);
    }
    else
    {
        DrawBox(30, 150, 330, 450, GetColor(0, 255, 0), TRUE);
    }
    std::wstring talkW = std::wstring(talkString.begin(), talkString.end());
    printfDx(talkW.c_str());
    DrawFormatString(100, 600, GetColor(255, 255, 255), talkW.c_str());
}
