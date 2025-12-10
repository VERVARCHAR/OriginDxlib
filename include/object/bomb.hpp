#pragma once
#ifndef _DXLIB_HPP_
#define _DXLIB_HPP_
#include "DxLib.h"
#endif // _DXLIB_HPP_

#ifndef _UTILS_H_
#define _UTILS_H_
#include "system/utils.hpp"
#endif

#ifndef _MATH_HPP_
#define _MATH_HPP_
#include "math.h"
#endif // _MATH_HPP_

#ifndef _POWER2_
#define _POWER2_
#define POWER2(x) (x) * (x)
#endif

#define MAX_BOMBS 5000

typedef struct Coordinate
{
    double x;
    double y;
} Vec2d;

typedef struct BombInfo
{
    bool isUsing = false;
    bool isPlayers = false;
    bool isGraze = false;
    Vec2d pos;  // 座標
    Vec2d vel;  // 速度
    int radius; // 半径
    int power;  // 威力(基本1)
    int time;
    short id; // shooted_id,bombs_id
    short type;
    bool isVanishing = false;
    int vanishTime;
} bombInfo;

#ifndef _EFFECTER_HPP_
#define _EFFECTER_HPP_
#include "system/effecter.hpp"
#endif
class Effecter;

class BombManager
{
private:
    int bombsHandle[16];

public:
    BombManager(BombInfo bombs[MAX_BOMBS]);
    ~BombManager();

    void init(BombInfo bombs[MAX_BOMBS]);

    int getEmptyIndex(BombInfo bombs[MAX_BOMBS]); // 空いてる弾幕のインデックス取得

    void setBombsHandle(int bombsHandle[16]);

    // 画面外判定とリセット
    void drawBombs(BombInfo bombs[MAX_BOMBS]);

    void updateBombs(BombInfo bombs[MAX_BOMBS]);

    // デバッグ用関数
    void DEBUG_printAllBombs(BombInfo bombs[MAX_BOMBS]);

    bool getOnScreen(BombInfo);

    void initBomb(BombInfo *);

    void removeBomb(BombInfo bombs[MAX_BOMBS], Effecter *effecter);
};

bool isHitBomb(BombInfo *bomb, Vec2d pos, int radius);
bool isHitBomb(BombInfo *bomb, Vec2d pos, int radius, int *grazeCount, Effecter *effecter);