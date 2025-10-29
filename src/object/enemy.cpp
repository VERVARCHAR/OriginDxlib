#ifndef _ENEMY_HPP__
#define _ENEMY_HPP_
#include "object/enemy.hpp"
#endif // _ENEMY_HPP__

Enemy::Enemy()
{
    pos.x = 350;
    pos.y = 350;
    hp = 100;
    radius = 10;
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
    DrawCircle(pos.x, pos.y, radius, GetColor(255, 0, 255), TRUE);
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing && bombs[i].isPlayers && isHit(&bombs[i], pos, radius))
        {
            hp--;
        }
    }
}

Vec2d Enemy::getPosition()
{
    return pos;
}

void Enemy::getBMgrData(BombManager &_BombManager)
{
}