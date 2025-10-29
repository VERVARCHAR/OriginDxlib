#ifndef _DXLIB_HPP_
#define _DXLIB_HPP_
#include "DxLib.h"
#endif // _DXLIB_HPP_

#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_MANAGER_HPP_

class Player
{
private:
    Vec2d pos;
    int type;      // 種類
    int hp;        // 体力
    char name[50]; // 名前
    int radius;

protected:
public:
    // usingBombStruct usingBombs[MAX_USING] = {{0, 0}}; // 使用中の弾幕数

    Player();
    ~Player();

    void setPosition(int _x, int _y);
    Vec2d getPosition();
    int getHP()
    {
        return hp;
    }
    // void getUsingBombs(UsingBombStruct (&_usingBombs)[MAX_USING][2]) { memcpy(_usingBombs, usingBombs, sizeof(usingBombs)); }
    // void setUsingBombs(UsingBombStruct (&_usingBombs)[MAX_USING][2]) { memcpy(usingBombs, _usingBombs, sizeof(usingBombs)); }
    void getBMgrData(BombManager &_BombManager);
    void playerUpdate(BombManager, BombInfo[MAX_BOMBS]);
    void shootBomb();
};