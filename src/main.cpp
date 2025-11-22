#ifndef _UTILS_HPP_
#define _UTILS_HPP_
#include "system/utils.hpp"
#endif

#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_MANAGER_HPP_

#ifndef _ENEMY_HPP_
#define _ENEMY_HPP_
#include "object/enemy.hpp"
#endif // _ENEMY_HPP_s

#ifndef _ENEMY_SHOOT_SCRIPT_01_HPP_
#define _ENEMY_SHOOT_SCRIPT_01_HPP_
#include "object/enemyShootScripts/01.hpp"
#endif // _ENEMY_SHOOT_SCRIPT_01_HPP_

#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_
#include "object/player.hpp"
#endif // _PLAYER_HPP_

#ifndef _STAGE_HPP_
#define _STAGE_HPP_
#include "object/stage.hpp"
#endif

#ifndef _UI_HPP_
#define _UI_HPP_
#include "system/UI.hpp"
#endif

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    SetUp();

    // 各クラスの宣言
    UI ui;
    ui.getImage();

    // TODO 以下,タイトル追加で変わりそう??

    BombInfo bombs[MAX_BOMBS];
    BombManager bMgr(bombs);

    StageManager sMgr(1, 0, 1);

    Player player;
    EnemyShootScript *enemyShootScript;

    int time = 0;
    int difficulty = 4;

    bool title = true;
    bool flag = false;

    // TODO :Loading
    // Title
    while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && UpdateKey() == 0)
    {
        if (!Update())
        {
            break;
        }
        if (Key[KEY_INPUT_1] == 1)
        {
            break;
        }
        DrawFormatString(30, 30, GetColor(255, 255, 255), L"Title");

        Wait();
    }

    while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && UpdateKey() == 0)
    {
        if (!Update())
        {
            break;
        }
        ui.loadingScreen();
        if (GetASyncLoadNum() == 0 && ui.minLoadingTime > 120)
        {
            bMgr.setBombsHandle(ui.bombsImageHandle);
            break;
        }
        Wait();
    }

    // TODO うまくいってないポイ??
    player.loadPlayerImage();
    sMgr.loadEnemy();
    while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && UpdateKey() == 0)
    {
        clsDx();
        if (!Update())
        {
            break;
        }

        ui.drawUI(sMgr);

        if (Key[KEY_INPUT_0] == 1)
        {
            player.setpower();
        }

        if (sMgr.isTalk)
        {
            ui.talkUI(sMgr.getTalkString(sMgr.talkCount), sMgr.getTalkWho(sMgr.talkCount));
            if (Key[KEY_INPUT_RETURN] == 1)
            {
                sMgr.talkCount++;
            }
            if (sMgr.talkCount == 7)
            {
                sMgr.endTalk();
            }
        }
        sMgr.updateStage(&bMgr, bombs, &player);
        bMgr.DEBUG_printAllBombs(bombs);
        // sMgr.DEBUG_print_enemies();
        Draw();
        Wait();
    }

    DxLib_End();
    return 0;
}
