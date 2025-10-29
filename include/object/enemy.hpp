#ifndef _DXLIB_HPP_
#define _DXLIB_HPP_
#include "DxLib.h"
#endif // _DXLIB_HPP_

#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_MANAGER_HPP_

// #ifndef _ENEMY_SHOOT_SCRIPT_01_HPP_
// #define _ENEMY_SHOOT_SCRIPT_01_HPP_
// #include "object/enemyShootScripts/01.hpp"
// #endif // _ENEMY_SHOOT_SCRIPT_01_HPP_

// typedef struct usingBombStruct
// {
//     bool usingFlag;
//     int timeCount;
// } UsingBombStruct;

class Enemy
{
private:
    Vec2d pos;
    int hp;        // 体力
    int type;      // 種類
    char name[50]; // 名前

protected:
public:
    // usingBombStruct usingBombs[MAX_USING] = {{0, 0}}; // 使用中の弾幕数

    Enemy();
    ~Enemy();

    void setPosition(int _x, int _y);
    Vec2d getPosition();
    // void getUsingBombs(UsingBombStruct (&_usingBombs)[MAX_USING][2]) { memcpy(_usingBombs, usingBombs, sizeof(usingBombs)); }
    // void setUsingBombs(UsingBombStruct (&_usingBombs)[MAX_USING][2]) { memcpy(usingBombs, _usingBombs, sizeof(usingBombs)); }
    void getBMgrData(BombManager &_BombManager);
    void enemyUpdate(int, int, BombManager, BombInfo[MAX_BOMBS]);
    void shootBomb();
};