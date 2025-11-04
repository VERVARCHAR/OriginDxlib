#ifndef _UTILS_HPP_
#define _UTILS_HPP_
#include "system/utils.hpp"
#endif

#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_MANAGER_HPP_

#ifndef __ENEMY_HPP_
#define _ENEMY_HPP_
#include "object/enemy.hpp"
#endif // __ENEMY_HPP_s

#ifndef _ENEMY_SHOOT_SCRIPT_01_HPP_
#define _ENEMY_SHOOT_SCRIPT_01_HPP_
#include "object/enemyShootScripts/01.hpp"
#endif // _ENEMY_SHOOT_SCRIPT_01_HPP_

#ifndef _PLAYER_HPP__
#define _PLAYER_HPP_
#include "object/player.hpp"
#endif // _PLAYER_HPP_

#ifndef _STAGE_HPP_
#define _STAGE_HPP_
#include "object/stage.hpp"
#endif

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    SetUp();

    BombInfo bombs[MAX_BOMBS];
    BombManager bMgr(bombs);

    StageManager sMgr(1, 0, 4);

    Player player;
    EnemyShootScript enemyShootScript;
    int time = 0;
    int difficulty = 4;

    bool flag = false;

    sMgr.loadEnemy();
    while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && UpdateKey() == 0)
    {
        clsDx();
        if (!Update())
        {
            break;
        }

        if (Key[KEY_INPUT_0] == 1)
        {
            player.setpower();
        }
        if (Key[KEY_INPUT_1] == 1)
        {
            flag = true;
        }
        if (flag)
        {
            sMgr.updateStage(&bMgr, bombs, &player);
        }
        bMgr.DEBUG_printAllBombs(bombs);

        Draw();
        Wait();
    }
    DxLib_End();
    return 0;
}
