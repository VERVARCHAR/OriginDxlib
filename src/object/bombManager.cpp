#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_HPP_

BombManager::BombManager(BombInfo bombs[MAX_BOMBS])
{
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        bombs[i].isUsing = false;
        bombs[i].isPlayers = false;
        bombs[i].pos.x = 0;
        bombs[i].pos.y = 0;
        bombs[i].vel.x = 0;
        bombs[i].vel.y = 0;
        bombs[i].radius = 0;
        bombs[i].power = 1;
        bombs[i].time = 0;
    }
}
BombManager::~BombManager()
{
    ;
}

int BombManager::getEmptyIndex(BombInfo bombs[MAX_BOMBS])
{
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (!bombs[i].isUsing)
        {
            return i;
        }
    }
    return -1;
}

bool BombManager::getOnScreen(BombInfo bomb)
{
    // TODO 画面サイズ取得に変更
    int screenWidth = 1000;
    int screenHeight = 600;
    if (bomb.pos.x + bomb.radius < 0 || bomb.pos.y - bomb.radius > screenWidth || bomb.pos.y + bomb.radius < 0 || bomb.pos.y - bomb.radius > screenHeight)
    {
        return false;
    }
    return true;
}

void BombManager::drawBombs(BombInfo bombs[MAX_BOMBS])
{
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing)
        {
            DrawCircle(bombs[i].pos.x, bombs[i].pos.y, bombs[i].radius, GetColor(255, 255, 0), TRUE);
        }
    }
}

void BombManager::DEBUG_printAllBombs(BombInfo bombs[MAX_BOMBS])
{
    int cnt = 0;
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing)
        {
            printfDx(L"Bomb %d: Active", i);
            cnt++;
        }
    }
    DrawFormatString(600, 600, GetColor(255, 255, 255), L"Bombs cnt %d", cnt);
}

void BombManager::updateBombs(BombInfo bombs[MAX_BOMBS])
{
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        bombs[i].pos.x += bombs[i].vel.x;
        bombs[i].pos.y += bombs[i].vel.y;
        bombs[i].time += 1;
        if (!getOnScreen(bombs[i]))
        {
            bombs[i].isUsing = false;
            bombs[i].isPlayers = false;
            bombs[i].pos.x = 0;
            bombs[i].pos.y = 0;
            bombs[i].vel.x = 0;
            bombs[i].vel.y = 0;
            bombs[i].radius = 0;
            bombs[i].power = 1;
            bombs[i].time = 0;
        }
    }
}

bool isHit(BombInfo *bomb, Vec2d pos, int radius)
{

    if (bomb->isUsing)
    {
        if ((pos.x - bomb->pos.x) * (pos.x - bomb->pos.x) + (pos.y - bomb->pos.y) * (pos.y - bomb->pos.y) < (bomb->radius + radius) * (bomb->radius + radius))
        {
            bomb->isUsing = false;
            return true;
        }
    }

    return false;
}