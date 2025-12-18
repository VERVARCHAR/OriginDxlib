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

void EnemyShootScript::Boss01Spell01(Enemy enemy, BombManager bMgr,
                                     BombInfo bombs[MAX_BOMBS],
                                     int time, int difficulty, Player player)
{
    double baseSpeed = 1.2 + difficulty * 0.4;
    int shotInterval = 10;
    int bulletPerWave = 16 + difficulty * 4;

    // --- 発射：回転しながら広がる水弾リング ---
    if (enemy.enemyStatus.time >= 0 && time % shotInterval == 0)
    {
        double baseAngle = 0.02 * time; // 時間で基準角を回転

        for (int i = 0; i < bulletPerWave; ++i)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 10);
            bombs[idx].type = 0; // ここは青い水弾グラフィックにするとなお良い

            double phi = baseAngle + (2.0 * PI * i) / bulletPerWave;
            bombs[idx].vel.x = baseSpeed * std::cos(phi);
            bombs[idx].vel.y = baseSpeed * std::sin(phi);
        }
    }

    // --- 既存弾に「水流」のゆらぎ＋わずかな回転を与える ---
    double rotateSpeed = 0.0005 * (1 + difficulty);
    double waveAmplitude = 0.3 + 0.1 * difficulty; // 速度の揺れの強さ
    double waveFreq = 0.04;                        // 揺れの周期

    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;

        double vx = bombs[i].vel.x;
        double vy = bombs[i].vel.y;
        double phi = std::atan2(vy, vx);
        double norm = std::sqrt(vx * vx + vy * vy);

        // ちょっとずつ回転
        phi = normalizeAngle(phi) + rotateSpeed;

        // 全体速度を時間＋ID依存で揺らす → 水面の波
        double wave = 1.0 + waveAmplitude * std::sin(waveFreq * time + i * 0.2);
        double newSpeed = baseSpeed * 0.6 + norm * 0.4; // 元の速さも少し混ぜる
        newSpeed *= wave;

        bombs[i].vel.x = newSpeed * std::cos(phi);
        bombs[i].vel.y = newSpeed * std::sin(phi);
    }
}

void EnemyShootScript::Boss01Spell02(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    // TODO: 実際のプレイヤー座標の取得方法に合わせて書き換えてください
    double px = player.getPosition().x;
    double py = player.getPosition().y;
    double ex = enemy.enemyStatus.pos.x;
    double ey = enemy.enemyStatus.pos.y;

    double dx = px - ex;
    double dy = py - ey;
    double baseAngle = std::atan2(dy, dx); // 自機狙い角度
    double speed = 2.0 + difficulty * 0.4;

    int shotInterval = 18 - difficulty * 2; // 難易度で短く（早く）する
    if (shotInterval < 6)
        shotInterval = 6;

    if (enemy.enemyStatus.time >= 0 && time % shotInterval == 0)
    {
        int fanCount = 4 + difficulty * 2; // 扇の本数
        double fanWidth = PI / 6.0;        // 扇全体の角幅

        for (int i = 0; i < fanCount; ++i)
        {
            double t = (fanCount == 1) ? 0.0 : (double)i / (fanCount - 1) - 0.5; // [-0.5, 0.5]
            double phi = baseAngle + fanWidth * t;

            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 5);
            bombs[idx].type = 1; // スプライト変えたいなら変更
            bombs[idx].vel.x = speed * std::cos(phi);
            bombs[idx].vel.y = speed * std::sin(phi);
        }
    }

    // --- 軌道にゆらぎを与える部分 ---
    double waveStrength = 0.02 * difficulty; // 難易度によって揺れ強め
    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;

        double vx = bombs[i].vel.x;
        double vy = bombs[i].vel.y;
        double angle = std::atan2(vy, vx);
        double norm = std::sqrt(vx * vx + vy * vy);

        // time を使って全体に「ぐにゃっ」とした揺れを付ける
        angle += waveStrength * std::sin(0.05 * time + i * 0.3);

        bombs[i].vel.x = norm * std::cos(angle);
        bombs[i].vel.y = norm * std::sin(angle);
    }
}

void EnemyShootScript::Boss01Spell03(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    double speed = 1.5 + difficulty * 0.3;
    int shotInterval = 15;
    int bulletPerWave = 10 + difficulty * 4;

    // --- 発射：ボス付近から上方向にばら撒く ---
    if (enemy.enemyStatus.time >= 0 && time % shotInterval == 0)
    {
        for (int i = 0; i < bulletPerWave; ++i)
        {
            double randOffset = ((i % 5) - 2) * 0.12; // 少し左右に散らす

            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 8);
            bombs[idx].type = 2;

            double phi = -PI / 2.0 + randOffset; // 基本は上向き(-90°)
            bombs[idx].vel.x = speed * std::cos(phi);
            bombs[idx].vel.y = speed * std::sin(phi);
        }
    }

    // --- 重力風の加速度を加える ---
    double gravity = 0.02 + 0.01 * difficulty; // 下向き加速度
    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;

        bombs[i].vel.y += gravity; // 下方向に徐々に加速
    }
}

//==========================================================
// Boss01Spell04 : 二重螺旋（スパイラル+反転）
//   - 2本の螺旋を同時に吐く（回転方向が逆）
//   - 東方の基本「ぐるぐる」系、stage02開幕向け
//==========================================================
void EnemyShootScript::Boss01Spell04(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const double speed = 1.6 + difficulty * 0.25;
    const int interval = 3;
    const double w = 0.11 + 0.01 * difficulty;

    if (enemy.enemyStatus.time >= 0 && time % interval == 0)
    {
        for (int k = 0; k < 2; ++k)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 6);
            bombs[idx].type = k % 3; // 見た目用(0/1/2)

            double dir = (k == 0) ? 1.0 : -1.0;
            double phi = normalizeAngle(time * w * dir + k * PI);

            bombs[idx].vel.x = speed * std::cos(phi);
            bombs[idx].vel.y = speed * std::sin(phi);
        }
    }

    // ほんの少し「水流」っぽく揺らす（密度が上がっても硬くならない）
    const double wobble = 0.0015 * (1 + difficulty);
    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;

        double vx = bombs[i].vel.x, vy = bombs[i].vel.y;
        double ang = std::atan2(vy, vx);
        double sp = std::sqrt(vx * vx + vy * vy);

        ang += wobble * std::sin(0.03 * time + i * 0.17);
        bombs[i].vel.x = sp * std::cos(ang);
        bombs[i].vel.y = sp * std::sin(ang);
    }
}

//==========================================================
// Boss01Spell05 : リング→回転加速（花弁リング強化）
//   - 一定間隔でリング生成
//   - 既存弾にゆっくり回転+微加速（「舞う」感）
//==========================================================
void EnemyShootScript::Boss01Spell05(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const int ringInterval = 80;
    const int N = 14 + difficulty * 4;
    const double baseSpeed = 1.1 + difficulty * 0.25;

    if (enemy.enemyStatus.time >= 0 && time % ringInterval == 0)
    {
        double phase = 0.12 * (time / ringInterval);

        for (int i = 0; i < N; ++i)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 8);
            bombs[idx].type = (i % 3);

            double phi = phase + (2.0 * PI * i) / (double)N;
            bombs[idx].vel.x = baseSpeed * std::cos(phi);
            bombs[idx].vel.y = baseSpeed * std::sin(phi);
        }
    }

    const double rot = 0.002 + 0.0006 * difficulty;
    const double accel = 1.0015 + 0.0005 * difficulty;

    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;

        double vx = bombs[i].vel.x, vy = bombs[i].vel.y;
        double ang = std::atan2(vy, vx);
        double sp = std::sqrt(vx * vx + vy * vy);

        ang = normalizeAngle(ang + rot);
        sp *= accel;

        bombs[i].vel.x = sp * std::cos(ang);
        bombs[i].vel.y = sp * std::sin(ang);
    }
}

//==========================================================
// Boss01Spell06 : フェイント狙い（遅い→急に狙い直し）
//   - 発射時は「外す」角度
//   - 一定時間後に自機狙いへ切り替え（東方の嫌らしさ）
//==========================================================
void EnemyShootScript::Boss01Spell06(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const int interval = 12;
    const double speed0 = 1.2 + difficulty * 0.20;
    const double speed1 = 3.0 + difficulty * 0.35;

    // 発射：わざとズラした狙い
    if (enemy.enemyStatus.time >= 0 && time % interval == 0)
    {
        Vec2d p = player.getPosition();
        Vec2d e = enemy.getPosition();

        double aim = std::atan2(p.y - e.y, p.x - e.x);
        aim = normalizeAngle(aim);

        double miss = 0.35 * std::sin(0.07 * time); // 外し量
        double phi = aim + miss;

        int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 7);
        bombs[idx].type = 2;

        bombs[idx].vel.x = speed0 * std::cos(phi);
        bombs[idx].vel.y = speed0 * std::sin(phi);
    }

    // 既存弾：一定時間後に「狙い直し」＋加速
    const int retargetT = 45 - difficulty * 5; // 難易度ほど早く
    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;

        if (bombs[i].time == retargetT)
        {
            Vec2d p = player.getPosition();
            double ang = std::atan2(p.y - bombs[i].pos.y, p.x - bombs[i].pos.x);
            ang = normalizeAngle(ang);

            bombs[i].vel.x = speed1 * std::cos(ang);
            bombs[i].vel.y = speed1 * std::sin(ang);
        }
    }
}

//==========================================================
// Boss01Spell07 : カーテン（扇＋位相ずらし）
//   - 真下中心の扇を、位相をずらしながら連射
//   - stage02ラスト：見た目が“弾幕”になる
//==========================================================
void EnemyShootScript::Boss01Spell07(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const int interval = 8;
    const int N = 7 + difficulty * 2;
    const double baseSpeed = 1.7 + difficulty * 0.25;

    const double center = PI * 0.5;
    const double fan = 0.80; // 広め

    if (enemy.enemyStatus.time >= 0 && time % interval == 0)
    {
        double phase = std::sin(0.03 * time) * 0.22;

        for (int i = 0; i < N; ++i)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 6);
            bombs[idx].type = i % 3;

            double t = (N == 1) ? 0.0 : (double)i / (double)(N - 1);
            double phi = center + phase + (t - 0.5) * fan;

            double sp = baseSpeed + 0.06 * std::sin(0.04 * time + i);
            bombs[idx].vel.x = sp * std::cos(phi);
            bombs[idx].vel.y = sp * std::sin(phi);
        }
    }

    // “流れ”感（ほんの少し角度揺らし）
    const double w = 0.001 + 0.0004 * difficulty;
    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;

        double vx = bombs[i].vel.x, vy = bombs[i].vel.y;
        double ang = std::atan2(vy, vx);
        double sp = std::sqrt(vx * vx + vy * vy);

        ang += w * std::sin(0.05 * time + i * 0.25);
        bombs[i].vel.x = sp * std::cos(ang);
        bombs[i].vel.y = sp * std::sin(ang);
    }
}

//==========================================================
// Boss01Spell08 : “壁”リング（外周に寄せる圧）
//   - リングを生成しつつ、既存弾を外側へ押し出すイメージ
//   - stage03開幕：避けの「配置」を作る
//==========================================================
void EnemyShootScript::Boss01Spell08(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const int ringInterval = 70;
    const int N = 16 + difficulty * 5;
    const double speed = 1.0 + difficulty * 0.22;

    if (enemy.enemyStatus.time >= 0 && time % ringInterval == 0)
    {
        double phase = 0.07 * (time / ringInterval);

        for (int i = 0; i < N; ++i)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 10);
            bombs[idx].type = (i % 3);

            double phi = phase + (2.0 * PI * i) / (double)N;
            bombs[idx].vel.x = speed * std::cos(phi);
            bombs[idx].vel.y = speed * std::sin(phi);
        }
    }

    // 外へ広げる（速度が遅い弾ほど少し押し出す）
    const double push = 0.002 + 0.0008 * difficulty;
    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;

        double vx = bombs[i].vel.x, vy = bombs[i].vel.y;
        double sp = std::sqrt(vx * vx + vy * vy);
        if (sp < 1e-6)
            continue;

        bombs[i].vel.x += (vx / sp) * push;
        bombs[i].vel.y += (vy / sp) * push;
    }
}

//==========================================================
// Boss01Spell09 : 反転螺旋（慣れ破壊）
//   - 一定時間ごとに回転方向を反転させる
//   - 東方の「読みにくさ」を作る定番ギミック
//==========================================================
void EnemyShootScript::Boss01Spell09(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const int interval = 4;
    const double speed = 1.7 + difficulty * 0.25;
    const int flipT = 180; // 3秒ごとに反転
    const double w = 0.12 + 0.01 * difficulty;

    if (enemy.enemyStatus.time >= 0 && time % interval == 0)
    {
        double dir = ((time / flipT) % 2 == 0) ? 1.0 : -1.0;
        double phi = normalizeAngle(time * w * dir);

        int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 6);
        bombs[idx].type = (time / interval) % 3;

        bombs[idx].vel.x = speed * std::cos(phi);
        bombs[idx].vel.y = speed * std::sin(phi);
    }
}

//==========================================================
// Boss01Spell10 : 交互狙い（偶数:狙い / 奇数:外し）
//   - 狙い弾と外し弾を交互に混ぜる
//   - “同じ速度”だと読みやすいので少しだけ差を付ける
//==========================================================
void EnemyShootScript::Boss01Spell10(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const int interval = 10;
    const double speedAim = 2.6 + difficulty * 0.35;
    const double speedMiss = 2.0 + difficulty * 0.25;

    if (enemy.enemyStatus.time >= 0 && time % interval == 0)
    {
        Vec2d p = player.getPosition();
        Vec2d e = enemy.getPosition();
        double aim = normalizeAngle(std::atan2(p.y - e.y, p.x - e.x));

        bool aimed = ((time / interval) % 2 == 0);
        double miss = aimed ? 0.0 : (0.45 * std::sin(0.04 * time)); // 外し

        double phi = aim + miss;
        double sp = aimed ? speedAim : speedMiss;

        // 2発（左右対称）で圧を作る
        for (int k = 0; k < 2; ++k)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 8);
            bombs[idx].type = aimed ? 2 : 1;

            double off = (k == 0) ? -0.12 : 0.12;
            bombs[idx].vel.x = sp * std::cos(phi + off);
            bombs[idx].vel.y = sp * std::sin(phi + off);
        }
    }
}

//==========================================================
// Boss01Spell11 : 重力雨（上打ち→落下）改
//   - Spell03の強化：上向きに吐いて、落下で壁を作る
//   - 落下速度が揃うと単調なので、少しだけ左右に揺らす
//==========================================================
void EnemyShootScript::Boss01Spell11(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const double speed = 1.7 + difficulty * 0.25;
    const int interval = 12;
    const int N = 10 + difficulty * 4;

    if (enemy.enemyStatus.time >= 0 && time % interval == 0)
    {
        for (int i = 0; i < N; ++i)
        {
            double spread = ((i % 7) - 3) * 0.10;

            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 8);
            bombs[idx].type = i % 3;

            double phi = -PI / 2.0 + spread; // 上向き
            bombs[idx].vel.x = speed * std::cos(phi);
            bombs[idx].vel.y = speed * std::sin(phi);
        }
    }

    // 重力＋左右の微揺れ
    const double gravity = 0.025 + 0.012 * difficulty;
    const double sway = 0.003 + 0.001 * difficulty;

    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;

        bombs[i].vel.y += gravity;
        bombs[i].vel.x += sway * std::sin(0.06 * time + i * 0.33);
    }
}

//==========================================================
// Boss01Spell12 : 桜符「舞い散る花弁」（締め）
//   - 小リングを高頻度で重ねる＋回転付与
//   - stage03ラストの“東方っぽい”見栄え重視
//==========================================================
void EnemyShootScript::Boss01Spell12(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const int interval = 18;
    const int N = 9 + difficulty * 3;
    const double speed = 1.2 + difficulty * 0.22;

    if (enemy.enemyStatus.time >= 0 && time % interval == 0)
    {
        double phase = 0.18 * (time / interval);

        for (int i = 0; i < N; ++i)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 6);
            bombs[idx].type = (i % 3);

            // “花弁”っぽく、均等ではなく少しズラす
            double jitter = 0.08 * std::sin(i * 1.7 + time * 0.02);
            double phi = phase + (2.0 * PI * i) / (double)N + jitter;

            bombs[idx].vel.x = speed * std::cos(phi);
            bombs[idx].vel.y = speed * std::sin(phi);
        }
    }

    // 舞い（回転＋減速気味→再加速気味の揺らぎ）
    const double rot = 0.003 + 0.0007 * difficulty;
    const double amp = 0.22 + 0.06 * difficulty;

    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        if (!bombs[i].isUsing)
            continue;
        if (bombs[i].id != enemy.getId())
            continue;

        double vx = bombs[i].vel.x, vy = bombs[i].vel.y;
        double ang = std::atan2(vy, vx);
        double sp = std::sqrt(vx * vx + vy * vy);

        ang = normalizeAngle(ang + rot);

        double wave = 1.0 + amp * std::sin(0.04 * time + i * 0.21);
        double newSp = (sp * 0.85 + speed * 0.15) * wave;

        bombs[i].vel.x = newSp * std::cos(ang);
        bombs[i].vel.y = newSp * std::sin(ang);
    }
}
