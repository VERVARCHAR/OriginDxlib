#ifndef _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_HPP_

Bomb::Bomb()
{
}

Bomb::~Bomb()
{
}

void Bomb::updateBomb()
{
    updatePosition(dx, dy);
    drawBomb();
}

void Bomb::drawBomb()
{
    DrawCircle(x, y, radius, GetColor(255, 0, 0), TRUE);
}

void Bomb::updatePosition(int _dx, int _dy)
{
    dx = _dx;
    dy = _dy;
    x += dx;
    y += dy;
}

bool Bomb::isInScreen()
{

    // TODO 画面サイズ取得に変更
    int screenWidth = 1000;
    int screenHeight = 600;
    if (x + radius < 0 || x - radius > screenWidth || y + radius < 0 || y - radius > screenHeight)
    {
        return false;
    }
    return true;
}
