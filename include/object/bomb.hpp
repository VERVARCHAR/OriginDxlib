#ifndef _DXLIB_HPP_
#define _DXLIB_HPP_
#include "DxLib.h"
#endif // _DXLIB_HPP_

#define MAX_BOMBS 100

// 弾幕のクラス
class Bomb
{
private:
    int x, y;   // 座標
    int dx, dy; // 速度
    int radius; // 半径
    int power;  // 威力(基本1)

    void updatePosition(int, int); // 位置更新
    void drawBomb();               // 描画

public:
    Bomb();
    ~Bomb();

    void setRadius(int _radius) { radius = _radius; } // 半径設定
    void setPower(int _power) { power = _power; }     // 威力設定

    // 座標設定
    void setPosition(int _x, int _y)
    {
        x = _x;
        y = _y;
    }

    // 速度設定
    void setVelocity(int _dx, int _dy)
    {
        dx = _dx;
        dy = _dy;
    }

    bool isInScreen(); // 画面内か判定
    void updateBomb(); // 弾幕の更新
};

class BombManager
{
public:
    BombManager();
    ~BombManager();

    int maxBombs;              // 最大弾幕数
    bool usinglist[MAX_BOMBS]; // 使用中リスト
    Bomb bombs[MAX_BOMBS];     // 弾幕配列

    int getEmptyIndex(); // 空いてる弾幕のインデックス取得

    // 弾幕設定
    void setBomb(int index, int x, int y, int dx, int dy, int radius, int power);

    // 弾幕リセット
    void resetBomb(int index);

    // 画面外判定とリセット
    void drawBombs();

    // デバッグ用関数
    void DEBUG_printAllBombs();
};