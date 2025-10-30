#ifndef _ENEMY_HPP__
#define _ENEMY_HPP_
#include "object/enemy.hpp"
#endif // _ENEMY_HPP__

Enemy::Enemy(Vec2d _pos, int _lives, int _hp, int _radius, int _type, int _shootType, short _id, char *_name)
{
    pos = _pos;
    hp = _hp;
    lives = _lives;
    radius = _radius;
    type = _type;
    shootType = _shootType;
    id = _id;
    strcpy(name, _name);
}

Enemy::~Enemy()
{
    ;
}

void Enemy::enemyUpdate(int time, int difficulty, BombManager bMgr, BombInfo bombs[MAX_BOMBS], EnemyShootScript enemyShootScript)
{
    if (!lives)
        return;
    // 敵の状態更新ロジックをここに実装
    DrawBox(pos.x - 10, pos.y - 10, pos.x + 10, pos.y + 10, GetColor(255, 0, 0), TRUE);
    DrawCircle(pos.x, pos.y, radius, GetColor(255, 0, 255), TRUE);

    shootBomb(enemyShootScript, bMgr, bombs, time, difficulty);

    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing && bombs[i].isPlayers && isHit(&bombs[i], pos, radius))
        {
            hp--;
        }
    }

    if (hp == 0)
    {
        lives -= 1;
    }
}

Vec2d Enemy::getPosition()
{
    return pos;
}

void Enemy::getBMgrData(BombManager &_BombManager)
{
}

void Enemy::shootBomb(EnemyShootScript enemyShootScript, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty)
{
    switch (shootType)
    {
    case 1:
        enemyShootScript.BombType01(*this, bMgr, bombs, time, dificulty);
        break;

    default:
        break;
    }
}