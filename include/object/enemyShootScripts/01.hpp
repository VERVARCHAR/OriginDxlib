#ifndef _ENEMY_HPP_
#define _ENEMY_HPP_
#include "object/enemy.hpp"
#endif // __ENEMY_HPP_s

#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_HPP_

#define _USE_MATH_DEFINES // for C++
#include <cmath>

#ifndef _NORM_
#define _NORM_
// 角度正規化（0～2π）
inline double normalizeAngle(double a)
{
    const double TWO_PI = 2.0 * M_PI;
    a = std::fmod(a, TWO_PI);
    if (a < 0)
        a += TWO_PI;
    return a;
}
#endif

class Enemy;

class EnemyShootScript
{
public:
    void BombType01(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty);
    void BombType02(Enemy &enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty);
};