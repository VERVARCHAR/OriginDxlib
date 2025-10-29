#ifndef _DXLIB_HPP_
#define _DXLIB_HPP_
#include "DxLib.h"
#endif // _DXLIB_HPP_

#ifndef _MATH_HPP_
#define _MATH_HPP_
#include "math.h"
#endif // _MATH_HPP_

#define MAX_BOMBS 1000

typedef struct Coordinate
{
    double x;
    double y;
} Vec2d;

typedef struct BombInfo
{
    bool isUsing = false;
    Vec2d pos;  // 座標
    Vec2d vel;  // 速度
    int radius; // 半径
    int power;  // 威力(基本1)
    int time;
} bombInfo;

// 弾幕のクラス
// class Bomb
// {
// private:
//     BombInfo bInfo;

//     void updatePosition(int, int); // 位置更新
//     void drawBomb();               // 描画

// public:
//     Bomb();
//     ~Bomb();

//     void setRadius(int _radius) { bInfo.radius = _radius; } // 半径設定
//     void setPower(int _power) { bInfo.power = _power; }     // 威力設定

//     // 座標設定
//     void setPosition(int _x, int _y)
//     {
//         bInfo.x = _x;
//         bInfo.y = _y;
//     }

//     // 速度設定
//     void setVelocity(int _dx, int _dy)
//     {
//         bInfo.dx = _dx;
//         bInfo.dy = _dy;
//     }

//     bool isInScreen(); // 画面内か判定
//     void updateBomb(); // 弾幕の更新
// };

class BombManager
{
public:
    BombManager(BombInfo bombs[MAX_BOMBS]);
    ~BombManager();

    // int maxBombs;              // 最大弾幕数
    // bool usinglist[MAX_BOMBS]; // 使用中リスト
    // Bomb bombs[MAX_BOMBS];     // 弾幕配列

    int getEmptyIndex(BombInfo bombs[MAX_BOMBS]); // 空いてる弾幕のインデックス取得

    // // 弾幕設定
    // void setBomb(int index, BombInfo bInfo[MAX_BOMBS]);

    // // 弾幕リセット
    // void resetBomb(int index);

    // 画面外判定とリセット
    void drawBombs(BombInfo bombs[MAX_BOMBS]);

    void updateBombs(BombInfo bombs[MAX_BOMBS]);

    // デバッグ用関数
    void DEBUG_printAllBombs(BombInfo bombs[MAX_BOMBS]);

    bool getOnScreen(BombInfo);
};