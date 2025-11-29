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

// void EnemyShootScript::Boss01Spell01(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
// {
//     double baseSpeed = 1.2 + difficulty * 0.4; // 弾速
//     int shotInterval = 6;                      // 何フレームごとに撃つか
//     int bulletPerWave = 16 + difficulty * 4;   // 一波の弾数

//     // --- 発射部分 ---
//     if (enemy.enemyStatus.time >= 0 && time % shotInterval == 0)
//     {
//         // time に応じて基準角をゆっくり変える → 回転する花弾
//         double baseAngle = 0.03 * time;

//         for (int i = 0; i < bulletPerWave; ++i)
//         {
//             int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 3);
//             bombs[idx].type = 0; // 見た目用に適宜変更

//             double phi = baseAngle + (2.0 * PI * i) / bulletPerWave;
//             bombs[idx].vel.x = baseSpeed * std::cos(phi);
//             bombs[idx].vel.y = baseSpeed * std::sin(phi);
//         }
//     }

//     // --- 弾の進行方向をじわっと回転させる部分 ---
//     double rotateSpeed = 0.0005 * (1 + difficulty); // 回転速度（難易度で強く）
//     for (int i = 0; i < MAX_BOMBS; ++i)
//     {
//         if (!bombs[i].isUsing)
//             continue;
//         if (bombs[i].id != enemy.getId())
//             continue;

//         double phi = std::atan2(bombs[i].vel.y, bombs[i].vel.x);
//         phi = normalizeAngle(phi) + rotateSpeed;
//         double norm = std::sqrt(bombs[i].vel.x * bombs[i].vel.x + bombs[i].vel.y * bombs[i].vel.y);
//         bombs[i].vel.x = norm * std::cos(phi);
//         bombs[i].vel.y = norm * std::sin(phi);
//     }
// }

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

// void EnemyShootScript::Boss01Spell01(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
// {
//     int localTime = enemy.enemyStatus.time; // ボスのスペル時間を想定
//     if (localTime < 0)
//         return;

//     // フェーズの切替タイミング
//     const int PHASE1_END = 600;  // 約10秒（60fps想定）
//     const int PHASE2_END = 1200; // その後10秒

//     if (localTime < PHASE1_END)
//     {
//         // --- フェーズ1：回転リング弾 ---
//         double speed = 1.2 + 0.3 * difficulty;
//         int shotInterval = 12;
//         int bulletPerWave = 24;

//         if (time % shotInterval == 0)
//         {
//             double baseAngle = 0.02 * time;
//             for (int i = 0; i < bulletPerWave; ++i)
//             {
//                 int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 15);
//                 bombs[idx].type = 0;

//                 double phi = baseAngle + (2.0 * PI * i) / bulletPerWave;
//                 bombs[idx].vel.x = speed * std::cos(phi);
//                 bombs[idx].vel.y = speed * std::sin(phi);
//             }
//         }

//         // わずかに回転
//         double rot = 0.0003 * (1 + difficulty);
//         for (int i = 0; i < MAX_BOMBS; ++i)
//         {
//             if (!bombs[i].isUsing)
//                 continue;
//             if (bombs[i].id != enemy.getId())
//                 continue;

//             double phi = std::atan2(bombs[i].vel.y, bombs[i].vel.x);
//             phi = normalizeAngle(phi) + rot;
//             double norm = std::sqrt(bombs[i].vel.x * bombs[i].vel.x + bombs[i].vel.y * bombs[i].vel.y);
//             bombs[i].vel.x = norm * std::cos(phi);
//             bombs[i].vel.y = norm * std::sin(phi);
//         }
//     }
//     else if (localTime < PHASE2_END)
//     {
//         // --- フェーズ2：自機狙い針弾ラッシュ ---
//         // TODO: 実際のプレイヤー座標取得に合わせて修正
//         double ex = enemy.enemyStatus.pos.x;
//         double ey = enemy.enemyStatus.pos.y;
//         double px = player.getPosition().x; // 仮
//         double py = player.getPosition().y; // 仮

//         double dx = px - ex;
//         double dy = py - ey;
//         double baseAngle = std::atan2(dy, dx);

//         double speed = 3.0 + 0.5 * difficulty;
//         int shotInterval = 6;
//         int spread = 2 + difficulty; // 多少ブレさせる本数

//         if (time % shotInterval == 0)
//         {
//             for (int i = -spread; i <= spread; ++i)
//             {
//                 double offset = 0.03 * i;
//                 int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 15);
//                 bombs[idx].type = 1;
//                 double phi = baseAngle + offset;
//                 bombs[idx].vel.x = speed * std::cos(phi);
//                 bombs[idx].vel.y = speed * std::sin(phi);
//             }
//         }
//     }
//     else
//     {
//         // PHASE2_END を超えたら、次のスペルへ移行するなど
//         // ここでは特に何もしない（外側のステートマシンで切り替え）
//     }
// }

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
