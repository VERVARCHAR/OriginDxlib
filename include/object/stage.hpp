#pragma once

#include "object/enemyShootScripts/EnemyShootScripts.hpp"

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
#include "object/enemyShootScripts/EnemyShootScripts.hpp"
#endif // _ENEMY_SHOOT_SCRIPT_01_HPP_

#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_
#include "object/player.hpp"
#endif // _PLAYER_HPP_

#include <vector>
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <string>

#define NOMINMAX
#include <windows.h>
using namespace std;
using json = nlohmann::json;

#define MAX_ENEMIES 128

// 前方宣言で循環依存を回避
class BombManager;
struct BombInfo;
class Player;

class Enemy;            // ポインタ保持のため前方宣言
class EnemyShootScript; // 前方宣言（実体は .cpp で include）
struct EnemyStatus;     // 実体は enemy.hpp 内

enum Scene
{
    PRELOADING,
    TITLE,
    LOADING,
    INGAME,
    RESULT,
};

typedef struct TalkData
{
    string talkString;
    bool isTalkEnemy;
};

typedef struct StageInfo
{
    int stage;
    int difficulty;
    int score;
    PlayerStatus nowStatus;
};

class StageManager
{
private:
    // TODO StageInfoへの移行
    int stage;
    int difficulty;

    StageInfo stageInfo;

    int time;
    int latestEnemyId;
    // Enemy *enemys[MAX_ENEMIES];
    EnemyShootScript enemyShootScript;
    std::vector<EnemyStatus> loadEnemies;
    int enemyCount;

public:
    bool isPause = false;
    bool isTalk;
    int talkCount;
    // TODO
    string talkString[20];
    int talkWho[20];
    TalkData talkData[20];

    bool isClearStage = false;
    bool isGameOver = false;

    int enemyImageHandle[32];
    int bossIndex = 0;

    StageManager(int _stage, int _time, int _difficulty);
    ~StageManager();

    Enemy *enemys[MAX_ENEMIES];

    void init(int _stage, int _time, int _difficulty);

    void LoadFromVector(const std::vector<EnemyStatus> &src);

    void loadEnemy();
    void spwanEnemy(int index, EnemyStatus enemyStatus);
    void deleteEnemy(int);
    void updateStage(BombManager *bMgr, BombInfo bombs[MAX_BOMBS], Player *player);
    // Enemy getEnemy(int index) { return enemys[i]; };
    int getEmptyIndex(); // 空いてる敵のインデックス取得

    void getClearStage();
    void getGameOver(Player *player);

    StageInfo getStageInfo()
    {
        return stageInfo;
    }

    void talk(int type);
    string getTalkString(int talkCount) { return talkString[talkCount]; };
    int getTalkWho(int talkCount) { return talkWho[talkCount]; };
    void endTalk()
    {
        isTalk = false;
        time += 1;
    };
};