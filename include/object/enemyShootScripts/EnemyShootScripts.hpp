#pragma once

#ifndef _ENEMY_HPP_
#define _ENEMY_HPP_
#include "object/enemy.hpp"
#endif // _ENEMY_HPP_s

#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_HPP_

#ifndef _ENEMY_SHOOT_SCRIPT_01_HPP_
#define _ENEMY_SHOOT_SCRIPT_01_HPP_
#include "object/enemyShootScripts/EnemyShootScripts.hpp"
#endif // _ENEMY_SHOOT_SCRIPT_01_HPP_

#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_
#include "object/player.hpp"
#endif // _PLAYER_HPP_

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES // for C++
#include <cmath>

#endif

constexpr double PI = 3.14159265358979323846;
constexpr double TWO_PI = 2.0 * PI; // ← 必ず初期化

#ifndef _NORM_
#define _NORM_
// 角度正規化（0～2π）
inline double normalizeAngle(double a)
{
    const double TWO_PI = 2.0 * PI;
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
    int InitBombShoot(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player, double radius);
    // void BombType00(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void BombType01(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void BombType02(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void BombType03(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void BombType04(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void BombType05(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void BombType06(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void BombType07(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void BombType08(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void BombType09(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void BombType10(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);

    void Boss01Spell01(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void Boss01Spell02(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void Boss01Spell03(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void Boss01Spell04(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void Boss01Spell05(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void Boss01Spell06(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void Boss01Spell07(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void Boss01Spell08(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void Boss01Spell09(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void Boss01Spell10(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void Boss01Spell11(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
    void Boss01Spell12(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player);
};