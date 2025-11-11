#pragma once

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
#include "object/enemyShootScripts/enemyShootScripts.hpp"
#endif // _ENEMY_SHOOT_SCRIPT_01_HPP_

#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_
#include "object/player.hpp"
#endif // _PLAYER_HPP_

#include <vector>
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

#define NOMINMAX
#include <windows.h>
// using namespace std;
using json = nlohmann::json;

#define MAX_ENEMIES 128

class Enemy;
// class EnemyShootScript;

class StageManager
{
private:
    int stage;
    int time;
    int difficulty;
    int latestEnemyId;
    Enemy *enemys[MAX_ENEMIES];
    EnemyShootScript enemyShootScript;
    std::vector<enemyStatus> loadEnemies;
    int enemyCount;

public:
    StageManager(int _stage, int _time, int _difficulty);
    ~StageManager();

    void LoadFromVector(const std::vector<EnemyStatus> &src);

    void loadEnemy();
    void spwanEnemy(int index, EnemyStatus enemyStatus);
    void deleteEnemy(int);
    void updateStage(BombManager *bMgr, BombInfo bombs[MAX_BOMBS], Player *player);
    int getEmptyIndex(); // 空いてる敵のインデックス取得
};