#ifndef _ENEMY_HPP__
#define _ENEMY_HPP_
#include "object/enemy.hpp"
#endif // _ENEMY_HPP__

Enemy::Enemy()
{
    pos.x = 350;
    pos.y = 350;
    hp = 1;
    type = 0;
    strcpy(name, "Default");
}

Enemy::~Enemy()
{
    ;
}

void Enemy::enemyUpdate(int time, int difficulty, BombManager bMgr, BombInfo bombs[MAX_BOMBS])
{
    // 敵の状態更新ロジックをここに実装
    DrawBox(pos.x - 10, pos.y - 10, pos.x + 10, pos.y + 10, GetColor(255, 0, 0), TRUE);
}

// void Enemy::setPosition(int _x, int _y)
// {
//     x = _x;
//     y = _y;
// }

Vec2d Enemy::getPosition()
{
    return pos;
}

void Enemy::getBMgrData(BombManager &_BombManager)
{
}