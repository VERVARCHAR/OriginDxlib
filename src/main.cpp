#include "system/utils.hpp"

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

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    SetUp();

    BombInfo bombs[MAX_BOMBS];
    BombManager bMgr(bombs);
    Enemy enemy;
    Player player;
    EnemyShootScript enemyShootScript;
    int time = 0;
    int difficulty = 1;

    while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && UpdateKey() == 0)
    {
        clsDx();
        if (!Update())
        {
            break;
        }

        enemyShootScript.BombType01(enemy, bMgr, bombs, time, difficulty);
        // enemyShootScript.getBombInfo(bombs);
        printfDx(L"%d\n", player.getHP());
        printfDx(L"%d\n", enemy.getHP());
        enemy.enemyUpdate(time, difficulty, bMgr, bombs);
        player.playerUpdate(bMgr, bombs);
        bMgr.updateBombs(bombs);
        bMgr.drawBombs(bombs);
        // bMgr.DEBUG_printAllBombs(bombs);

        time++;
        // printfDx(L"%d\n", time);
        Draw();
        Wait();
    }
    DxLib_End();
    return 0;
}
