#ifndef _ENEMY_HPP_
#define _ENEMY_HPP_
#include "object/enemy.hpp"
#endif // _ENEMY_HPP_

Enemy::Enemy(EnemyStatus _enemyStatus)
{
    init(_enemyStatus);
}

Enemy::~Enemy()
{
    // if();
}

void Enemy::init(EnemyStatus _enemyStatus)
{
    enemyStatus = _enemyStatus;
}

// TODO どこかで定義されてるはず
#define SQRT2 1.414
void Enemy::enemyDraw()
{
    if (imageHandle < 0)
    {
        DrawBox(enemyStatus.pos.x - enemyStatus.radius * SQRT2, enemyStatus.pos.y - enemyStatus.radius * SQRT2, enemyStatus.pos.x + enemyStatus.radius * SQRT2, enemyStatus.pos.y + enemyStatus.radius * SQRT2, GetColor(0, 25, 255), TRUE);
    }
    else
    {
        // 敵の状態更新ロジックをここに実装
        DrawExtendGraph(enemyStatus.pos.x - enemyStatus.radius * SQRT2, enemyStatus.pos.y - enemyStatus.radius * SQRT2, enemyStatus.pos.x + enemyStatus.radius * SQRT2, enemyStatus.pos.y + enemyStatus.radius * SQRT2, imageHandle, TRUE);
    }
    printfDx(L"enemy image handle %d\n", imageHandle);
    // DrawBox(enemyStatus.pos.x - 10, enemyStatus.pos.y - 10, enemyStatus.pos.x + 10, enemyStatus.pos.y + 10, GetColor(255, 0, 0), TRUE);
    // DrawCircle(enemyStatus.pos.x, enemyStatus.pos.y, enemyStatus.radius, GetColor(255, 0, 255), TRUE);

    if (enemyStatus.type >= 100)
    {
        printfDx(L"EnemyLives:%d\n", enemyStatus.lives);
        printfDx(L"EnemyHP:%d\n", enemyStatus.hp);
        printfDx(L"EnemiesSpell:%d\n", enemyStatus.isSpell);
        printfDx(L"SpellCount:%d\n", enemyStatus.spellCount);
        printfDx(L"EnemyTime:%d\n", enemyStatus.time);
    }
}

void Enemy::enemyUpdate(int time, int difficulty, BombManager *bMgr, BombInfo bombs[MAX_BOMBS], EnemyShootScript *enemyShootScript, Player *player, Effecter *effecter, ItemManager *iMgr)
{
    if (!enemyStatus.isAlive)
    {
        return;
    }
    else
    {
        if ((!player->getStatus().invincible) && isHitPlayer(player))
        {
            player->Dead();
        }
        if (enemyStatus.isSpell)
        {
            // DrawFormatString(100, 40, GetColor(255, 255, 255), L"Spell");
            // TODO effect
            switch (enemyStatus.shootType)
            {
            case 1:
                enemyShootScript->Boss01Spell01(*this, *bMgr, bombs, time, difficulty, *player);
                /* code */
                break;
            case 2:
                enemyShootScript->Boss01Spell02(*this, *bMgr, bombs, time, difficulty, *player);

                break;
            case 3:
                enemyShootScript->Boss01Spell03(*this, *bMgr, bombs, time, difficulty, *player);

            default:
                break;
            }
        }
        else
        {
            shootBomb(enemyShootScript, bMgr, bombs, enemyStatus.time, difficulty, *player);
        }

        // TODO 敵の動きも関数にしたいかなぁ
        enemyMove();
        enemyStatus.time++;

        for (int i = 0; i < MAX_BOMBS; i++)
        {
            if (bombs[i].isUsing && bombs[i].isPlayers && !enemyStatus.isInvincible && isHitBomb(&bombs[i], enemyStatus.pos, enemyStatus.radius))
            {
                enemyStatus.hp--;
            }
        }

        // 敵のHPが0になった時の処理
        if (enemyStatus.hp <= 0 && enemyStatus.lives > 0)
        {
            enemyStatus.lives -= 1;
            enemyStatus.shootType += 1;

            // もし敵がボスで，HPが0になったらスペルフラグをfalseにし，無敵時間を付与
            if (enemyStatus.isSpell == true)
            {
                enemyStatus.isSpell = false;
                enemyStatus.isInvincible = true;
                enemyStatus.invincibleTime = 120;
                enemyStatus.hp = enemyStatus.maxHp;
                enemyStatus.spellCount += 1;
            }
        }

        // 敵のHPが0になるか，画面外に出た場合の処理
        if (enemyStatus.lives == 0 || !getOnScreen())
        {
            enemyStatus.isAlive = false;
            // iMgr->spawnItem(ItemType::POWER, enemyStatus.pos, {0, 1});
            if (enemyStatus.lives == 0)
            {
                effecter->playEnemyExplode(enemyStatus.pos);
                iMgr->spawnItem(ItemType::SCORE, enemyStatus.pos, {0, 1});
            }
        }

        // 敵のtypeが100以上のとき，敵はボス
        if (enemyStatus.type >= 100)
        {
            // ボスのHPが100を下回ったらスペル発動
            if (enemyStatus.hp <= 100 && !enemyStatus.isSpell)
            {
                enemyStatus.isSpell = true;
                enemyStatus.time = 0;
                // enemyStatus.spellStartTime = time;
            }
        }

        // 無敵時間の処理
        if (enemyStatus.invincibleTime >= 0)
        {
            enemyStatus.invincibleTime -= 1;
        }
        else
        {
            enemyStatus.isInvincible = false;
        }
    }
}

Vec2d Enemy::getPosition()
{
    return enemyStatus.pos;
}

void Enemy::getBMgrData(BombManager &_BombManager)
{
}

bool Enemy::getOnScreen()
{
    // TODO 画面サイズ取得に変更
    int screenWidth = 1000;
    int screenHeight = 600;
    // int hideScreen
    if (enemyStatus.pos.x + enemyStatus.radius < -50 || enemyStatus.pos.y - enemyStatus.radius > screenWidth + 50 || enemyStatus.pos.y + enemyStatus.radius < -50 || enemyStatus.pos.y - enemyStatus.radius > screenHeight + 50)
    {
        return false;
    }
    return true;
}

void Enemy::shootBomb(EnemyShootScript *enemyShootScript, BombManager *bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{

    switch (enemyStatus.shootType)
    {
    case 0:
        enemyShootScript->BombType00(*this, *bMgr, bombs, time, difficulty, player);
        break;

    case 1:
        enemyShootScript->BombType01(*this, *bMgr, bombs, time, difficulty, player);
        break;
    default:

        printfDx(L"[DEBUG]:shootBomb %d\n", enemyStatus.shootType);
        break;
    }
}

bool Enemy::isHitPlayer(Player *player)
{
    Vec2d playerPos = player->getPosition();
    int playerRadius = player->getRadius();

    if (enemyStatus.isAlive)
    {
        if (POWER2(((playerPos.x) - (enemyStatus.pos.x))) + POWER2(((playerPos.y) - (enemyStatus.pos.y - playerRadius))) < POWER2(((playerRadius) + (enemyStatus.radius))))
        {
            return true;
        }
    }
    return false;
}

void Enemy::enemyMove()
{
    switch (enemyStatus.type)
    {
    case 100:
        if (enemyStatus.time == 60)
        {
            enemyStatus.vel = {0, 0};
        }
    default:

        printfDx(L"[DEBUG]:shootBomb %d\n", enemyStatus.shootType);
        break;
    }
    enemyStatus.pos.x += enemyStatus.vel.x;
    enemyStatus.pos.y += enemyStatus.vel.y;
}