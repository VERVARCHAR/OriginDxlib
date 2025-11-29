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
    SetUseASyncLoadFlag(TRUE);

    SetUseASyncLoadFlag(FALSE);
}

void UI::getInGameImage()
{
    titleHandle = LoadGraph(L"..\\..\\assets\\background\\BackGround03.png");
    LoadDivGraph(L"..\\..\\assets\\bombs\\MiddleBomb.png", 8, 8, 1, 512, 512, bombsImageHandle);
    LoadDivGraph(L"..\\..\\assets\\bombs\\LargeBomb.png", 8, 8, 1, 512, 512, &bombsImageHandle[8]);
    logoHandle = LoadGraph(L"..\\..\\assets\\logos.png");
    lifeImageHandle = LoadGraph(L"..\\..\\assets\\others\\life.png");
    spellImageHandle = LoadGraph(L"..\\..\\assets\\others\\spells.png");
    LoadDivGraph(L"..\\..\\assets\\others\\difficulty.png", 4, 1, 4, 512, 200, difficultyImageHandle);
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

    DrawFormatString(WINDOW_WIDTH - 100, WINDOW_HEIGHT - 100, GetColor(255, 255, 255), L"少女祈祷中");

    minLoadingTime++;
}

void UI::drawUI(StageInfo stageInfo)
{
    // 背景
    DrawGraph(0, 0, titleHandle, TRUE);

    // プレイエリア（左側）: 少し余白を残す
    const int playLeft = 10;
    const int playTop = 10;
    const int playRight = (int)(WINDOW_WIDTH * 0.6) - 10; // ちょっとだけ狭めて縁取り
    const int playBottom = WINDOW_HEIGHT - 10;
    DrawBox(playLeft, playTop, playRight, playBottom, GetColor(0, 0, 0), TRUE);

    // ==============================
    // 右側 UI パネル
    // ==============================
    const int uiLeft = playRight + 20;     // プレイエリアと少し隙間をあける
    const int uiRight = WINDOW_WIDTH - 30; // 右端にも余白
    const int uiTop = 30;
    const int uiBottom = WINDOW_HEIGHT / 2 + 50; // HUD は上のほうだけ使う

    // パネル背景と枠線
    DrawBox(uiLeft, uiTop, uiRight, uiBottom, GetColor(0, 0, 0), TRUE);
    DrawBox(uiLeft, uiTop, uiRight, uiBottom, GetColor(80, 80, 160), FALSE);

    // テキストのベース座標
    int textX = uiLeft + 20;
    int textY = uiTop + 20;

    // ==============================
    // 難易度
    // ==============================
    DrawFormatString(textX, textY, GetColor(255, 255, 255), L"DIFFICULTY");
    textY += 20;

    // 元コードと同じサイズ (256 x 100) で配置
    const int diffW = 256;
    const int diffH = 100;
    int diffX1 = uiRight - diffW - 20;
    int diffY1 = uiTop + 10;
    int diffX2 = diffX1 + diffW;
    int diffY2 = diffY1 + diffH;

    DrawExtendGraph(
        diffX1, diffY1,
        diffX2, diffY2,
        difficultyImageHandle[stageInfo.difficulty - 1],
        TRUE);

    // ==============================
    // スコア
    // ==============================
    textY = diffY2 + 10;
    DrawFormatString(textX, textY, GetColor(255, 255, 255), L"SCORE : %d", stageInfo.score);
    textY += 30;

    // ==============================
    // ライフ（星アイコン）
    // ==============================
    DrawFormatString(textX, textY, GetColor(255, 255, 255), L"LIFE");
    textY += 20;

    const int iconSize = 50; // 元コードと同じ 50x50
    const int lifeBaseX = textX + 80;
    const int lifeY1 = textY;
    const int lifeY2 = lifeY1 + iconSize;

    for (int i = 0; i < stageInfo.nowStatus.lives; i++)
    {
        int x1 = lifeBaseX + i * (iconSize + 6);
        int x2 = x1 + iconSize;
        DrawExtendGraph(x1, lifeY1, x2, lifeY2, lifeImageHandle, TRUE);
    }
    textY = lifeY2 + 10;

    // ==============================
    // ボム（ハートアイコン）
    // ==============================
    DrawFormatString(textX, textY, GetColor(255, 255, 255), L"BOMB");
    textY += 20;

    const int spellBaseX = textX + 80;
    const int spellY1 = textY;
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
    DrawFormatString(textX, textY, GetColor(255, 255, 255),
                     L"POWER : %lf / 5.00", stageInfo.nowStatus.power);

    textY += 10;

    // ==============================
    // グレイス表示
    // ==============================
    DrawFormatString(textX, textY, GetColor(255, 255, 255),
                     L"GRAZE : %d", stageInfo.nowStatus.grazeCount);

    // ロゴ
    DrawGraph(WINDOW_WIDTH - 400, WINDOW_HEIGHT - 300, logoHandle, TRUE);
    // [DEBUG]
    // printfDx(L"lifeHandle : %d , spellHandle : %d\n", lifeImageHandle, spellImageHandle);
}

void UI::drawBossStatus(EnemyStatus enemyStatus)
{
    if (!enemyStatus.isAlive)
        return;

    const int barWidth = 630;
    const int barHeight = 5;
    const int barX = 100;
    const int barY = 20;

    double ratio = (double)enemyStatus.hp / enemyStatus.maxHp;
    int currentWidth = (int)(barWidth * ratio);

    DrawFormatString(10, barY, GetColor(255, 255, 255), L"%s", enemyStatus.name.c_str());

    // 背景（グレー）
    DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(80, 80, 80), TRUE);

    // 赤ゲージ
    DrawBox(barX, barY, barX + currentWidth, barY + barHeight, GetColor(255, 60, 60), TRUE);

    // 枠
    // DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(255, 255, 255), FALSE);
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
    // [DEBUG]
    printfDx(talkW.c_str());
    DrawFormatString(100, 600, GetColor(255, 255, 255), talkW.c_str());
}

void UI::drawSpellCardText(EnemyStatus enemyStatus, SpellInfo spellInfo, int time)
{
    if (enemyStatus.isSpell)
    {
        if (enemyStatus.time < 120)
        { // 表示120フレーム
            DrawFormatString(240, 100, GetColor(255, 128, 255), L"Spell Card!!");
            DrawFormatString(240, 130, GetColor(255, 255, 255), spellInfo.spellName.c_str());
        }
    }
}