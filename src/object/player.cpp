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
        DrawExtendGraph(pos.x - 30, pos.y - 30, pos.x + 30, pos.y + 30, charaImageHandle[0], TRUE);
        DrawCircle(pos.x, pos.y - 5, radius, GetColor(255, 255, 255), TRUE);

        // [DEBUG]
        DrawCircle(pos.x, pos.y - 5, radius * 8, GetColor(255, 255, 255), FALSE);
    }
}

void Player::playerUpdate(BombManager *bMgr, BombInfo bombs[MAX_BOMBS], Effecter *effecter)
{

    // DrawBox(pos.x - 10, pos.y - 10, pos.x + 10, pos.y + 10, GetColor(0, 0, 255), TRUE);
    printfDx(L"characterHandle : %d\n", charaImageHandle[0]);

    if (status.isShoot)
    {
        shootBomb(bMgr, bombs);
        effecter->playSE_Shot();
    }

    if (!status.invincible)
    {
        for (int i = 0; i < MAX_BOMBS; i++)
        {
            if (bombs[i].isUsing && !bombs[i].isPlayers && isHitBomb(&bombs[i], pos, radius, &status.grazeCount))
            {
                Dead();
                effecter->playPlayerExplode(pos);
            }
        }
    }
    else
    {
        status.invincibleTime -= 1;

        // ボム発動中
        if (status.isSpells)
        {
            SpelCard(bMgr, bombs);
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
    }
    chattering = false;
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

        // TODO effect
        moveSpeed = 1.5;
    }
    else
    {
        status.isShift = false;
        moveSpeed = 3.0;
    }

    if (Key[KEY_INPUT_UP] > 1)
    {
        pos.y -= moveSpeed;
    }
    if (Key[KEY_INPUT_DOWN] > 1)
    {
        pos.y += moveSpeed;
    }
    if (Key[KEY_INPUT_LEFT] > 1)
    {
        pos.x -= moveSpeed;
    }
    if (Key[KEY_INPUT_RIGHT] > 1)
    {
        pos.x += moveSpeed;
    }

    if (Key[KEY_INPUT_X] == 1)
    {
        if (status.spells > 0)
        {
            status.spells -= 1;
            status.isSpells = true;
            status.invincible = true;
            status.invincibleTime = 5 * 60;
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
    double speed = 15;

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
        bombs[idx].pos.x += (status.isShift ? -10 : -20) * (status.power - 1) + (status.isShift ? 20 : 40) * i;
        bombs[idx].pos.y += 20;
        bombs[idx].isPlayers = true;
        bombs[idx].type = 2;

        bombs[idx].vel.y = -speed;
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
    charaImageHandle[0] = LoadGraph(L"..\\..\\assets\\player\\player.png");
    // LoadDivGraph(L"../../img/reimu_img.jpg", 18, 6, 3, 32, 48, charaImageHandle);
}