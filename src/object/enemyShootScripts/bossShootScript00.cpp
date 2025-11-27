#pragma once

#ifndef _ENEMY_SHOOT_SCRIPT_01_HPP_
#define _ENEMY_SHOOT_SCRIPT_01_HPP_
#include "object/enemyShootScripts/EnemyShootScripts.hpp"
#endif // _ENEMY_SHOOT_SCRIPT_01_HPP_

#ifndef _ENEMY_HPP_
#define _ENEMY_HPP_
#include "object/enemy.hpp"
#endif // _ENEMY_HPP_s

#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_HPP_

#define _USE_MATH_DEFINES // for C++
#include <cmath>

class Enemy;

void EnemyShootScript::Boss01Spell01(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{

    double speed = 1.0 + difficulty * 0.5; // doubleに

    if (time % 10 == 0)
    {
        if (60 < time % (int)((90) * (1 + difficulty * 0.25)))
        {
            for (int i = 0; i < difficulty * 12; i++)
            {
                int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 15);
                bombs[idx].type = 0;

                double phi = (2.0 * PI * i) / (difficulty * 12.0); // 浮動小数割り算
                bombs[idx].vel.x = speed * std::cos(phi + (time % 3) * 0.1);
                bombs[idx].vel.y = speed * std::sin(phi + (time % 3) * 0.1);
            }
        }
    }

    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing)
        {
            if (bombs[i].id == enemy.getId())
            {
                double phi = std::atan2(bombs[i].vel.y, bombs[i].vel.x);
                phi = normalizeAngle(phi) + 0.0001 * difficulty;
                double norm = sqrt(pow(bombs[i].vel.x, 2) + pow(bombs[i].vel.y, 2));
                bombs[i].vel.x = norm * std::cos(phi);
                bombs[i].vel.y = norm * std::sin(phi);
            }
        }
    }
}

void EnemyShootScript::Boss01Spell02(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{

    double speed = 1.0 + difficulty * 0.5; // doubleに

    if (time % 10 == 0)
    {
        if (60 < time % (int)((90) * (1 + difficulty * 0.25)))
        {
            for (int i = 0; i < difficulty * 16; i++)
            {
                int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 15);
                bombs[idx].type = 0;

                double phi = (2.0 * PI * i) / (difficulty * 12.0); // 浮動小数割り算
                bombs[idx].vel.x = speed * std::cos(phi + time * 0.1);
                bombs[idx].vel.y = speed * std::sin(phi + time * 0.1);
            }
        }
    }

    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing)
        {
            if (bombs[i].id == enemy.getId())
            {
                double phi = std::atan2(bombs[i].vel.y, bombs[i].vel.x);
                phi = normalizeAngle(phi) + 0.0001 * difficulty;
                double norm = sqrt(pow(bombs[i].vel.x, 2) + pow(bombs[i].vel.y, 2));
                bombs[i].vel.x = norm * std::cos(phi);
                bombs[i].vel.y = norm * std::sin(phi);
            }
        }
    }
}

void EnemyShootScript::Boss01Spell03(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{

    double speed = 1.0 + difficulty * 0.5; // doubleに

    if (time % 10 == 0)
    {
        if (60 < time % (int)((90) * (1 + difficulty * 0.25)))
        {
            for (int i = 0; i < difficulty * 20; i++)
            {
                int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 15);
                bombs[idx].type = 0;

                double phi = (2.0 * PI * i) / (difficulty * 12.0); // 浮動小数割り算
                bombs[idx].vel.x = speed * std::cos(phi + time * 0.1);
                bombs[idx].vel.y = speed * std::sin(phi + time * 0.1);
            }
        }
    }

    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing)
        {
            if (bombs[i].id == enemy.getId())
            {
                double phi = std::atan2(bombs[i].vel.y, bombs[i].vel.x);
                phi = normalizeAngle(phi) + 0.0001 * difficulty;
                double norm = sqrt(pow(bombs[i].vel.x, 2) + pow(bombs[i].vel.y, 2));
                bombs[i].vel.x = norm * std::cos(phi);
                bombs[i].vel.y = norm * std::sin(phi);
            }
        }
    }
}