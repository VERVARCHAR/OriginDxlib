#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_
#include "object/player.hpp"
#endif // _PLAYER_HPP_

Player::Player()
{
    init();
}

Player::~Player()
{
    ;
}

void Player::init()
{
    pos.x = 350;
    pos.y = 500;
    status.lives = 2;
    status.spells = 3;
    status.power = 1;
    status.isSpells = false;
    status.invincible = false;
    status.invincibleTime = 0;
    status.isShoot = false;
    status.isShift = false;
    status.grazeCount = 0;
    radius = 3;
    type = 0;
    std::string name_utf8 = "Default";
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring name_w = converter.from_bytes(name_utf8);
    name = name_w;
}

// TODO どっかで宣言してそう???
#define SQRT2 1.414

void Player::playerDraw()
{

    if (!status.isSpells && status.invincible && status.lives != 0)
    {
        if (status.invincibleTime % 10 == 0)
        {
            isDraw = !isDraw;
        }
    }

    if (isDraw)
    {
        DrawGraph(pos.x - 24, pos.y - 25, charaImageHandle[keyVector], TRUE);
        // DrawCircle(pos.x, pos.y - 5, radius, GetColor(0, 0, 0), FALSE);

        for (int i = 0; i < (int)(status.power); i++)
        {
            DrawRotaGraph(
                pos.x + (status.isShift ? -10 : -20) * (status.power - 1) + (status.isShift ? 20 : 40) * i, pos.y + 40, 1.0f, (3.14 / 180) * time * ((i % 2) + 1), charaImageHandle[3], TRUE);
        }

        if (status.isShift)
        {
            DrawGraph(pos.x - 32, pos.y - 32, charaShiftedEffect[0], TRUE);
            DrawRotaGraph(pos.x, pos.y, 1.0f, (3.14 / 180) * time, charaShiftedEffect[1], TRUE);
            DrawRotaGraph(pos.x, pos.y, 1.0f, -(3.14 / 180) * time, charaShiftedEffect[2], TRUE);
        }

        // [DEBUG]
        // DrawCircle(pos.x, pos.y - 5, radius * 8, GetColor(255, 255, 255), FALSE);
    }
}

void Player::playerUpdate(BombManager *bMgr, BombInfo bombs[MAX_BOMBS], Effecter *effecter, Enemy *enemies[MAX_ENEMIES])
{

    // DrawBox(pos.x - 10, pos.y - 10, pos.x + 10, pos.y + 10, GetColor(0, 0, 255), TRUE);
    // [DEBUG]
    // printfDx(L"characterHandle : %d\n", charaImageHandle[0]);

    if (status.isShoot)
    {
        shootBomb(bMgr, bombs);
        effecter->playSE_Shot();
    }

    if (!status.invincible)
    {
        for (int i = 0; i < MAX_BOMBS; i++)
        {
            if (bombs[i].isUsing && !bombs[i].isPlayers && isHitBomb(&bombs[i], pos, radius, &status.grazeCount, effecter))
            {
                Dead();
                effecter->playPlayerExplode(pos);
            }
        }
    }
    else
    {
        // ボム発動中
        if (status.isSpells)
        {
            SpelCard(bMgr, bombs);
            if (status.invincibleTime == 300)
            {
                effecter->playSpell(pos);
            }
        }

        if (status.invincibleTime == 0)
        {
            isDraw = true;
            status.invincible = false;
            if (status.isSpells)
            {
                status.isSpells = false;
            }
        }
        status.invincibleTime -= 1;
    }
    updateBombs(bMgr, bombs, enemies);
    chattering = false;
    time++;
}

Vec2d Player::getPosition()
{
    return pos;
}

// TODO プレイ画面外に出れないようにする
void Player::getKeyInput(bool isTalk)
{
    double moveSpeed = 0;
    if (Key[KEY_INPUT_LSHIFT] > 1)
    {
        status.isShift = true;
        moveSpeed = 1.5;
    }
    else
    {
        status.isShift = false;
        moveSpeed = 3.0;
    }

    keyVector = 0;
    if (Key[KEY_INPUT_UP] > 1)
    {
        pos.y -= moveSpeed;
        if (pos.y < 10)
        {
            pos.y = 10;
        }
    }
    if (Key[KEY_INPUT_DOWN] > 1)
    {
        pos.y += moveSpeed;
        if (pos.y > WINDOW_HEIGHT - 10)
        {
            pos.y = WINDOW_HEIGHT - 10;
        }
    }
    if (Key[KEY_INPUT_LEFT] > 1)
    {
        pos.x -= moveSpeed;
        keyVector = 1;
        if (pos.x < 10)
        {
            pos.x = 10;
        }
    }
    if (Key[KEY_INPUT_RIGHT] > 1)
    {
        pos.x += moveSpeed;
        keyVector = 2;
        if (pos.x > WINDOW_WIDTH * 0.6 + 22)
        {
            pos.x = WINDOW_WIDTH * 0.6 + 22;
        }
    }

    if (Key[KEY_INPUT_X] == 1)
    {
        if (status.spells > 0 && !status.invincible)
        {
            status.spells -= 1;
            status.isSpells = true;
            status.invincible = true;
            status.invincibleTime = 300;
        }
    }
    if (Key[KEY_INPUT_Z] > 0 && Key[KEY_INPUT_Z] % 5 == 0)
    {
        if (!isTalk)
        {
            // 無敵時間でない もしくは、無敵時間だがスペル使用時なら打てる
            if (!status.invincible || (status.invincible && status.isSpells))
            {
                status.isShoot = true;
            }
        }
    }
    else
    {
        status.isShoot = false;
    }
}

void Player::Dead()
{
    if (!chattering)
    {
        status.lives -= 1;
        status.invincible = true;
        status.invincibleTime = 3 * 60;
        chattering = true;
    }
}

void Player::shootBomb(BombManager *bMgr, BombInfo bombs[MAX_BOMBS])
{
    double speed = 12;

    for (int i = 0; i < (int)(status.power); i++)
    {
        int idx = bMgr->getEmptyIndex(bombs);
        if (idx < 0)
        {
            // 弾がこれ以上出せないときの対処（何もしない・ログを出すなど）
            printfDx(L"[WARN] player bullet overflow\n");
        }
        bombs[idx].isUsing = true;
        bombs[idx].time = 0;
        bombs[idx].radius = 5;
        bombs[idx].pos = pos;
        bombs[idx].pos.x += (status.isShift ? -10 : -20) * ((int)status.power - 1) + (status.isShift ? 20 : 40) * i;
        bombs[idx].pos.y += 20;

        bombs[idx].isPlayers = true;
        bombs[idx].type = 2;

        bombs[idx].vel.x = 0.0;
        bombs[idx].vel.y = -speed;
    }
}

static double clampd(double v, double lo, double hi)
{
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}

static void normalize(double &x, double &y)
{
    const double l2 = x * x + y * y;
    if (l2 < 1e-12)
    {
        x = 0.0;
        y = -1.0; // 上
        return;
    }
    const double inv = 1.0 / sqrt(l2);
    x *= inv;
    y *= inv;
}

void Player::updateBombs(BombManager *bMgr, BombInfo bombs[MAX_BOMBS], Enemy *enemies[MAX_ENEMIES])
{

    // ---- 調整パラメータ（東方っぽく）----
    const double speed = 12;    // 弾速（発射側と一致推奨）
    const double detectR = 500; // この距離以内の敵に反応
    const double detectR2 = detectR * detectR;

    const double turn = 0.16;       // 1フレームで曲げる強さ(0.08〜0.16おすすめ)
    const double keepSpeed = speed; // 速度一定にする

    for (int i = 0; i < MAX_BOMBS; ++i)
    {
        BombInfo &b = bombs[i];
        if (!b.isUsing)
            continue;
        if (!b.isPlayers)
            continue;
        // if (b.type != 2)
        //     continue; // プレイヤー通常弾だけホーミング

        // 1) 近い敵を探す
        Enemy *target = nullptr;
        double bestD2 = detectR2;

        for (int e = 0; e < MAX_ENEMIES; ++e)
        {
            Enemy *en = enemies[e];
            if (!en)
                continue;

            // 例: if (!en->isActive()) continue;
            if (!en->getIsAlive())
                continue;

            if (en->enemyStatus.isInvincible)
                continue;

            const Vec2d epos = en->getPosition();
            const double dx = epos.x - b.pos.x;
            const double dy = epos.y - b.pos.y;
            const double d2 = dx * dx + dy * dy;

            if (d2 < bestD2)
            {
                bestD2 = d2;
                target = en;
            }
        }

        // 2) ターゲットがいるなら、進行方向を少しだけそっちへ曲げる
        if (target)
        {
            // 目標方向
            const Vec2d tpos = target->getPosition();
            double tx = tpos.x - b.pos.x;
            double ty = tpos.y - b.pos.y;
            normalize(tx, ty);

            // 現在方向
            double vx = b.vel.x;
            double vy = b.vel.y;
            normalize(vx, vy);

            // 方向補間（急旋回しないホーミング）
            const double t = clampd(turn, 0.0, 1.0);
            double nx = vx * (1.0 - t) + tx * t;
            double ny = vy * (1.0 - t) + ty * t;
            normalize(nx, ny);

            // 速度一定化して反映
            b.vel.x = nx * keepSpeed;
            b.vel.y = ny * keepSpeed;
        }
    }
}

void Player::SpelCard(BombManager *bMgr, BombInfo bombs[MAX_BOMBS])
{
    DrawCircle(pos.x, pos.y, 250, GetColor(128, 128, 128), FALSE);
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (!bombs[i].isPlayers && POWER2((pos.x) - (bombs[i].pos.x)) + POWER2((pos.y) - (bombs[i].pos.y)) < POWER2(250))
        {
            bMgr->initBomb(&bombs[i]);
        }
    }
}

void Player::loadPlayerImage()
{
    // TODO スプライトシートに変えたい
    // charaImageHandle[0] = LoadGraph(L"..\\..\\assets\\player\\player.png");
    LoadDivGraph(L"..\\..\\assets\\player\\player.png", 3, 1, 3, 48, 50, charaImageHandle);
    charaImageHandle[3] = LoadGraph(L"..\\..\\assets\\player\\player_items.png");
    // LoadDivGraph(L"../../img/reimu_img.jpg", 18, 6, 3, 32, 48, charaImageHandle);
    LoadDivGraph(L"..\\..\\assets\\player\\effect_player_shifted.png", 3, 3, 1, 64, 64, charaShiftedEffect);
}