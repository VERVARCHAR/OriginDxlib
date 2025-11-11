#pragma once
#ifndef _DXLIB_HPP_
#define _DXLIB_HPP_
#include "DxLib.h"
#endif // _DXLIB_HPP_

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
    Vec2d pos;  // 座標
    Vec2d vel;  // 速度
    int radius; // 半径
    int power;  // 威力(基本1)
    int time;
    short id; // shooted_id,bombs_id
    short type;
} bombInfo;

class BombManager
{
private:
    int bombsHandle[16];

public:
    BombManager(BombInfo bombs[MAX_BOMBS]);
    ~BombManager();

    int getEmptyIndex(BombInfo bombs[MAX_BOMBS]); // 空いてる弾幕のインデックス取得

    void setBombsHandle(int bombsHandle[16]);

    // 画面外判定とリセット
    void drawBombs(BombInfo bombs[MAX_BOMBS]);

    void updateBombs(BombInfo bombs[MAX_BOMBS]);

    // デバッグ用関数
    void DEBUG_printAllBombs(BombInfo bombs[MAX_BOMBS]);

    bool getOnScreen(BombInfo);

    void initBomb(BombInfo *);
};

bool isHitBomb(BombInfo *bomb, Vec2d pos, int radius);