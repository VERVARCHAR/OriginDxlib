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
    DeleteFontToHandle(uiFont);
    DeleteFontToHandle(talkFont);
}

void UI::getImage()
{
    SetUseASyncLoadFlag(TRUE);

    SetUseASyncLoadFlag(FALSE);
}

void UI::getInGameImage()
{
    UIImageHandle = LoadGraph(L"..\\..\\assets\\background\\assetsEditor\\Background.PNG");
    LoadDivGraph(L"..\\..\\assets\\bombs\\MiddleBomb.png", 8, 8, 1, 512, 512, bombsImageHandle);
    LoadDivGraph(L"..\\..\\assets\\bombs\\LargeBomb.png", 8, 8, 1, 512, 512, &bombsImageHandle[8]);
    logoHandle = LoadGraph(L"..\\..\\assets\\logo.png");
    lifeImageHandle = LoadGraph(L"..\\..\\assets\\others\\life.png");
    spellImageHandle = LoadGraph(L"..\\..\\assets\\others\\spells.png");
    LoadDivGraph(L"..\\..\\assets\\others\\difficulty.png", 4, 1, 4, 512, 200, difficultyImageHandle);
    stageImageHandle[0] = LoadGraph(L"..\\..\\assets\\background\\st01\\st01.PNG");
    stageImageHandle[1] = LoadGraph(L"..\\..\\assets\\background\\st02.PNG");
    stageImageHandle[2] = LoadGraph(L"..\\..\\assets\\background\\st03.PNG");
    playerImageHandle = LoadGraph(L"..\\..\\assets\\player\\CutIn_PlayerChar01.png");

    uiFont = CreateFontToHandle(L"源真ゴシック", 42, 3, DX_FONTTYPE_ANTIALIASING);
    talkFont = CreateFontToHandle(L"源真ゴシック", 32, 2, DX_FONTTYPE_ANTIALIASING);

    bossCutInImageHandle[0] = LoadGraph(L"..\\..\\assets\\enemy\\cutin_Boss01.png");
    bossCutInImageHandle[1] = LoadGraph(L"..\\..\\assets\\enemy\\cutin_Boss02.png");
    bossCutInImageHandle[2] = LoadGraph(L"..\\..\\assets\\enemy\\cutin_Boss03.png");
}

void UI::loadPreLoadingImages()
{
    loadingHandle = LoadGraph(L"..\\..\\assets\\others\\loading.png");
    preLoadingHandle = LoadGraph(L"..\\..\\assets\\background\\assetsEditor\\PreLoading.PNG");
    titleHandle = LoadGraph(L"..\\..\\assets\\background\\assetsEditor\\Title.PNG");
    LoadDivGraph(L"..\\..\\assets\\effects\\loadingParticle.png", 2, 2, 1, 256, 256, loadingSakuraHandle);
}

void UI::drawPreLoading()
{
    DrawGraph(0, 0, preLoadingHandle, TRUE);
    DrawGraph(WINDOW_WIDTH - 256 - 20, WINDOW_HEIGHT - 144 - 30, loadingSakuraHandle[(minLoadingTime / 12) % 2], TRUE);
    DrawExtendGraph(WINDOW_WIDTH - 256 - 20, WINDOW_HEIGHT - 144 - 20, WINDOW_WIDTH - 20, WINDOW_HEIGHT - 20, loadingHandle, TRUE);
    minLoadingTime++;
}

void UI::loadingScreen()
{
    DrawGraph(WINDOW_WIDTH - 256 - 20, WINDOW_HEIGHT - 144 - 30, loadingSakuraHandle[(minLoadingTime / 12) % 2], TRUE);
    DrawExtendGraph(WINDOW_WIDTH - 256 - 20, WINDOW_HEIGHT - 144 - 20, WINDOW_WIDTH - 20, WINDOW_HEIGHT - 20, loadingHandle, TRUE);
    minLoadingTime++;
}

void UI::drawTitle()
{
    DrawGraph(0, 0, titleHandle, TRUE);
}

void UI::drawUI(StageInfo stageInfo)
{
    // 背景
    const int playLeft = 10;
    const int playTop = 10;
    const int playRight = (int)(WINDOW_WIDTH * 0.6) + 22; // ちょっとだけ狭めて縁取り
    const int playBottom = WINDOW_HEIGHT - 10;

    DrawExtendGraph(0, -684, playRight, WINDOW_HEIGHT, stageImageHandle[stageInfo.stage - 1], TRUE);
    // DrawGraph(0, -10, stageImageHandle[stageInfo.stage - 1], TRUE);
    DrawGraph(0, 0, UIImageHandle, TRUE);
    // プレイエリア（左側）: 少し余白を残す

    if (time < 120)
        drawBorderAllGetItem();

    // ==============================
    // 右側 UI パネル
    // ==============================
    const int uiLeft = playRight + 20;     // プレイエリアと少し隙間をあける
    const int uiRight = WINDOW_WIDTH - 30; // 右端にも余白
    const int uiTop = 30;
    const int uiBottom = WINDOW_HEIGHT / 2 + 50; // HUD は上のほうだけ使う

    // パネル背景と枠線
    // DrawBox(uiLeft, uiTop, uiRight, uiBottom, GetColor(0, 0, 0), TRUE);
    // DrawBox(uiLeft, uiTop, uiRight, uiBottom, GetColor(80, 80, 160), FALSE);

    // テキストのベース座標
    int textX = uiLeft + 20;
    int textY = uiTop + 20;

    // ==============================
    // 難易度
    // ==============================
    // DrawFormatString(textX, textY, GetColor(255, 255, 255), L"DIFFICULTY");
    textY += 20;

    // 元コードと同じサイズ (256 x 100) で配置
    const int diffW = 256;
    const int diffH = 100;
    int diffX1 = uiRight - diffW - 20;
    int diffY1 = uiTop + 10;
    int diffX2 = diffX1 + diffW;
    int diffY2 = diffY1 + diffH;

    // DrawExtendGraph(
    //     922, 10,
    //     1178, 138,
    //     difficultyImageHandle[stageInfo.difficulty - 1],
    //     TRUE);
    DrawGraph(WINDOW_WIDTH - 512 + 5, -50, difficultyImageHandle[stageInfo.difficulty - 1], TRUE);

    // ==============================
    // スコア
    // ==============================
    textY = diffY2 + 10;
    // DrawFormatString(1000, 155, GetColor(255, 255, 255), L"", stageInfo.score);

    if (highScore < stageInfo.score)
    {
        highScore = stageInfo.score;
    }

    DrawFormatStringToHandle(
        999, 99,
        GetColor(255, 255, 255),
        uiFont,
        L"%12d",
        highScore);
    DrawFormatStringToHandle(
        1000, 100,
        GetColor(255, 223, 0),
        uiFont,
        L"%12d",
        highScore);

    DrawFormatStringToHandle(
        999, 154,
        GetColor(255, 255, 255),
        uiFont,
        L"%12d",
        stageInfo.score);
    DrawFormatStringToHandle(
        1000, 155,
        GetColor(255, 223, 0),
        uiFont,
        L"%12d",
        stageInfo.score);

    DrawFormatStringToHandle(
        999, 204,
        GetColor(255, 255, 255),
        uiFont,
        L"%12d",
        stageInfo.nowStatus.grazeCount);
    DrawFormatStringToHandle(
        1000, 205,
        GetColor(128, 255, 128),
        uiFont,
        L"%12d",
        stageInfo.nowStatus.grazeCount);

    const int iconSize = 50; // 元コードと同じ 50x50
    const int lifeBaseX = 1050;
    const int lifeY1 = 280;
    const int lifeY2 = lifeY1 + iconSize;

    for (int i = 0; i < stageInfo.nowStatus.lives; i++)
    {
        int x1 = lifeBaseX + i * (iconSize + 6);
        int x2 = x1 + iconSize;
        DrawExtendGraph(x1, lifeY1, x2, lifeY2, lifeImageHandle, TRUE);
    }
    textY = lifeY2 + 10;

    // ==============================
    // ボム
    // ==============================
    // DrawFormatString(textX, textY, GetColor(255, 255, 255), L"BOMB");
    textY += 20;

    const int spellBaseX = 1050;
    const int spellY1 = 330;
    const int spellY2 = spellY1 + iconSize;

    for (int i = 0; i < stageInfo.nowStatus.spells; i++)
    {
        int x1 = spellBaseX + i * (iconSize + 6);
        int x2 = x1 + iconSize;
        DrawExtendGraph(x1, spellY1, x2, spellY2, spellImageHandle, TRUE);
    }
    textY = spellY2 + 10;

    // ==============================
    // パワー表示
    // ==============================
    // DrawFormatString(1000, 400, GetColor(255, 255, 255),
    //                  L"%lf / 5.00", stageInfo.nowStatus.power);

    DrawFormatStringToHandle(
        999, 389,
        GetColor(255, 255, 255),
        uiFont,
        L"%.2lf / 5.00",
        stageInfo.nowStatus.power);
    DrawFormatStringToHandle(
        1000, 390,
        GetColor(128, 255, 128),
        uiFont,
        L"%.2lf / 5.00",
        stageInfo.nowStatus.power);

    textY += 10;

    // ロゴ
    DrawGraph(WINDOW_WIDTH - 400, WINDOW_HEIGHT - 300, logoHandle, TRUE);
    // [DEBUG]
    // printfDx(L"lifeHandle : %d , spellHandle : %d\n", lifeImageHandle, spellImageHandle);
    time++;
}

void UI::drawBossStatus(EnemyStatus enemyStatus)
{
    if (!enemyStatus.isAlive)
        return;

    const int barWidth = 630;
    const int barHeight = 3;
    const int barX = 100;
    const int barY = 25;

    double ratio = (double)enemyStatus.hp / enemyStatus.maxHp;
    int currentWidth = (int)(barWidth * ratio);

    DrawFormatString(10, barY, GetColor(255, 255, 255), L"%s", enemyStatus.name.c_str());

    for (int i = 0; i < enemyStatus.spellAmount - enemyStatus.spellCount; i++)
    {
        DrawFormatString(10 + i * 15, barY - 10, GetColor(255, 0, 0), L"☆");
    }

    // 背景（グレー）
    DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(80, 80, 80), TRUE);

    // 赤ゲージ
    DrawBox(barX, barY, barX + currentWidth, barY + barHeight, GetColor(255, 60, 60), TRUE);

    // 枠
    // DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(255, 255, 255), FALSE);
}

void UI::talkUI(int stage, std::wstring talkString, int talkWho)
{
    if (talkWho == 0)
    {
        // DrawBox(330, 200, (int)(WINDOW_WIDTH * 0.6) - 30, 500, GetColor(255, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
        DrawGraph(500, 200, bossCutInImageHandle[stage - 1], TRUE);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
        DrawGraph(30, 180, playerImageHandle, TRUE);
    }
    else
    {
        // DrawBox(30, 150, 330, 450, GetColor(0, 255, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
        DrawGraph(30, 200, playerImageHandle, TRUE);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
        DrawGraph(500, 180, bossCutInImageHandle[stage - 1], TRUE);
    }

    // [DEBUG]
    // printfDx(talkString.c_str());
    // DrawFormatString(100, 600, GetColor(255, 255, 255), talkString.c_str());

    DrawBox(10, 400, 780, 600, GetColor(0, 0, 0), TRUE);
    DrawBox(20, 410, 770, 590, GetColor(225, 160, 60), TRUE);
    DrawFormatStringToHandle(
        30, 420,
        GetColor(255, 255, 255),
        talkFont,
        talkString.c_str());
}

void UI::drawSpellCardText(EnemyStatus enemyStatus, SpellInfo spellInfo, int time)
{
    if (enemyStatus.isSpell)
    {
        if (enemyStatus.time < 90)
        {
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
            DrawBox(540, 490, 790, 530, GetColor(0, 0, 0), TRUE);
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
            DrawFormatString(550, 500, GetColor(255, 255, 255), L"%s", spellInfo.spellName.c_str());
        }
    }
}

void UI::drawBorderAllGetItem()
{
    if (time % 60 <= 30)
    {
        const int playLeft = 10;
        const int playRight = (int)(WINDOW_WIDTH * 0.6) + 22;
        DrawFormatString(playRight / 2 + playLeft, 120, GetColor(255, 255, 255), L"Border Item Line!!!");
        DrawLine(playLeft, 120, playRight, 120, GetColor(200, 200, 200), TRUE);
    }
}