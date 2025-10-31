#pragma once

#ifndef _DXLIB_HPP_
#define _DXLIB_HPP_
#include "DxLib.h"
#endif // _DXLIB_HPP_

#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_MANAGER_HPP_

#ifndef _UTILS_HPP_
#define _UTILS_HPP_
#include "system/utils.hpp"
#endif

extern int Key[256];

typedef struct Status
{
    int lives;
    int spels;
    int power;
    bool isSpel;
    bool invincible;
    bool isShoot;
    bool isShift;
    int invincibleTime;
} status;

class Player
{
private:
    Vec2d pos;
    int type; // 種類
    Status status;
    char name[50]; // 名前
    int radius;

protected:
public:
    // usingBombStruct usingBombs[MAX_USING] = {{0, 0}}; // 使用中の弾幕数

    Player();
    ~Player();

    void setpower() { status.power += 1; };

    void setPosition(int _x, int _y);
    Vec2d getPosition();
    Status getStatus()
    {
        return status;
    }
    // void getUsingBombs(UsingBombStruct (&_usingBombs)[MAX_USING][2]) { memcpy(_usingBombs, usingBombs, sizeof(usingBombs)); }
    // void setUsingBombs(UsingBombStruct (&_usingBombs)[MAX_USING][2]) { memcpy(usingBombs, _usingBombs, sizeof(usingBombs)); }
    void getBMgrData(BombManager &_BombManager);
    void playerUpdate(BombManager, BombInfo[MAX_BOMBS]);
    void shootBomb(BombManager bMgr, BombInfo bombs[MAX_BOMBS]);
    void getKeyInput();
    void SpelCard(BombManager, BombInfo[MAX_BOMBS]);
    void ShootBomb(BombManager bMgr, BombInfo bombs[MAX_BOMBS]);
    void Dead();
    void debugStatus()
    {
        printfDx(L"lives : %d\n", status.lives);
        printfDx(L"spels : %d\n", status.spels);
        printfDx(L"power : %d\n", status.power);
        printfDx(L"isInv : %d\n", status.invincible);
        printfDx(L"InvT  : %d\n", status.invincibleTime);
    };
};