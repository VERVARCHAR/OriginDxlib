#pragma once
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
#include "object/enemyShootScripts/enemyShootScripts.hpp"
#endif // _ENEMY_SHOOT_SCRIPT_01_HPP_

#ifndef _PLAYER_HPP__
#define _PLAYER_HPP_
#include "object/player.hpp"
#endif // _PLAYER_HPP_

#define MAX_ENEMYS 128

class Enemy;

typedef struct LoadEnemyStatus
{
    int time;
    EnemyStatus enemyStatus;
} loadEnemyStatus;

class StageManager
{
private:
    int stage;
    int time;
    int difficulty;
    int latestEnemyId;
    Enemy *enemys[MAX_ENEMYS];
    EnemyShootScript enemyShootScript;
    LoadEnemyStatus loadEnemyStatus[MAX_ENEMYS];

public:
    StageManager(int _stage, int _time, int _difficulty);
    ~StageManager();

    void loadEnemy();
    void spwanEnemy(int index, EnemyStatus enemyStatus);
    void deleteEnemy(int);
    void updateStage(BombManager *bMgr, BombInfo bombs[MAX_BOMBS], Player *player);
    int getEmptyIndex(); // 空いてる敵のインデックス取得
};