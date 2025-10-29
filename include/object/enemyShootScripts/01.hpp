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

class EnemyShootScript
{
public:
    void BombType01(Enemy &enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty)
    {
        double speed = 1.0 + dificulty; // doubleに

        if (time % 20 == 0)
        {
            for (int i = 0; i < dificulty * 32; i++)
            {
                int idx = bMgr.getEmptyIndex(bombs);
                bombs[idx].isUsing = true;
                bombs[idx].time = 0;
                bombs[idx].radius = 5;
                bombs[idx].pos = enemy.getPosition();
                bombs[idx].isPlayers = false;

                double phi = (2.0 * M_PI * i) / (dificulty * 32.0); // 浮動小数割り算
                bombs[idx].vel.y = speed * std::cos(phi);           // intにしない
                bombs[idx].vel.x = speed * std::sin(phi);
            }
        }
    }

    void BombType02(Enemy &enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty)
    {
    }
};