#ifndef _DXLIB_HPP_
#define _DXLIB_HPP_
#include "DxLib.h"
#endif // _DXLIB_HPP_

#define MAX_USING 100

class Enemy
{
private:
    int x, y;                                // 座標
    int hp;                                  // 体力
    int type;                                // 種類
    char name[50];                           // 名前
    int usingBombs[MAX_USING][2] = {{0, 0}}; // 使用中の弾幕数

public:
    Enemy();
    ~Enemy();

    void setPosition(int _x, int _y);
    void shootBomb();
};