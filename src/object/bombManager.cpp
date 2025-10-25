#ifndef _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_HPP_

BombManager::BombManager()
{
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        usinglist[i] = false;
        bombs[i].setPosition(0, 0);
        bombs[i].setVelocity(0, 0);
        bombs[i].setRadius(0);
        bombs[i].setPower(0);
    }
}
BombManager::~BombManager()
{
    ;
}

int BombManager::getEmptyIndex()
{
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (!usinglist[i])
        {
            return i;
        }
    }
    return -1;
}

void BombManager::setBomb(int index, int x, int y, int dx, int dy, int radius, int power)
{
    usinglist[index] = true;
    bombs[index].setPosition(x, y);
    bombs[index].setVelocity(dx, dy);
    bombs[index].setRadius(radius);
    bombs[index].setPower(power);
}

void BombManager::resetBomb(int index)
{
    usinglist[index] = false;
    bombs[index].setPosition(0, 0);
    bombs[index].setVelocity(0, 0);
    bombs[index].setRadius(0);
    bombs[index].setPower(0);
}

void BombManager::drawBombs()
{
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (!bombs[i].isInScreen())
        {
            usinglist[i] = false;
        }
        if (usinglist[i])
        {
            bombs[i].updateBomb();
        }
    }
}

void BombManager::DEBUG_printAllBombs()
{
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (usinglist[i])
        {
            DrawFormatString(10, 10 + i * 20, GetColor(255, 255, 255), L"Bomb %d: Active", i);
        }
    }
}