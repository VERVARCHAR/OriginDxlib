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
#include "object/enemyShootScripts/EnemyShootScripts.hpp"
#endif // _ENEMY_SHOOT_SCRIPT_01_HPP_

#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_
#include "object/player.hpp"
#endif // _PLAYER_HPP_

class EnemyShootScript;
class Player;

typedef struct EnemyStatus
{
    Vec2d pos;
    Vec2d vel;
    int type; // 種類
    int lives;
    int hp; // 体力
    int radius;
    int shootType;
    int spwanTime;
    int time;
    short id;
    bool isAlive;
    char name[32]; // 名前
};

class Enemy
{
private:
protected:
public:
    Enemy(EnemyStatus enemyStatus);
    ~Enemy();

    void setPosition(int _x, int _y);
    Vec2d getPosition();
    EnemyStatus enemyStatus;
    EnemyStatus getStatus()
    {
        return enemyStatus;
    }

    void setStatus(EnemyStatus _status)
    {
        enemyStatus = _status;
    }
    int getTime() { return enemyStatus.time; };

    int getId() { return enemyStatus.id; };
    void setIsAlive(bool flag) { enemyStatus.isAlive = flag; };
    void getBMgrData(BombManager &_BombManager);
    void enemyUpdate(int, int, BombManager *, BombInfo[MAX_BOMBS], EnemyShootScript, Player *player);
    void shootBomb(EnemyShootScript, BombManager *bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty, Player player);
    bool getOnScreen();
    bool getIsAlive(int index);
    bool isHitPlayer(Player *player);
    Vec2d getPos(int index);
};