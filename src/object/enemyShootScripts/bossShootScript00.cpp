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

static inline double Len2(double x, double y) { return std::sqrt(x * x + y * y); }

void EnemyShootScript::Boss01Spell01(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS],
                                     int time, int difficulty, Player player)
{
    // ▼リング速度：全体的に遅め + 難易度で少し伸びる
    const double ringSpeed = 0.75 + difficulty * 0.10; // (例) d=1:0.85, d=3:1.05
    const int ringN0 = 14 + difficulty * 4;
    const int ringN1 = 10 + difficulty * 3;

    const int cycle = 240;
    const int burstEvery = 30;
    const int burstCount = 3;
    const int burstWindow = burstEvery * burstCount; // 90f
    const int pauseWindow = 60;

    Vec2d epos = enemy.getPosition();
    Vec2d ppos = player.getPosition();

    auto spawnAt = [&](Vec2d s, Vec2d v, int gfxType, int value)
    {
        int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, value);
        bombs[idx].type = gfxType;
        bombs[idx].pos = s;
        bombs[idx].vel = v;
    };

    int t = time % cycle;

    auto fireRing = [&](double phase, int N, double spd, Vec2d origin, int value)
    {
        for (int i = 0; i < N; ++i)
        {
            double phi = phase + (2.0 * PI * i) / (double)N;
            Vec2d v{spd * std::cos(phi), spd * std::sin(phi)};
            spawnAt(origin, v, (i % 3), value);
        }
    };

    Vec2d orbL{epos.x - 60.0, epos.y + 10.0};
    Vec2d orbR{epos.x + 60.0, epos.y + 10.0};

    bool inBurstA = (t < burstWindow);
    bool inBurstB = (t >= burstWindow + pauseWindow);

    if (enemy.enemyStatus.time >= 0)
    {
        if (inBurstA && (t % burstEvery == 0))
        {
            double phase = 0.02 * time;
            fireRing(phase, ringN0, ringSpeed, epos, 10);

            // ▼3way速度：ここが体感の速さの主因なのでしっかり落とす
            double aim = normalizeAngle(std::atan2(ppos.y - epos.y, ppos.x - epos.x));
            double sp = 1.25 + difficulty * 0.20; // (例) d=1:1.95, d=3:2.35
            double spread = 0.12 + 0.02 * difficulty;

            for (int k = -1; k <= 1; ++k)
            {
                double a = aim + k * spread;
                Vec2d v{sp * std::cos(a), sp * std::sin(a)};
                spawnAt((k == -1 ? orbL : (k == 1 ? orbR : epos)), v, 2, 7);
            }
        }
        else if (inBurstB && ((t - (burstWindow + pauseWindow)) % burstEvery == 0))
        {
            double phase = -0.018 * time + 0.7;
            Vec2d origin = (((t / burstEvery) % 2) == 0) ? orbL : orbR;
            fireRing(phase, ringN1, ringSpeed * 1.05, origin, 8); // 1.15→1.05に弱め
        }
    }

    // ▼水流ゆらぎ：回転・速度揺れを弱めて、加速しすぎないように
    const double rot = 0.00045 * (1 + difficulty); // 少し弱め
    const double amp = 0.08 + 0.03 * difficulty;   // 0.12+0.04*d → 控えめに

    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;

        double vx = bombs[i].vel.x, vy = bombs[i].vel.y;
        double ang = std::atan2(vy, vx);
        double spd = Len2(vx, vy);

        ang = normalizeAngle(ang + rot);
        spd = 1;

        bombs[i].vel.x = spd * std::cos(ang);
        bombs[i].vel.y = spd * std::sin(ang);
    }
}

void EnemyShootScript::Boss01Spell02(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS],
                                     int time, int difficulty, Player player)
{
    Vec2d e = enemy.getPosition();
    Vec2d p = player.getPosition();

    auto spawnAt = [&](Vec2d s, double sp, double ang, int gfxType, int value)
    {
        int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, value);
        bombs[idx].type = gfxType;
        bombs[idx].pos = s;
        bombs[idx].vel.x = sp * std::cos(ang);
        bombs[idx].vel.y = sp * std::sin(ang);
    };

    const int cycle = 210;
    const int fanWindow = 90;
    const int pauseWindow = 40;
    int t = time % cycle;

    Vec2d gunU{e.x, e.y - 20.0};
    Vec2d gunD{e.x, e.y + 30.0};

    if (enemy.enemyStatus.time >= 0)
    {
        if (t < fanWindow)
        {
            int interval = 16 - difficulty * 2;
            if (interval < 6)
                interval = 6;

            if (t % interval == 0)
            {
                double aim = normalizeAngle(std::atan2(p.y - e.y, p.x - e.x));
                int fanCount = 4 + difficulty * 2;
                double fanWidth = PI / 5.5;
                double sp = 2.0 + difficulty * 0.4;

                for (int i = 0; i < fanCount; ++i)
                {
                    double u = (fanCount == 1) ? 0.0 : (double)i / (fanCount - 1) - 0.5;
                    double ang = aim + fanWidth * u;
                    Vec2d src = (i == fanCount / 2) ? e : ((i % 2 == 0) ? gunU : gunD);
                    spawnAt(src, sp, ang, 1, 6);
                }
            }
        }
        else if (t < fanWindow + pauseWindow)
        {
        }
        else
        {
            const int ringEvery = 28;
            const int ringCount = 3;
            int tr = t - (fanWindow + pauseWindow);

            if (tr < ringEvery * ringCount && (tr % ringEvery == 0))
            {
                int N = 12 + difficulty * 4;
                double sp = 1.25 + difficulty * 0.22;
                double phase = 0.03 * time + 0.6;

                Vec2d L{e.x - 70.0, e.y + 5.0};
                Vec2d R{e.x + 70.0, e.y + 5.0};
                Vec2d src = ((tr / ringEvery) % 2 == 0) ? L : R;

                for (int i = 0; i < N; ++i)
                {
                    double ang = phase + (2.0 * PI * i) / (double)N;
                    spawnAt(src, sp, ang, (i % 3), 8);
                }
            }
        }
    }

    double waveStrength = 0.01 * difficulty;
    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;

        double vx = bombs[i].vel.x, vy = bombs[i].vel.y;
        double ang = std::atan2(vy, vx);
        double sp = Len2(vx, vy);

        ang += waveStrength * std::sin(0.05 * time + i * 0.27);
        bombs[i].vel.x = sp * std::cos(ang);
        bombs[i].vel.y = sp * std::sin(ang);
    }
}

void EnemyShootScript::Boss01Spell03(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS],
                                     int time, int difficulty, Player player)
{
    Vec2d e = enemy.getPosition();
    Vec2d p = player.getPosition();

    auto spawnAt = [&](Vec2d s, Vec2d v, int gfxType, int value)
    {
        int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, value);
        bombs[idx].type = gfxType;
        bombs[idx].pos = s;
        bombs[idx].vel = v;
    };

    const int cycle = 240;
    const int burstEvery = 18;
    const int burstCount = 3;
    const int burstWindow = burstEvery * burstCount;
    const int pauseWindow = 50;
    int t = time % cycle;

    const double upSpeed = 1.4 + difficulty * 0.30;
    const int N = 10 + difficulty * 4;

    if (enemy.enemyStatus.time >= 0)
    {
        bool burstA = (t < burstWindow);
        bool burstB = (t >= burstWindow + pauseWindow && t < (burstWindow + pauseWindow + burstWindow));

        if ((burstA && (t % burstEvery == 0)) ||
            (burstB && ((t - (burstWindow + pauseWindow)) % burstEvery == 0)))
        {
            for (int i = 0; i < N; ++i)
            {
                double off = ((i % 7) - 3) * 0.11;
                double ang = -PI / 2.0 + off;
                Vec2d v{upSpeed * std::cos(ang), upSpeed * std::sin(ang)};
                spawnAt(e, v, (i % 3), 8);
            }
        }

        const int laserCycle = 240;
        const int laserT0 = 120;
        const int laserGap = 36;
        const int laserLen = 18;
        const double laserStep = 14.0;
        const double laserSpeed = 3.0 + difficulty * 0.40;

        int lt = time % laserCycle;
        if (lt == laserT0 || lt == laserT0 + laserGap)
        {
            Vec2d src = (lt == laserT0) ? Vec2d{e.x - 180.0, e.y + 40.0} : Vec2d{e.x + 180.0, e.y + 40.0};
            double ang = normalizeAngle(std::atan2(p.y - src.y, p.x - src.x));

            Vec2d dir{std::cos(ang), std::sin(ang)};
            Vec2d v{laserSpeed * dir.x, laserSpeed * dir.y};

            for (int i = 0; i < laserLen; ++i)
            {
                Vec2d s{src.x - dir.x * (i * laserStep), src.y - dir.y * (i * laserStep)};
                spawnAt(s, v, 2, 6);
            }
        }
    }

    double gravity = 0.024 + 0.012 * difficulty;
    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;
        bombs[i].vel.y += gravity;
    }
}

// ---- Spell04-12 are included below (same as generated in the assistant response) ----
// For brevity in this generator, Spell04-12 are kept in the output file as well.

// (Spell04)
void EnemyShootScript::Boss01Spell04(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS],
                                     int time, int difficulty, Player player)
{
    Vec2d e = enemy.getPosition();

    auto spawnAt = [&](Vec2d s, double sp, double ang, int gfxType, int value)
    {
        int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, value);
        bombs[idx].type = gfxType;
        bombs[idx].pos = s;
        bombs[idx].vel.x = sp * std::cos(ang);
        bombs[idx].vel.y = sp * std::sin(ang);
    };

    const int cycle = 240;
    const int spiralOn = 120;
    const int pause = 40;
    int t = time % cycle;

    const int interval = 4;
    const double sp = 1.6 + difficulty * 0.25;
    const double w = 0.12 + 0.01 * difficulty;

    if (enemy.enemyStatus.time >= 0)
    {
        if (t < spiralOn || t >= spiralOn + pause)
        {
            if (time % interval == 0)
            {
                for (int k = 0; k < 2; ++k)
                {
                    double dir = (k == 0) ? 1.0 : -1.0;
                    double ang = normalizeAngle(time * w * dir + k * PI);
                    spawnAt(e, sp, ang, (k % 3), 6);
                }
            }

            if (time % 60 == 0)
            {
                int N = 10 + difficulty * 3;
                double rsp = 1.15 + difficulty * 0.2;
                double phase = 0.05 * time;

                Vec2d L{e.x - 55.0, e.y + 10.0};
                Vec2d R{e.x + 55.0, e.y + 10.0};
                Vec2d src = ((time / 60) % 2 == 0) ? L : R;

                for (int i = 0; i < N; ++i)
                {
                    double a = phase + (2.0 * PI * i) / (double)N;
                    spawnAt(src, rsp, a, (i % 3), 7);
                }
            }
        }
    }
}

// (Spell05)
void EnemyShootScript::Boss01Spell05(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS],
                                     int time, int difficulty, Player player)
{
    Vec2d e = enemy.getPosition();

    auto spawnRing = [&](int N, double sp, double phase, Vec2d src, int value)
    {
        for (int i = 0; i < N; ++i)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, value);
            bombs[idx].type = (i % 3);
            bombs[idx].pos = src;

            double ang = phase + (2.0 * PI * i) / (double)N;
            bombs[idx].vel.x = sp * std::cos(ang);
            bombs[idx].vel.y = sp * std::sin(ang);
        }
    };

    const int burstEvery = 30;
    const int burstCount = 3;
    const int pause = 45;
    const int block = burstEvery * burstCount + pause;
    int t = time % (block * 3);

    int kind = t / block;
    int tt = t % block;

    if (enemy.enemyStatus.time >= 0)
    {
        if (tt < burstEvery * burstCount && (tt % burstEvery == 0))
        {
            if (kind == 0)
            {
                int N = 18 + difficulty * 4;
                double sp = 1.05 + difficulty * 0.22;
                double phase = 0.03 * time;
                spawnRing(N, sp, phase, e, 10);
            }
            else if (kind == 1)
            {
                int N = 12 + difficulty * 3;
                double sp = 1.35 + difficulty * 0.25;
                double phase = -0.028 * time + 0.7;

                Vec2d A{e.x - 80.0, e.y + 5.0};
                Vec2d B{e.x + 80.0, e.y + 5.0};
                Vec2d src = ((tt / burstEvery) % 2 == 0) ? A : B;

                spawnRing(N, sp, phase, src, 8);
            }
            else
            {
                int N = 14 + difficulty * 4;
                double sp = 1.20 + difficulty * 0.23;
                double phase = 0.02 * time;

                for (int i = 0; i < N; ++i)
                {
                    int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 9);
                    bombs[idx].type = (i % 3);
                    bombs[idx].pos = e;

                    double jitter = 0.12 * std::sin(time * 0.03 + i * 1.3);
                    double ang = phase + (2.0 * PI * i) / (double)N + jitter;

                    bombs[idx].vel.x = sp * std::cos(ang);
                    bombs[idx].vel.y = sp * std::sin(ang);
                }
            }
        }
    }

    const double rot = 0.0018 + 0.0006 * difficulty;
    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;

        double vx = bombs[i].vel.x, vy = bombs[i].vel.y;
        double ang = std::atan2(vy, vx);
        double sp = Len2(vx, vy);

        ang = normalizeAngle(ang + rot);
        bombs[i].vel.x = sp * std::cos(ang);
        bombs[i].vel.y = sp * std::sin(ang);
    }
}

// (Spell06)
void EnemyShootScript::Boss01Spell06(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS],
                                     int time, int difficulty, Player player)
{
    Vec2d e = enemy.getPosition();
    Vec2d p = player.getPosition();

    auto spawnAt = [&](Vec2d s, Vec2d v, int gfxType, int value)
    {
        int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, value);
        bombs[idx].type = gfxType;
        bombs[idx].pos = s;
        bombs[idx].vel = v;
    };

    const int cycle = 210;
    const int on = 100;
    const int pause = 40;
    int t = time % cycle;

    if (enemy.enemyStatus.time >= 0)
    {
        if (t < on || t >= on + pause)
        {
            if (time % 14 == 0)
            {
                double aim = normalizeAngle(std::atan2(p.y - e.y, p.x - e.x));
                double sp = 2.4 + difficulty * 0.35;
                double spread = 0.10 + 0.02 * difficulty;

                for (int k = -1; k <= 1; ++k)
                {
                    double a = aim + k * spread;
                    Vec2d v{sp * std::cos(a), sp * std::sin(a)};
                    spawnAt(e, v, 2, 6);
                }
            }

            if (time % 60 == 0)
            {
                Vec2d L{e.x - 160.0, e.y + 40.0};
                Vec2d R{e.x + 160.0, e.y + 40.0};
                Vec2d src = ((time / 60) % 2 == 0) ? L : R;

                double ang = normalizeAngle(std::atan2(p.y - src.y, p.x - src.x));
                Vec2d dir{std::cos(ang), std::sin(ang)};
                double sp = 3.2 + difficulty * 0.45;
                Vec2d v{sp * dir.x, sp * dir.y};

                int len = 22;
                double step = 13.0;
                for (int i = 0; i < len; ++i)
                {
                    Vec2d s{src.x - dir.x * (i * step), src.y - dir.y * (i * step)};
                    spawnAt(s, v, 1, 6);
                }
            }
        }
    }
}

// (Spell07)
void EnemyShootScript::Boss01Spell07(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS],
                                     int time, int difficulty, Player player)
{
    Vec2d e = enemy.getPosition();

    const int burstEvery = 24;
    const int burstCount = 3;
    const int pause = 48;
    const int cycle = burstEvery * burstCount + pause;
    int t = time % cycle;

    if (enemy.enemyStatus.time >= 0)
    {
        if (t < burstEvery * burstCount && (t % burstEvery == 0))
        {
            int N = 7 + difficulty * 2;
            double sp = 1.8 + difficulty * 0.25;

            double center = PI * 0.5;
            double fan = 0.85;
            double phase = std::sin(0.03 * time) * 0.25;

            for (int i = 0; i < N; ++i)
            {
                int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 6);
                bombs[idx].type = (i % 3);
                bombs[idx].pos = e;

                double u = (N == 1) ? 0.0 : (double)i / (double)(N - 1);
                double ang = center + phase + (u - 0.5) * fan;

                bombs[idx].vel.x = sp * std::cos(ang);
                bombs[idx].vel.y = sp * std::sin(ang);
            }
        }
    }

    const int zig = 18;
    const double zigAmp = 0.07 + 0.02 * difficulty;
    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;

        if (bombs[i].time > 0 && (bombs[i].time % zig) == 0)
        {
            bombs[i].vel.x = -bombs[i].vel.x * (1.0 + zigAmp);
        }
    }
}

// (Spell08)
void EnemyShootScript::Boss01Spell08(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS],
                                     int time, int difficulty, Player player)
{
    Vec2d e = enemy.getPosition();

    auto fireRingAt = [&](Vec2d src, int N, double sp, double phase, int value)
    {
        for (int i = 0; i < N; ++i)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, value);
            bombs[idx].type = (i % 3);
            bombs[idx].pos = src;

            double ang = phase + (2.0 * PI * i) / (double)N;
            bombs[idx].vel.x = sp * std::cos(ang);
            bombs[idx].vel.y = sp * std::sin(ang);
        }
    };

    const int burstEvery = 36;
    const int burstCount = 3;
    const int pause = 54;
    const int cycle = burstEvery * burstCount + pause;
    int t = time % cycle;

    Vec2d p0{e.x - 90.0, e.y - 10.0};
    Vec2d p1{e.x + 90.0, e.y - 10.0};
    Vec2d p2{e.x - 90.0, e.y + 70.0};
    Vec2d p3{e.x + 90.0, e.y + 70.0};

    if (enemy.enemyStatus.time >= 0)
    {
        if (t < burstEvery * burstCount && (t % burstEvery == 0))
        {
            int N = 10 + difficulty * 4;
            double sp = 1.10 + difficulty * 0.22;
            double phase = 0.04 * time + 0.3;

            int k = (t / burstEvery) % 4;
            Vec2d src = (k == 0) ? p0 : (k == 1) ? p1
                                    : (k == 2)   ? p2
                                                 : p3;
            fireRingAt(src, N, sp, phase, 8);
        }
    }
}

// (Spell09)
void EnemyShootScript::Boss01Spell09(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS],
                                     int time, int difficulty, Player player)
{
    Vec2d e = enemy.getPosition();
    Vec2d p = player.getPosition();

    auto spawnAt = [&](Vec2d s, Vec2d v, int gfxType, int value)
    {
        int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, value);
        bombs[idx].type = gfxType;
        bombs[idx].pos = s;
        bombs[idx].vel = v;
    };

    const int interval = 4;
    const int flipT = 180;
    const double w = 0.12 + 0.01 * difficulty;
    const double sp = 1.7 + difficulty * 0.25;

    if (enemy.enemyStatus.time >= 0 && time % interval == 0)
    {
        double dir = ((time / flipT) % 2 == 0) ? 1.0 : -1.0;
        double ang = normalizeAngle(time * w * dir);
        Vec2d v{sp * std::cos(ang), sp * std::sin(ang)};
        spawnAt(e, v, (time / interval) % 3, 6);
    }

    if (enemy.enemyStatus.time >= 0 && (time % flipT) == 10)
    {
        Vec2d src{e.x, e.y + 40.0};
        double ang = normalizeAngle(std::atan2(p.y - src.y, p.x - src.x));
        Vec2d dir{std::cos(ang), std::sin(ang)};
        double lsp = 3.0 + difficulty * 0.45;
        Vec2d v{lsp * dir.x, lsp * dir.y};

        int len = 26;
        double step = 12.0;
        for (int i = 0; i < len; ++i)
        {
            Vec2d s{src.x - dir.x * (i * step), src.y - dir.y * (i * step)};
            spawnAt(s, v, 2, 6);
        }
    }
}

// (Spell10)
void EnemyShootScript::Boss01Spell10(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS],
                                     int time, int difficulty, Player player)
{
    Vec2d e = enemy.getPosition();
    Vec2d p = player.getPosition();

    auto spawnAt = [&](Vec2d s, double sp, double ang, int gfxType, int value)
    {
        int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, value);
        bombs[idx].type = gfxType;
        bombs[idx].pos = s;
        bombs[idx].vel.x = sp * std::cos(ang);
        bombs[idx].vel.y = sp * std::sin(ang);
    };

    const int shotEvery = 24;
    const int shotCount = 3;
    const int pause = 36;
    const int block = shotEvery * shotCount + pause;
    int t = time % (block * 2);
    bool modeAimed = (t < block);
    int tt = t % block;

    if (enemy.enemyStatus.time >= 0)
    {
        if (tt < shotEvery * shotCount && (tt % shotEvery == 0))
        {
            if (modeAimed)
            {
                double aim = normalizeAngle(std::atan2(p.y - e.y, p.x - e.x));
                double sp = 2.5 + difficulty * 0.35;
                double spread = 0.12 + 0.02 * difficulty;

                for (int k = -1; k <= 1; ++k)
                {
                    double ang = aim + k * spread;
                    Vec2d src = (k == -1) ? Vec2d{e.x - 65.0, e.y + 25.0} : (k == 1) ? Vec2d{e.x + 65.0, e.y + 25.0}
                                                                                     : e;
                    spawnAt(src, sp, ang, 2, 7);
                }
            }
            else
            {
                int N = 12 + difficulty * 4;
                double sp = 1.25 + difficulty * 0.22;
                double phase = 0.03 * time + 0.6;
                Vec2d src = e;

                for (int i = 0; i < N; ++i)
                {
                    double ang = phase + (2.0 * PI * i) / (double)N;
                    spawnAt(src, sp, ang, (i % 3), 8);
                }
            }
        }
    }
}

// (Spell11)
void EnemyShootScript::Boss01Spell11(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS],
                                     int time, int difficulty, Player player)
{
    Vec2d e = enemy.getPosition();

    auto spawnAt = [&](Vec2d s, Vec2d v, int gfxType, int value)
    {
        int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, value);
        bombs[idx].type = gfxType;
        bombs[idx].pos = s;
        bombs[idx].vel = v;
    };

    const int laneCount = 5 + difficulty;
    const double laneSpan = 70.0;
    const int rainInterval = 10;

    if (enemy.enemyStatus.time >= 0 && time % rainInterval == 0)
    {
        double sp = 0.8 + difficulty * 0.12;

        for (int i = 0; i < laneCount; ++i)
        {
            double u = (laneCount == 1) ? 0.0 : (double)i / (double)(laneCount - 1) - 0.5;
            Vec2d src{e.x + u * laneSpan * 2.0, e.y - 40.0};
            Vec2d v{0.0, sp};
            spawnAt(src, v, (i % 3), 6);
        }
    }

    const int ringEvery = 36;
    const int ringCount = 3;
    const int pause = 54;
    const int cycle = ringEvery * ringCount + pause;
    int t = time % cycle;

    if (enemy.enemyStatus.time >= 0 && t < ringEvery * ringCount && (t % ringEvery == 0))
    {
        int N = 10 + difficulty * 3;
        double sp = 1.0 + difficulty * 0.18;
        double phase = 0.05 * time;

        Vec2d src = ((t / ringEvery) % 2 == 0) ? Vec2d{e.x - 80.0, e.y + 10.0} : Vec2d{e.x + 80.0, e.y + 10.0};

        for (int i = 0; i < N; ++i)
        {
            double ang = phase + (2.0 * PI * i) / (double)N;
            Vec2d v{sp * std::cos(ang), sp * std::sin(ang)};
            spawnAt(src, v, (i % 3), 7);
        }
    }

    const double gravity = 0.03 + 0.014 * difficulty;
    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;
        bombs[i].vel.y += gravity;
        bombs[i].vel.x += (0.0015 + 0.0005 * difficulty) * std::sin(0.06 * time + i * 0.31);
    }
}

// (Spell12)
void EnemyShootScript::Boss01Spell12(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS],
                                     int time, int difficulty, Player player)
{
    Vec2d e = enemy.getPosition();
    Vec2d p = player.getPosition();

    auto spawnAt = [&](Vec2d s, Vec2d v, int gfxType, int value)
    {
        int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, value);
        bombs[idx].type = gfxType;
        bombs[idx].pos = s;
        bombs[idx].vel = v;
    };

    const int ringEvery = 24;
    const int ringCount = 3;
    const int pauseA = 36;
    const int followEvery = 18;
    const int followCount = 3;
    const int pauseB = 30;

    const int block = ringEvery * ringCount + pauseA + followEvery * followCount + pauseB + 1;
    int t = time % block;

    double satR = 85.0;
    double satW = 0.03;
    double satA = time * satW;
    Vec2d sat0{e.x + std::cos(satA) * satR, e.y + std::sin(satA) * satR};
    Vec2d sat1{e.x + std::cos(satA + PI) * satR, e.y + std::sin(satA + PI) * satR};

    if (enemy.enemyStatus.time >= 0)
    {
        if (t < ringEvery * ringCount && (t % ringEvery == 0))
        {
            int shotNo = t / ringEvery;
            int kind = (time / block) % 3;

            if (kind == 0)
            {
                int N = 18 + difficulty * 5;
                double sp = 1.15 + difficulty * 0.24;
                double phase = 0.035 * time + 0.2 * shotNo;

                for (int i = 0; i < N; ++i)
                {
                    double ang = phase + (2.0 * PI * i) / (double)N;
                    Vec2d v{sp * std::cos(ang), sp * std::sin(ang)};
                    spawnAt(e, v, (i % 3), 10);
                }
            }
            else if (kind == 1)
            {
                int N = 12 + difficulty * 4;
                double sp = 1.45 + difficulty * 0.28;
                double phase = -0.03 * time + 0.7;

                Vec2d src = (shotNo % 2 == 0) ? sat0 : sat1;
                for (int i = 0; i < N; ++i)
                {
                    double ang = phase + (2.0 * PI * i) / (double)N;
                    Vec2d v{sp * std::cos(ang), sp * std::sin(ang)};
                    spawnAt(src, v, (i % 3), 9);
                }
            }
            else
            {
                int N = 16 + difficulty * 4;
                double sp = 1.25 + difficulty * 0.26;
                double phase = 0.025 * time;

                for (int i = 0; i < N; ++i)
                {
                    double jitter = 0.10 * std::sin(time * 0.04 + i * 1.6);
                    double ang = phase + (2.0 * PI * i) / (double)N + jitter;
                    Vec2d v{sp * std::cos(ang), sp * std::sin(ang)};
                    spawnAt(e, v, (i % 3), 10);
                }
            }
        }
        else if (t < ringEvery * ringCount + pauseA)
        {
        }
        else if (t < ringEvery * ringCount + pauseA + followEvery * followCount)
        {
            int tt = t - (ringEvery * ringCount + pauseA);
            if (tt % followEvery == 0)
            {
                double aim = normalizeAngle(std::atan2(p.y - e.y, p.x - e.x));
                double sp = 2.9 + difficulty * 0.45;
                double spread = 0.12 + 0.02 * difficulty;

                for (int k = -1; k <= 1; ++k)
                {
                    double ang = aim + k * spread;
                    Vec2d src = (k == -1) ? sat0 : (k == 1) ? sat1
                                                            : e;
                    Vec2d v{sp * std::cos(ang), sp * std::sin(ang)};
                    spawnAt(src, v, 2, 7);
                }
            }
        }
        else if (t < ringEvery * ringCount + pauseA + followEvery * followCount + pauseB)
        {
        }
        else
        {
            Vec2d src = sat0;
            double ang = normalizeAngle(std::atan2(p.y - src.y, p.x - src.x));
            Vec2d dir{std::cos(ang), std::sin(ang)};
            double lsp = 3.4 + difficulty * 0.55;
            Vec2d v{lsp * dir.x, lsp * dir.y};

            int len = 28;
            double step = 12.0;
            for (int i = 0; i < len; ++i)
            {
                Vec2d s{src.x - dir.x * (i * step), src.y - dir.y * (i * step)};
                spawnAt(s, v, 1, 6);
            }
        }
    }

    const double rot = 0.0025 + 0.0008 * difficulty;
    const double amp = 0.18 + 0.06 * difficulty;

    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;

        double vx = bombs[i].vel.x, vy = bombs[i].vel.y;
        double ang = std::atan2(vy, vx);
        double sp = Len2(vx, vy);

        ang = normalizeAngle(ang + rot);
        sp *= (1.0 + amp * std::sin(0.04 * time + i * 0.21));

        bombs[i].vel.x = sp * std::cos(ang);
        bombs[i].vel.y = sp * std::sin(ang);
    }
}
