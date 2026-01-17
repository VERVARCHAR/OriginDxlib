#pragma once

#ifndef _UTILS_HPP_
#define _UTILS_HPP_
#include "system/utils.hpp"
#endif

#include <string>

#define MAX_EFFECTS 256

enum class EffectType
{
    HitSpark,
    EnemyExplode,
    EnemyShoot,
    PlayerExplode,
    PlayerShoot,
    Spell,
    EnemySpell,
    BulletVanish,
    TextPopup,
    Graze,
    BGMStage,
    BGMBoss
};

struct Effect
{
    bool isAlive = false;
    EffectType type;

    Vec2d pos;
    int time = 0;     // 生存フレーム
    int lifeTime = 0; // 何フレームで消えるか

    // アニメーション用
    int handle = -1;       // 1枚 or スプライトシート
    int divNum = 1;        // コマ数
    int frame = 0;         // 現在のコマ
    int frameInterval = 4; // 何フレームごとに次コマ

    // 描画サイズなど
    float scale = 1.0f;
    int alpha = 255;

    // テキスト系用
    std::wstring text;
};

class Effecter
{
public:
    Effecter();
    ~Effecter();

    void init();

    void loadEffecter(); // 画像・音読み込み
    void loadBGM();
    void effecterUpdate(); // 毎フレーム更新
    void effecterDraw();   // 毎フレーム描画

    // --------- API（ゲームから呼びたい関数）---------
    void playHitSpark(const Vec2d &pos);
    void playEnemyExplode(const Vec2d &pos);
    void playBossExplode(const Vec2d &pos);
    void playBulletVanish(const Vec2d &pos);
    void playPlayerExplode(const Vec2d &pos);
    void playSpell(const Vec2d &pos);
    void playEnemySpell(const Vec2d &pos);
    void playGraze(const Vec2d &pos);
    void playStageBGM(int stage);
    void playBossBGM(int stage);

    void playSE_Shot();
    void playSE_EnemyDead();
    void playSE_BossDead();
    void playSE_PlayerDead();
    void playSE_Bomb();
    void PlaySE_BombVanish();

    void PlayBGM_Title();
    void PlayBGM_Stage(int stage);
    void PlayBGM_Boss(int stage);

    void StopBGM_Title() { StopSoundMem(bgmTitle); }
    void StopBGM_Stage(int stage) { StopSoundMem(bgmStage[stage - 1]); };
    // TODO stage
    void StopBGM_Boss(int stage) { StopSoundMem(bgmBoss[0]); };

    void setEnemyCutInImage(int handle) { enemyCutInhandle = handle; };

    int bgmTitle = -1;

private:
    Effect effects[MAX_EFFECTS];

    // 画像ハンドル
    int hitSparkHandle = -1;
    int enemyExplodeHandle[3][10] = {0};
    int bulletVanishHandle[12] = {-1};
    int playerExplodeHandle[10] = {0};
    int particleHandle[16] = {0};
    int sakuraEffect[12] = {0};
    int enemyCutInhandle = -1;

    // SE ハンドル
    int seShot = -1;
    int seEnemyDead = -1;
    int seBossDead = -1;
    int sePlayerDead = -1;
    int seBomb = -1;
    int seVanish = -1;
    int seGraze = -1;
    int seSpell = -1;
    int seCountdown = -1;

    // BGM ハンドル
    int bgmStage[7] = {-1};
    int bgmBoss[7] = {-1};

    int getEmptyIndex();
    void spawnEffect(EffectType type, const Vec2d &pos);
};