#ifndef _DXLIB_HPP_
#define _DXLIB_HPP_
#include "DxLib.h"
#endif // _DXLIB_HPP_

#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_MANAGER_HPP_

#ifndef _ENEMY_SHOOT_SCRIPT_01_HPP_
#define _ENEMY_SHOOT_SCRIPT_01_HPP_
#include "object/enemyShootScripts/01.hpp"
#endif // _ENEMY_SHOOT_SCRIPT_01_HPP_

class EnemyShootScript;

class Enemy
{
private:
    Vec2d pos;
    int type; // 種類
    int lives;
    int hp;        // 体力
    char name[50]; // 名前
    int radius;
    int shootType;

protected:
public:
    short id;

    // usingBombStruct usingBombs[MAX_USING] = {{0, 0}}; // 使用中の弾幕数

    Enemy(Vec2d _pos, int lives, int _hp, int _radius, int _type, int _shootType, short _id, char *_name);
    ~Enemy();

    void setPosition(int _x, int _y);
    Vec2d getPosition();
    int getHP()
    {
        return hp;
    }
    // void getUsingBombs(UsingBombStruct (&_usingBombs)[MAX_USING][2]) { memcpy(_usingBombs, usingBombs, sizeof(usingBombs)); }
    // void setUsingBombs(UsingBombStruct (&_usingBombs)[MAX_USING][2]) { memcpy(usingBombs, _usingBombs, sizeof(usingBombs)); }
    void getBMgrData(BombManager &_BombManager);
    void enemyUpdate(int, int, BombManager, BombInfo[MAX_BOMBS], EnemyShootScript);
    void shootBomb(EnemyShootScript, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty);
};