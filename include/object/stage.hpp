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

#ifndef _EFFECTER_HPP_
#define _EFFECTER_HPP_
#include "system/effecter.hpp"
#endif

#ifndef _ITEMS_HPP_
#define _ITEMS_HPP_
#include "object/item.hpp"
#endif

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

// シーン制御列挙体
enum Scene
{
    PRELOADING,
    TITLE,
    LOADING,
    INGAME,
    RESULT,
    DEBUG,
};

enum Difficulty
{
    EASY = 1,
    NORMAL = 2,
    HARD = 3,
    LUNATIC = 4,
    EXTRA = 5,
};

// TODO 道中会話用に複数必要??フレーム数とかもいるかも???
// 会話データ構造体
typedef struct TalkData
{
    string talkString;
    bool isTalkEnemy;
};

// ステージ情報構造体(UIに渡すようが主)
typedef struct StageInfo
{
    int stage;
    Difficulty difficulty;
    int score;
    PlayerStatus nowStatus;
};

class StageManager
{
private:
    // TODO StageInfoへの移行
    int stage;      // ステージ
    int difficulty; // 難易度

    StageInfo stageInfo;

    int time;          // ステージタイマー
    int latestEnemyId; // 最新の敵ID(つかってない??)

    // Enemy *enemies[MAX_ENEMIES];
    EnemyShootScript enemyShootScript;    // 敵の弾幕コードクラス
    std::vector<EnemyStatus> loadEnemies; // 敵情報読み込みベクトル
    int enemyCount;                       // 敵カウンター

public:
    bool isPause = false;      // ポーズフラグ
    bool isTalk = false;       // 会話フラグ
    int talkCount = 0;         // トークカウンタ
    bool isClearStage = false; // ステージクリアフラグ
    bool isGameOver = false;   // ゲームオーバーフラグ

    // TODO TalkData構造体への移行
    string talkString[20];
    int talkWho[20];
    TalkData talkData[20];

    Enemy *enemies[MAX_ENEMIES]; // 実際に扱う敵情報格納配列

    int enemyImageHandle[32]; // 敵画像ハンドラ
    int bossIndex = 0;        // ボスのID格納変数

    StageManager(int _stage, int _time, Difficulty _difficulty);
    ~StageManager();

    void init(int _stage, int _time, Difficulty _difficulty);

    void loadEnemy();                                                                                                // 敵をJSONファイルから読み込む一連の処理呼び出し関数
    void LoadFromVector(const std::vector<EnemyStatus> &srcEnemyStatus, const std::vector<SpellInfo> &srcSpellInfo); // JSONファイルからVectorにしたものを配列に落とし込む

    void spawnEnemy(int index, EnemyStatus enemyStatus); // 敵情報をEnemyに代入
    void deleteEnemy(int);                               // ??

    // ステージ情報更新関数
    void updateStage(BombManager *bMgr, ItemManager *iMgr, BombInfo bombs[MAX_BOMBS], Player *player, Effecter *effecter);
    // Enemy getEnemy(int index) { return enemies[i]; };

    int getEmptyIndex(); // 空いてる敵のインデックス取得(おそらく使ってない)

    void getClearStage();             // ステージクリア判定処理
    void getGameOver(Player *player); // ゲームオーバー判定処理

    // ステージ情報のgetter(UI用)
    StageInfo getStageInfo()
    {
        return stageInfo;
    }

    void setScore(int scoreValue)
    {
        stageInfo.score += scoreValue;
    }

    // TODO もっとちゃんとしたやつ作る
    void talk(int type); // 会話処理

    // TODO 中身ちゃんとしたやつ作って
    string getTalkString(int talkCount) { return talkString[talkCount]; }; // 外部ファイルから会話内容を取得する

    // だれが話しているのかを取得する
    int getTalkWho(int talkCount) { return talkWho[talkCount]; };

    // 会話判定終了処理
    void endTalk()
    {
        isTalk = false;
        time += 1;
    };
};