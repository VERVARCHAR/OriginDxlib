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

    Scene scene = PRELOADING;

    // 各クラスの宣言
    UI ui;
    ui.startLoading();
    ui.getImage();

    while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && UpdateKey() == 0)
    {
        if (!Update())
        {
            break;
        }
        ui.loadingScreen();
        if (GetASyncLoadNum() == 0 && ui.minLoadingTime > 120)
        {
            scene = TITLE;
            break;
        }
        Wait();
        Draw();
    }

    // TODO 以下,タイトル追加で変わりそう??

    BombInfo bombs[MAX_BOMBS];
    BombManager bMgr(bombs);

    StageManager sMgr(1, 0, 1);

    Player player;
    EnemyShootScript *enemyShootScript;

    int time = 0;
    // int difficulty = 4;

    bool title = true;
    bool flag = false;

    // TODO :Loading
    // Title
    while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && UpdateKey() == 0)
    {
        clsDx();

        if (!Update())
        {
            break;
        }
        switch (scene)
        {
        case TITLE:
            if (Key[KEY_INPUT_1] == 1)
            {
                ui.startLoading();
                scene = LOADING;
                break;
            }
            DrawFormatString(30, 30, GetColor(255, 255, 255), L"Title");
            break;

        case LOADING:
            if (ui.minLoadingTime == 0)
            {
                player.init();
                bMgr.init(bombs);
                sMgr.init(1, 0, 1);

                player.loadPlayerImage();
                sMgr.loadEnemy();
                bMgr.setBombsHandle(ui.bombsImageHandle);
            }
            ui.loadingScreen();
            if (GetASyncLoadNum() == 0 && ui.minLoadingTime > 120)
            {
                scene = INGAME;
                break;
            }
            break;
        case INGAME:

            ui.drawUI(sMgr.getStageInfo());

            if (Key[KEY_INPUT_ESCAPE] == 1)
            {
                sMgr.isPause = !sMgr.isPause;
            }

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
            if (sMgr.isPause && !sMgr.isGameOver)
            {
                DrawString(200, 200, L"PAUSE", GetColor(255, 255, 0));
            }

            if (sMgr.isGameOver)
            {
                DrawString(200, 200, L"Game Over...", GetColor(255, 255, 0));
                if (Key[KEY_INPUT_R] == 1)
                {
                    ui.startLoading();
                    scene = LOADING;
                }
            }
            bMgr.DEBUG_printAllBombs(bombs);
            // sMgr.DEBUG_print_enemies();

            break;
        case RESULT:
            break;
        default:
            break;
        }

        Draw();
        Wait();
    }

    DxLib_End();
    return 0;
}
