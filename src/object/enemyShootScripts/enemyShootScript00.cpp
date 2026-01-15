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

int EnemyShootScript::InitBombShoot(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player, double radius)
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

//==========================================================
// BombType01 : 連射・自機狙い（基本の撒き）
//   - 一定区間だけ自機狙い弾を連射
//   - 東方の「通常弾：狙い連射」枠
//==========================================================
void EnemyShootScript::BombType01(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const double speed = 2.0 + difficulty * 0.45;
    const int interval = 8; // 連射間隔

    // 発射（区間制御：周期の後半だけ撃つ）
    if (time % interval == 0)
    {
        if (60 < time % (int)(120 * (1 + difficulty * 0.20)))
        {
            Vec2d playerPos = player.getPosition();
            Vec2d enemyPos = enemy.getPosition();

            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 10);
            bombs[idx].type = 1; // 画像type（0/1/2のみ想定）

            double phi = std::atan2((playerPos.y - enemyPos.y), (playerPos.x - enemyPos.x));
            phi = normalizeAngle(phi);

            bombs[idx].vel.x = speed * std::cos(phi);
            bombs[idx].vel.y = speed * std::sin(phi);
        }
    }

    // 補正（現状の書き方を踏襲：角度正規化のみ）
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing && bombs[i].id == enemy.getId())
        {
            double phi = std::atan2(bombs[i].vel.y, bombs[i].vel.x);
            phi = normalizeAngle(phi);
            double norm = std::sqrt(bombs[i].vel.x * bombs[i].vel.x + bombs[i].vel.y * bombs[i].vel.y);
            bombs[i].vel.x = norm * std::cos(phi);
            bombs[i].vel.y = norm * std::sin(phi);
        }
    }
}

//==========================================================
// BombType02 : 自機狙い 3way（薄い拡散）
//   - 一発ごとに「狙い＋左右」に3本
//   - 道中中型やボス通常に便利
//==========================================================
void EnemyShootScript::BombType02(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const int interval = 18;
    const double speed = 2.4 + difficulty * 0.5;
    const double spread = 0.12 + 0.02 * difficulty; // ラジアン

    if (time % interval == 0)
    {
        Vec2d p = player.getPosition();
        Vec2d e = enemy.getPosition();

        double base = std::atan2((p.y - e.y), (p.x - e.x));
        base = normalizeAngle(base);

        for (int k = -1; k <= 1; k++)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 9);
            bombs[idx].type = 2;

            double phi = base + k * spread;
            bombs[idx].vel.x = speed * std::cos(phi);
            bombs[idx].vel.y = speed * std::sin(phi);
        }
    }
}

//==========================================================
// BombType03 : スパイラル連射（回転弾流）
//   - 角度を time に応じて回し続ける
//   - 東方の「ぐるぐる」基本
//==========================================================
void EnemyShootScript::BombType03(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const int interval = 3;
    const double speed = 1.8 + difficulty * 0.35;
    const double w = 0.10 + 0.01 * difficulty; // 回転速度

    if (time % interval == 0)
    {
        int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 6);
        bombs[idx].type = 0;

        double phi = normalizeAngle(time * w);
        bombs[idx].vel.x = speed * std::cos(phi);
        bombs[idx].vel.y = speed * std::sin(phi);
    }
}

//==========================================================
// BombType04 : リング→ゆっくり回転（開幕圧）
//   - 一定間隔で円形リングを生成
//   - 生成された弾を毎フレ少し回転させる
//==========================================================
void EnemyShootScript::BombType04(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const int ringInterval = 90;
    const double speed = 1.2 + difficulty * 0.25;
    const int N = 10 + difficulty * 4;

    // リング生成
    if (time % ringInterval == 0)
    {
        for (int i = 0; i < N; i++)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 6);
            bombs[idx].type = 1;

            double phi = (2.0 * PI * i) / (double)N;
            bombs[idx].vel.x = speed * std::cos(phi);
            bombs[idx].vel.y = speed * std::sin(phi);
        }
    }

    // 回転付与（敵ID一致の弾だけ）
    const double rot = 0.004 + 0.001 * difficulty;
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing && bombs[i].id == enemy.getId())
        {
            double phi = std::atan2(bombs[i].vel.y, bombs[i].vel.x);
            double norm = std::sqrt(bombs[i].vel.x * bombs[i].vel.x + bombs[i].vel.y * bombs[i].vel.y);

            phi = normalizeAngle(phi + rot);
            bombs[i].vel.x = norm * std::cos(phi);
            bombs[i].vel.y = norm * std::sin(phi);
        }
    }
}

//==========================================================
// BombType05 : 扇状ウェーブ（左右に揺れるカーテン）
//   - 狙いではなく「扇＋位相」を付けて避けさせる
//   - 道中に置くと東方っぽい密度が出る
//==========================================================
void EnemyShootScript::BombType05(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const int interval = 10;
    const int N = 5 + difficulty * 2;
    const double baseSpeed = 1.8 + difficulty * 0.3;

    // 扇の中心角（真下基準）
    const double center = PI * 0.5;
    const double fan = 0.55; // 扇の広がり

    if (time % interval == 0)
    {
        // ちょっと揺れる中心（ウェーブ感）
        double wobble = std::sin(time * 0.06) * 0.18;

        for (int i = 0; i < N; i++)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 6);
            bombs[idx].type = (i % 2);

            double t = (N == 1) ? 0.0 : (double)i / (double)(N - 1);
            double phi = center + wobble + (t - 0.5) * fan;

            double sp = baseSpeed + 0.05 * i;
            bombs[idx].vel.x = sp * std::cos(phi);
            bombs[idx].vel.y = sp * std::sin(phi);
        }
    }
}

//==========================================================
// BombType06 : 停止→再加速（詐欺弾）
//   - 最初ゆっくり、一定時間後に一気に加速
//   - UpdateBombs は vel しか使わないので、ここで vel を増やす
//==========================================================
void EnemyShootScript::BombType06(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const int interval = 12;
    const double speed0 = 0.8 + difficulty * 0.15;
    const double accel = 1.025 + 0.003 * difficulty; // 毎フレの倍率
    const int trigger = 45;                          // このtimeを超えたら加速開始

    if (time % interval == 0)
    {
        Vec2d p = player.getPosition();
        Vec2d e = enemy.getPosition();

        double phi = std::atan2((p.y - e.y), (p.x - e.x));
        phi = normalizeAngle(phi);

        int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 7);
        bombs[idx].type = 2;

        bombs[idx].vel.x = speed0 * std::cos(phi);
        bombs[idx].vel.y = speed0 * std::sin(phi);
    }

    // 弾ごとに「生成後の時間」を見て加速
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing && bombs[i].id == enemy.getId())
        {
            if (bombs[i].time > trigger)
            {
                bombs[i].vel.x *= accel;
                bombs[i].vel.y *= accel;
            }
        }
    }
}

//==========================================================
// BombType07 : 交互回転スパイラル（二重螺旋）
//   - 偶数フレームと奇数フレームで回転方向を変える
//   - 2本の螺旋が絡む東方感
//==========================================================
void EnemyShootScript::BombType07(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const int interval = 4;
    const double speed = 1.6 + difficulty * 0.25;
    const double w = (0.12 + 0.01 * difficulty);

    if (time % interval == 0)
    {
        // 2発（逆回転の起点）
        for (int k = 0; k < 2; k++)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 6);
            bombs[idx].type = k; // 0/1

            double dir = (k == 0) ? 1.0 : -1.0;
            double phi = normalizeAngle(time * w * dir + k * PI);

            bombs[idx].vel.x = speed * std::cos(phi);
            bombs[idx].vel.y = speed * std::sin(phi);
        }
    }
}

//==========================================================
// BombType08 : ジグザグ弾（蛇行）
//   - 直進している弾の vel.x を周期的に反転させて蛇行させる
//   - 見た目が“妖精っぽい嫌らしさ”になる
//==========================================================
void EnemyShootScript::BombType08(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const int interval = 10;
    const double speed = 2.0 + difficulty * 0.35;

    if (time % interval == 0)
    {
        // 真下〜やや斜めの範囲で生成
        double phi = PI * 0.5 + std::sin(time * 0.07) * 0.25;

        int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 7);
        bombs[idx].type = 1;
        bombs[idx].vel.x = speed * std::cos(phi);
        bombs[idx].vel.y = speed * std::sin(phi);
    }

    // 蛇行（一定周期でX反転）
    const int zigInterval = 18;
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing && bombs[i].id == enemy.getId())
        {
            if (bombs[i].time > 0 && (bombs[i].time % zigInterval) == 0)
            {
                bombs[i].vel.x *= -1.0;
            }
        }
    }
}

//==========================================================
// BombType09 : ばら撒き＋狙い混合（通常弾の王道）
//   - 「ばら撒き(ランダム扇)」に「薄い狙い」を混ぜる
//   - 避けルートを固定させない東方らしさ
//==========================================================
void EnemyShootScript::BombType09(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const int interval = 10;
    const double speed = 1.9 + difficulty * 0.35;

    if (time % interval == 0)
    {
        Vec2d p = player.getPosition();
        Vec2d e = enemy.getPosition();

        // 狙い
        double aim = std::atan2((p.y - e.y), (p.x - e.x));
        aim = normalizeAngle(aim);

        // ばら撒き中心（真下）＋少し揺らし
        double center = PI * 0.5 + std::sin(time * 0.05) * 0.15;

        // ばら撒き2発
        for (int k = 0; k < 2; k++)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 6);
            bombs[idx].type = 0;

            double phi = center + (k == 0 ? -0.35 : 0.35);
            bombs[idx].vel.x = speed * std::cos(phi);
            bombs[idx].vel.y = speed * std::sin(phi);
        }

        // 薄い狙い1発（低密度）
        if ((time / interval) % 3 == 0)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 8);
            bombs[idx].type = 2;
            bombs[idx].vel.x = (speed + 0.6) * std::cos(aim);
            bombs[idx].vel.y = (speed + 0.6) * std::sin(aim);
        }
    }
}

//==========================================================
// BombType10 : 花弁リング（桜テーマ向け）
//   - 小リングを短間隔で重ねて「花弁の層」を作る
//   - 少しずつ回転位相をずらすと“桜っぽい”
//==========================================================
void EnemyShootScript::BombType10(Enemy enemy, BombManager bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    const int interval = 20;
    const double speed = 1.3 + difficulty * 0.22;
    const int N = 8 + difficulty * 3;

    if (time % interval == 0)
    {
        // 位相ずらし（リングが重なって花弁感が出る）
        double phase = (time / interval) * 0.18;

        for (int i = 0; i < N; i++)
        {
            int idx = InitBombShoot(enemy, bMgr, bombs, time, difficulty, player, 6);
            bombs[idx].type = (short)(i % 3); // 0/1/2で見た目を散らす

            double phi = (2.0 * PI * i) / (double)N + phase;
            bombs[idx].vel.x = speed * std::cos(phi);
            bombs[idx].vel.y = speed * std::sin(phi);
        }
    }

    // わずかに回転付与（花弁の“舞い”）
    const double rot = 0.0025 + 0.0006 * difficulty;
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing && bombs[i].id == enemy.getId())
        {
            double phi = std::atan2(bombs[i].vel.y, bombs[i].vel.x);
            double norm = std::sqrt(bombs[i].vel.x * bombs[i].vel.x + bombs[i].vel.y * bombs[i].vel.y);

            phi = normalizeAngle(phi + rot);
            bombs[i].vel.x = norm * std::cos(phi);
            bombs[i].vel.y = norm * std::sin(phi);
        }
    }
}
