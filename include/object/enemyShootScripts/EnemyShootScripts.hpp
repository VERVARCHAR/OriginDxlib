#pragma once
#ifndef _ENEMY_HPP_
#define _ENEMY_HPP_
#include "object/enemy.hpp"
#endif // __ENEMY_HPP_s

#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_HPP_

#ifndef _ENEMY_SHOOT_SCRIPT_01_HPP_
#define _ENEMY_SHOOT_SCRIPT_01_HPP_
#include "object/enemyShootScripts/enemyShootScripts.hpp"
#endif // _ENEMY_SHOOT_SCRIPT_01_HPP_

#ifndef _PLAYER_HPP__
#define _PLAYER_HPP_
#include "object/player.hpp"
#endif // _PLAYER_HPP_

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
class Player;

class EnemyShootScript
{
public:
    int InitBombShoot(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty, Player player);
    void BombType00(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty, Player player);
    void BombType01(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty, Player player);
};