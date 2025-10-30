#ifndef _ENEMY_SHOOT_SCRIPT_01_HPP_
#define _ENEMY_SHOOT_SCRIPT_01_HPP_
#include "object/enemyShootScripts/01.hpp"
#endif // _ENEMY_SHOOT_SCRIPT_01_HPP_
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
void EnemyShootScript::BombType01(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty)
{

    double speed = 1.0 + dificulty * 0.5; // doubleに

    if (time % 20 == 0)
    {
        for (int i = 0; i < dificulty * 12; i++)
        {
            int idx = bMgr.getEmptyIndex(bombs);
            bombs[idx].isUsing = true;
            bombs[idx].time = 0;
            bombs[idx].radius = 5;
            bombs[idx].pos = enemy.getPosition();
            bombs[idx].isPlayers = false;
            bombs[idx].id = enemy.id;

            double phi = (2.0 * M_PI * i) / (dificulty * 12.0); // 浮動小数割り算
            bombs[idx].vel.x = speed * std::cos(phi);
            bombs[idx].vel.y = speed * std::sin(phi);
        }
    }

    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing)
        {
            if (bombs[i].id == enemy.id)
            {
                double phi = std::atan2(bombs[i].vel.y, bombs[i].vel.x);
                phi = normalizeAngle(phi) + 0.001 * dificulty;
                bombs[i].vel.x = speed * std::cos(phi);
                bombs[i].vel.y = speed * std::sin(phi);
            }
        }
    }
}

void EnemyShootScript::BombType02(Enemy &enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty)
{
}
