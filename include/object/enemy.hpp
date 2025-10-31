#pragma once

#ifndef _DXLIB_HPP_
#define _DXLIB_HPP_
#include "DxLib.h"
#endif // _DXLIB_HPP_

#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_MANAGER_HPP_

#ifndef _ENEMY_SHOOT_SCRIPT_01_HPP_
#define _ENEMY_SHOOT_SCRIPT_01_HPP_
#include "object/enemyShootScripts/01.hpp"
#endif // _ENEMY_SHOOT_SCRIPT_01_HPP_

class EnemyShootScript;

typedef struct EnemyStatus
{
    Vec2d pos;
    Vec2d vel;
    int type; // 種類
    int lives;
    int hp; // 体力
    int radius;
    int shootType;
    short id;
    bool isAlive;
    char name[32]; // 名前
} enemyStatus;

class Enemy
{
private:
    EnemyStatus enemyStatus;

protected:
public:
    // usingBombStruct usingBombs[MAX_USING] = {{0, 0}}; // 使用中の弾幕数

    Enemy(EnemyStatus enemyStatus);
    ~Enemy();

    void setPosition(int _x, int _y);
    Vec2d getPosition();
    EnemyStatus getStatus()
    {
        return enemyStatus;
    }

    void setStatus(EnemyStatus _status)
    {
        enemyStatus = _status;
    }

    int getId() { return enemyStatus.id; };
    void setIsAlive(bool flag) { enemyStatus.isAlive = flag; };
    // void setData(EnemyStatus enemyStatus);
    // void getUsingBombs(UsingBombStruct (&_usingBombs)[MAX_USING][2]) { memcpy(_usingBombs, usingBombs, sizeof(usingBombs)); }
    // void setUsingBombs(UsingBombStruct (&_usingBombs)[MAX_USING][2]) { memcpy(usingBombs, _usingBombs, sizeof(usingBombs)); }
    void getBMgrData(BombManager &_BombManager);
    void enemyUpdate(int, int, BombManager, BombInfo[MAX_BOMBS], EnemyShootScript);
    void shootBomb(EnemyShootScript, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty);
};