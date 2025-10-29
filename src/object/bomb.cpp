// #ifndef _BOMB_HPP_
// #include "object/bomb.hpp"
// #endif // _BOMB_HPP_

// Bomb::Bomb()
// {
// }

// Bomb::~Bomb()
// {
// }

// void Bomb::updateBomb()
// {
//     updatePosition(bInfo.dx, bInfo.dy);
//     drawBomb();
// }

// void Bomb::drawBomb()
// {
//     DrawCircle(bInfo.x, bInfo.y, bInfo.radius, GetColor(255, 0, 0), TRUE);
// }

// void Bomb::updatePosition(int _dx, int _dy)
// {
//     bInfo.dx = _dx;
//     bInfo.dy = _dy;
//     bInfo.x += bInfo.dx;
//     bInfo.y += bInfo.dy;
// }

// bool Bomb::isInScreen()
// {

//     // TODO 画面サイズ取得に変更
//     int screenWidth = 1000;
//     int screenHeight = 600;
//     if (bInfo.x + bInfo.radius < 0 || bInfo.x - bInfo.radius > screenWidth || bInfo.y + bInfo.radius < 0 || bInfo.y - bInfo.radius > screenHeight)
//     {
//         return false;
//     }
//     return true;
// }
