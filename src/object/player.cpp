#ifndef _PLAYER_HPP__
#define _PLAYER_HPP_
#include "object/player.hpp"
#endif // _PLAYER_HPP_

Player::Player()
{
    pos.x = 350;
    pos.y = 500;
    status.lives = 2;
    status.spels = 3;
    status.power = 1;
    status.isSpel = false;
    status.invincible = false;
    status.invincibleTime = 0;
    status.isShoot = false;
    status.isShift = false;
    radius = 3;
    type = 0;
    strcpy(name, "Default");
}

Player::~Player()
{
    ;
}

void Player::playerUpdate(BombManager bMgr, BombInfo bombs[MAX_BOMBS], StageManager sMgr)
{
    getKeyInput();
    DrawBox(pos.x - 10, pos.y - 10, pos.x + 10, pos.y + 10, GetColor(0, 0, 255), TRUE);
    DrawCircle(pos.x, pos.y, radius, GetColor(255, 0, 255), TRUE);

    if (status.isShoot)
    {
        shootBomb(bMgr, bombs);
    }

    if (!status.invincible)
    {
        for (int i = 0; i < MAX_BOMBS; i++)
        {
            if (bombs[i].isUsing && !bombs[i].isPlayers && isHitBomb(&bombs[i], pos, radius) && isHitEnemy(sMgr))
            {
                Dead();
            }
        }
    }
    else
    {
        status.invincibleTime -= 1;
        if (status.isSpel)
        {
            SpelCard(bMgr, bombs);
        }
        if (status.invincibleTime == 0)
        {
            status.invincible = false;
            if (status.isSpel)
            {
                status.isSpel = false;
            }
        }
    }
}

Vec2d Player::getPosition()
{
    return pos;
}

void Player::getKeyInput()
{
    double moveSpeed = 0;
    if (Key[KEY_INPUT_LSHIFT] > 1)
    {
        status.isShift = true;
        moveSpeed = 1.5;
    }
    else
    {
        status.isShift = false;
        moveSpeed = 3.0;
    }

    if (Key[KEY_INPUT_UP] > 1)
    {
        pos.y -= moveSpeed;
    }
    if (Key[KEY_INPUT_DOWN] > 1)
    {
        pos.y += moveSpeed;
    }
    if (Key[KEY_INPUT_LEFT] > 1)
    {
        pos.x -= moveSpeed;
    }
    if (Key[KEY_INPUT_RIGHT] > 1)
    {
        pos.x += moveSpeed;
    }

    if (Key[KEY_INPUT_X] == 1)
    {
        status.isSpel = true;
        status.invincible = true;
        status.invincibleTime = 5 * 60;
    }

    if (Key[KEY_INPUT_Z] > 0 && Key[KEY_INPUT_Z] % 5 == 0)
    {
        // 無敵時間でない もしくは、無敵時間だがスペル使用時なら打てる
        if (!status.invincible || (status.invincible && status.isSpel))
        {
            status.isShoot = true;
        }
    }
    else
    {
        status.isShoot = false;
    }
}

void Player::Dead()
{
    status.lives -= 1;
    status.invincible = true;
    status.invincibleTime = 3 * 60;
}

void Player::shootBomb(BombManager bMgr, BombInfo bombs[MAX_BOMBS])
{
    double speed = 15;

    for (int i = 0; i < status.power; i++)
    {
        int idx = bMgr.getEmptyIndex(bombs);
        bombs[idx].isUsing = true;
        bombs[idx].time = 0;
        bombs[idx].radius = 5;
        bombs[idx].pos = pos;
        bombs[idx].pos.x += (status.isShift ? -10 : -20) * (status.power - 1) + (status.isShift ? 20 : 40) * i;
        bombs[idx].pos.y += 20;
        bombs[idx].isPlayers = true;
        bombs[idx].type = 2;

        bombs[idx].vel.y = -speed;
    }
}

void Player::SpelCard(BombManager bMgr, BombInfo bombs[MAX_BOMBS])
{
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (!bombs[i].isPlayers && POWER2((pos.x) - (bombs[i].pos.x)) + POWER2((pos.y) - (bombs[i].pos.y)) < POWER2(250))
        {
            bMgr.initBomb(&bombs[i]);
        }
    }
}

bool Player::isHitEnemy(StageManager sMgr)
{
    Vec2d enmeyPos = {0, 0};
    int enemyRadius = 0;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (sMgr.enemys[i]->enemyStatus.isAlive)
        {
            enmeyPos = sMgr.enemys[i]->enemyStatus.pos;
            enemyRadius = sMgr.enemys[i]->enemyStatus.radius;

            if (POWER2(((enmeyPos.x) - (pos.x))) + POWER2(((enmeyPos.y) - (pos.y))) < POWER2(((enemyRadius) + (radius))))
            {
                return true;
            }
        }
    }
}