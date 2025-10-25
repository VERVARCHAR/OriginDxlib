#ifndef _ENEMY_HPP__
#define _ENEMY_HPP_
#include "object/enemy.hpp"
#endif // _ENEMY_HPP__

Enemy::Enemy()
{
    x = 0;
    y = 0;
    hp = 1;
    type = 0;
    strcpy(name, "Default");
}

Enemy::~Enemy()
{
    ;
}

void Enemy::setPosition(int _x, int _y)
{
    x = _x;
    y = _y;
}
