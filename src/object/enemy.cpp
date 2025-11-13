#ifndef _ENEMY_HPP_
#define _ENEMY_HPP_
#include "object/enemy.hpp"
#endif // _ENEMY_HPP_

Enemy::Enemy(EnemyStatus _enemyStatus)
{
    enemyStatus = _enemyStatus;
}

Enemy::~Enemy()
{
    ;
}

void Enemy::enemyUpdate(int time, int difficulty, BombManager *bMgr, BombInfo bombs[MAX_BOMBS], EnemyShootScript enemyShootScript, Player *player)
{
    if (!enemyStatus.isAlive)
    {
        return;
    }
    else
    {
        // 敵の状態更新ロジックをここに実装
        DrawBox(enemyStatus.pos.x - 10, enemyStatus.pos.y - 10, enemyStatus.pos.x + 10, enemyStatus.pos.y + 10, GetColor(255, 0, 0), TRUE);
        DrawCircle(enemyStatus.pos.x, enemyStatus.pos.y, enemyStatus.radius, GetColor(255, 0, 255), TRUE);

        shootBomb(enemyShootScript, bMgr, bombs, enemyStatus.time, difficulty, *player);
        if ((!player->getStatus().invincible) && isHitPlayer(player))
        {
            player->Dead();
        }

        enemyStatus.pos.x += enemyStatus.vel.x;
        enemyStatus.pos.y += enemyStatus.vel.y;
        enemyStatus.time++;

        for (int i = 0; i < MAX_BOMBS; i++)
        {
            if (bombs[i].isUsing && bombs[i].isPlayers && isHitBomb(&bombs[i], enemyStatus.pos, enemyStatus.radius))
            {
                enemyStatus.hp--;
            }
        }

        if (enemyStatus.hp == 0)
        {
            enemyStatus.lives -= 1;
        }

        if (enemyStatus.lives == 0 || !getOnScreen())
        {
            enemyStatus.isAlive = false;
        }
    }
}

Vec2d Enemy::getPosition()
{
    return enemyStatus.pos;
}

void Enemy::getBMgrData(BombManager &_BombManager)
{
}

bool Enemy::getOnScreen()
{
    // TODO 画面サイズ取得に変更
    int screenWidth = 1000;
    int screenHeight = 600;
    // int hideScreen
    if (enemyStatus.pos.x + enemyStatus.radius < -50 || enemyStatus.pos.y - enemyStatus.radius > screenWidth + 50 || enemyStatus.pos.y + enemyStatus.radius < -50 || enemyStatus.pos.y - enemyStatus.radius > screenHeight + 50)
    {
        return false;
    }
    return true;
}

void Enemy::shootBomb(EnemyShootScript enemyShootScript, BombManager *bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty, Player player)
{

    switch (enemyStatus.shootType)
    {
    case 0:
        enemyShootScript.BombType00(*this, *bMgr, bombs, time, dificulty, player);
        break;

    case 1:
        enemyShootScript.BombType01(*this, *bMgr, bombs, time, dificulty, player);
        break;
    default:

        printfDx(L"[DEBUG]:shootBomb %d\n", enemyStatus.shootType);
        break;
    }
}

bool Enemy::isHitPlayer(Player *player)
{
    Vec2d playerPos = player->getPosition();
    int playerRadius = player->getRadius();

    if (enemyStatus.isAlive)
    {
        if (POWER2(((playerPos.x) - (enemyStatus.pos.x))) + POWER2(((playerPos.y) - (enemyStatus.pos.y))) < POWER2(((playerRadius) + (enemyStatus.radius))))
        {
            return true;
        }
    }
    return false;
}