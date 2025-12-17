#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_HPP_

#ifndef _UTILS_HPP_
#define _UTILS_HPP_
#include "system/utils.hpp"
#endif

BombManager::BombManager(BombInfo bombs[MAX_BOMBS])
{
    init(bombs);
}
BombManager::~BombManager()
{
    ;
}

void BombManager::init(BombInfo bombs[MAX_BOMBS])
{
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        initBomb(&bombs[i]);
    }
}

int BombManager::getEmptyIndex(BombInfo bombs[MAX_BOMBS])
{
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (!bombs[i].isUsing)
        {
            return i;
        }
    }
    return -1;
}

bool BombManager::getOnScreen(BombInfo bomb)
{
    // TODO 画面サイズ取得に変更
    int screenWidth = (WINDOW_WIDTH * 0.6) + 22;
    int screenHeight = WINDOW_HEIGHT - 10;
    if (bomb.pos.x + bomb.radius < 10 || bomb.pos.x - bomb.radius > screenWidth || bomb.pos.y + bomb.radius < 10 || bomb.pos.y - bomb.radius > screenHeight)
    {
        return false;
    }
    return true;
}

#define SQRT2 1.414

void BombManager::drawBombs(BombInfo bombs[MAX_BOMBS])
{
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing)
        {
            switch (bombs[i].type)
            {
            case 0:
                // DrawCircle(bombs[i].pos.x, bombs[i].pos.y, bombs[i].radius, GetColor(255, 255, 0), TRUE);
                DrawExtendGraph(bombs[i].pos.x - bombs[i].radius * SQRT2, bombs[i].pos.y - bombs[i].radius * SQRT2, bombs[i].pos.x + bombs[i].radius * SQRT2, bombs[i].pos.y + bombs[i].radius * SQRT2, bombsHandle[0], TRUE);
                // DrawCircle(bombs[i].pos.x, bombs[i].pos.y, bombs[i].radius, GetColor(255, 255, 0), TRUE);
                break;
            case 1:
                DrawExtendGraph(bombs[i].pos.x - bombs[i].radius * SQRT2, bombs[i].pos.y - bombs[i].radius * SQRT2, bombs[i].pos.x + bombs[i].radius * SQRT2, bombs[i].pos.y + bombs[i].radius * SQRT2, bombsHandle[1], TRUE);
                // DrawCircle(bombs[i].pos.x, bombs[i].pos.y, bombs[i].radius, GetColor(255, 255, 0), TRUE);
                break;
            case 2:
                DrawExtendGraph(bombs[i].pos.x - bombs[i].radius * SQRT2, bombs[i].pos.y - bombs[i].radius * SQRT2, bombs[i].pos.x + bombs[i].radius * SQRT2, bombs[i].pos.y + bombs[i].radius * SQRT2, bombsHandle[2], TRUE);
                // DrawCircle(bombs[i].pos.x, bombs[i].pos.y, bombs[i].radius, GetColor(255, 255, 0), TRUE);
                break;
            default:
                break;
            }
        }
    }
}

void BombManager::DEBUG_printAllBombs(BombInfo bombs[MAX_BOMBS])
{
    int cnt = 0;
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing)
        {
            // printfDx(L"Bomb %d: Active", i);
            cnt++;
        }
    }
    DrawFormatString(600, 600, GetColor(255, 255, 255), L"Bombs cnt %d", cnt);
}

void BombManager::updateBombs(BombInfo bombs[MAX_BOMBS])
{
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing)
        {
            bombs[i].pos.x += bombs[i].vel.x;
            bombs[i].pos.y += bombs[i].vel.y;
            bombs[i].time += 1;
            if (!getOnScreen(bombs[i]))
            {
                initBomb(&bombs[i]);
            }
        }
    }
}

void BombManager::initBomb(BombInfo *bomb)
{
    bomb->isUsing = false;
    bomb->isPlayers = false;
    bomb->isGraze = false;
    bomb->pos.x = -1000;
    bomb->pos.y = -1000;
    bomb->vel.x = 0;

    bomb->vel.y = 0;
    bomb->radius = -1;
    bomb->power = 1;
    bomb->time = -1;
    bomb->id = -1;
    bomb->type = -1;
    bomb->isVanishing = false;
    bomb->vanishTime = 0;
}

void BombManager::setBombsHandle(int _bombsHandle[16])
{
    for (int i = 0; i < 16; i++)
    {
        bombsHandle[i] = _bombsHandle[i];
    }
}

bool isHitBomb(BombInfo *bomb, Vec2d pos, int radius)
{

    if (bomb->isUsing)
    {
        if (POWER2(((bomb->pos.x) - (pos.x))) + POWER2(((bomb->pos.y) - (pos.y))) < POWER2(((bomb->radius) + (radius))))
        {
            bomb->isUsing = false;
            return true;
        }
    }

    return false;
}

bool isHitBomb(BombInfo *bomb, Vec2d pos, int radius, int *grazeCount, Effecter *effecter)
{

    if (bomb->isUsing)
    {
        if (POWER2(((bomb->pos.x) - (pos.x))) + POWER2(((bomb->pos.y) - (pos.y))) < POWER2(((bomb->radius) + (radius))))
        {
            bomb->isUsing = false;
            return true;
        }
        if (!bomb->isGraze && POWER2(((bomb->pos.x) - (pos.x))) + POWER2(((bomb->pos.y) - (pos.y))) < POWER2(((bomb->radius) + (radius * 8))))
        {
            *grazeCount += 1;
            bomb->isGraze = true;
            effecter->playGraze(pos);
        }
    }

    return false;
}

void BombManager::removeBomb(BombInfo bombs[MAX_BOMBS], Effecter *effecter)
{
    // effecter->PlaySE_BombVanish();
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing && !bombs[i].isPlayers)
        {
            effecter->playBulletVanish(bombs[i].pos);
            initBomb(&bombs[i]);
        }
    }
}