#ifndef _ENEMY_HPP__
#define _ENEMY_HPP_
#include "object/enemy.hpp"
#endif // _ENEMY_HPP__

Enemy::Enemy(EnemyStatus _enemyStatus)
{
    enemyStatus = _enemyStatus;
}

Enemy::~Enemy()
{
    ;
}

void Enemy::enemyUpdate(int time, int difficulty, BombManager bMgr, BombInfo bombs[MAX_BOMBS], EnemyShootScript enemyShootScript)
{
    if (!enemyStatus.isAlive)
        return;
    // 敵の状態更新ロジックをここに実装
    DrawBox(enemyStatus.pos.x - 10, enemyStatus.pos.y - 10, enemyStatus.pos.x + 10, enemyStatus.pos.y + 10, GetColor(255, 0, 0), TRUE);
    DrawCircle(enemyStatus.pos.x, enemyStatus.pos.y, enemyStatus.radius, GetColor(255, 0, 255), TRUE);

    shootBomb(enemyShootScript, bMgr, bombs, time, difficulty);

    enemyStatus.pos.x += enemyStatus.vel.x;
    enemyStatus.pos.y += enemyStatus.vel.y;

    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing && bombs[i].isPlayers && isHit(&bombs[i], enemyStatus.pos, enemyStatus.radius))
        {
            enemyStatus.hp--;
        }
    }

    if (enemyStatus.hp == 0)
    {
        enemyStatus.lives -= 1;
    }

    if (enemyStatus.lives == 0)
    {
        enemyStatus.isAlive = false;
    }
}

Vec2d Enemy::getPosition()
{
    return enemyStatus.pos;
}

void Enemy::getBMgrData(BombManager &_BombManager)
{
}

void Enemy::shootBomb(EnemyShootScript enemyShootScript, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty)
{
    switch (enemyStatus.shootType)
    {
    case 1:
        enemyShootScript.BombType01(*this, bMgr, bombs, time, dificulty);
        break;

    default:
        break;
    }
}