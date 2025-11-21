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

int EnemyShootScript::InitBombShoot(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty, Player player, double radius)
{
    int idx = bMgr.getEmptyIndex(bombs);
    if (idx < 0)
    {
        // 弾がこれ以上出せないときの対処（何もしない・ログを出すなど）
        printfDx(L"[WARN] player bullet overflow\n");
    }
    bombs[idx].isUsing = true;
    bombs[idx].time = 0;
    bombs[idx].radius = radius;
    bombs[idx].pos = enemy.getPosition();
    bombs[idx].isPlayers = false;
    bombs[idx].id = enemy.getId();

    return idx;
}

void EnemyShootScript::BombType00(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty, Player player)
{

    double speed = 1.0 + dificulty * 0.5; // doubleに

    if (time % 120 == 0)
    {
        for (int i = 0; i < dificulty * 12; i++)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, dificulty, player, 15);
            bombs[idx].type = 0;

            double phi = (2.0 * PI * i) / (dificulty * 12.0); // 浮動小数割り算
            bombs[idx].vel.x = speed * std::cos(phi);
            bombs[idx].vel.y = speed * std::sin(phi);
        }
    }

    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing)
        {
            if (bombs[i].id == enemy.getId())
            {
                double phi = std::atan2(bombs[i].vel.y, bombs[i].vel.x);
                phi = normalizeAngle(phi) + 0.0001 * dificulty;
                double norm = sqrt(pow(bombs[i].vel.x, 2) + pow(bombs[i].vel.y, 2));
                bombs[i].vel.x = norm * std::cos(phi);
                bombs[i].vel.y = norm * std::sin(phi);
            }
        }
    }
}

void EnemyShootScript::BombType01(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty, Player player)
{
    double speed = 2 + dificulty * 0.5; // doubleに

    if (time % 10 == 0)
    {
        if (60 < time % (int)((90) * (1 + dificulty * 0.25)))
        {
            Vec2d playerPos = player.getPosition();
            Vec2d enemyPos = enemy.getPosition();

            int idx = InitBombShoot(enemy, bMgr, bombs, time, dificulty, player, 20);
            bombs[idx].type = 1;

            double phi = std::atan2((playerPos.y - enemyPos.y), (playerPos.x - enemyPos.x));
            phi = normalizeAngle(phi);
            bombs[idx].vel.x = speed * std::cos(phi);
            bombs[idx].vel.y = speed * std::sin(phi);
        }
    }

    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing)
        {
            if (bombs[i].id == enemy.getId())
            {
                double phi = std::atan2(bombs[i].vel.y, bombs[i].vel.x);
                phi = normalizeAngle(phi);
                double norm = sqrt(pow(bombs[i].vel.x, 2) + pow(bombs[i].vel.y, 2));
                bombs[i].vel.x = norm * std::cos(phi);
                bombs[i].vel.y = norm * std::sin(phi);
            }
        }
    }
}
