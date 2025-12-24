#ifndef _ENEMY_HPP_
#define _ENEMY_HPP_
#include "object/enemy.hpp"
#endif // _ENEMY_HPP_

#include <cmath>

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
    // printfDx(L"enemy image handle %d\n", imageHandle);
    // DrawBox(enemyStatus.pos.x - 10, enemyStatus.pos.y - 10, enemyStatus.pos.x + 10, enemyStatus.pos.y + 10, GetColor(255, 0, 0), TRUE);
    // DrawCircle(enemyStatus.pos.x, enemyStatus.pos.y, enemyStatus.radius, GetColor(255, 0, 255), TRUE);

    // [DEBUG]
    // if (enemyStatus.type >= 100)
    // {
    //     printfDx(L"EnemyLives:%d\n", enemyStatus.lives);
    //     printfDx(L"EnemyHP:%d\n", enemyStatus.hp);
    //     printfDx(L"EnemiesSpell:%d\n", enemyStatus.isSpell);
    //     printfDx(L"SpellCount:%d\n", enemyStatus.spellCount);
    //     printfDx(L"EnemyTime:%d\n", enemyStatus.time);
    // }
}

void Enemy::enemyUpdate(int time, StageInfo *stageInfo, BombManager *bMgr, BombInfo bombs[MAX_BOMBS], EnemyShootScript *enemyShootScript, Player *player, Effecter *effecter, ItemManager *iMgr)
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

        // TODO 敵の動きも関数にしたいかなぁ
        enemyMove(player->getPosition());
        enemyStatus.time++;

        for (int i = 0; i < MAX_BOMBS; i++)
        {
            if (bombs[i].isUsing && bombs[i].isPlayers && !enemyStatus.isInvincible && isHitBomb(&bombs[i], enemyStatus.pos, enemyStatus.radius))
            {
                enemyStatus.hp--;
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

                iMgr->spawnItem(ItemType::SCORE, enemyStatus.pos, {0, 2}, stageInfo->difficulty + 10, time);
                iMgr->spawnItem(ItemType::POWER, enemyStatus.pos, {0, 2}, stageInfo->difficulty + 10, time);
            }
        }
    }

    // 敵のtypeが100以上のとき，敵はボス
    if (enemyStatus.type >= 100)
    {
        // ボスのHPが100を下回ったらスペル発動
        if (enemyStatus.hp <= 100 && !enemyStatus.isSpell)
        {
            enemyStatus.isSpell = true;
            enemyStatus.time = -120;
            bMgr->removeBomb(bombs, effecter);
            enemyStatus.isInvincible = true;
            enemyStatus.invincibleTime = 120;
        }
    }

    // 敵のHPが0になった時の処理
    if (enemyStatus.hp <= 0 && enemyStatus.lives > 0)
    {
        enemyStatus.lives -= 1;
        enemyStatus.shootType += 1;
        stageInfo->score += 1000;

        // もし敵がボスで，HPが0になったらスペルフラグをfalseにし，無敵時間を付与
        if (enemyStatus.isSpell == true)
        {
            enemyStatus.isSpell = false;
            enemyStatus.isInvincible = true;
            enemyStatus.invincibleTime = 120;
            enemyStatus.hp = enemyStatus.maxHp;
            enemyStatus.spellCount += 1;
            bMgr->removeBomb(bombs, effecter);
            stageInfo->score += 100000;
            enemyStatus.time = 0;

            if (enemyStatus.shootType > 10)
            {
                enemyStatus.shootType = 1;
            }
        }
        if (enemyStatus.type >= 100 && 1 == enemyStatus.lives)
        {
            enemyStatus.isInvincible = true;
            enemyStatus.invincibleTime = 180;
            enemyStatus.isSpell = true;
            enemyStatus.time = -180;
        }
    }

    if (enemyStatus.isSpell && enemyStatus.invincibleTime == 120)
    {
        enemyStatus.isSpell = true;
        Vec2d cutIn = {790, 100};
        effecter->playEnemySpell(cutIn);
    }

    if (!enemyStatus.isInvincible)
    {
        if (enemyStatus.isSpell)
        {
            // DrawFormatString(100, 40, GetColor(255, 255, 255), L"Spell");

            shootSpell(enemyShootScript, bMgr, bombs, enemyStatus.time, stageInfo->difficulty, *player);
        }
        else
        {
            shootBomb(enemyShootScript, bMgr, bombs, enemyStatus.time, stageInfo->difficulty, *player);
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
        // case 0:
        //     enemyShootScript->BombType00(*this, *bMgr, bombs, time, difficulty, player);
        //     break;

    case 1:
        enemyShootScript->BombType01(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 2:
        enemyShootScript->BombType02(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 3:
        enemyShootScript->BombType03(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 4:
        enemyShootScript->BombType04(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 5:
        enemyShootScript->BombType05(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 6:
        enemyShootScript->BombType06(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 7:
        enemyShootScript->BombType07(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 8:
        enemyShootScript->BombType08(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 9:
        enemyShootScript->BombType09(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 10:
        enemyShootScript->BombType10(*this, *bMgr, bombs, time, difficulty, player);
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

#include <cmath>
// 他の include は既存に合わせて
void Enemy::enemyMove(Vec2d playerPos)
{
    // =========================================================
    // moveType 決定（互換性: moveType==0 なら旧typeを使用）
    // =========================================================
    int mvType = enemyStatus.moveType;
    if (mvType == 0)
    {
        mvType = enemyStatus.type;
    }

    // =========================================================
    // 初期化（spawn直後の1回だけ）
    //   - enemyIO 側で basePos=centerPos=pos に初期化済み前提
    //   - このブロックでは「必要なmoveTypeだけ」基準座標を上書きする
    // =========================================================
    if (enemyStatus.time == 0 && enemyStatus.moveInit == 0)
    {
        switch (mvType)
        {
        case 4:
        {
            // moveType=4 は targetY を centerPos.y に保持して使う
            const double targetY = 120.0;
            enemyStatus.basePos = enemyStatus.pos;
            enemyStatus.centerPos = {enemyStatus.pos.x, targetY};
        }
        break;

        case 7:
        {
            // moveType=7 は円運動中心を centerPos に保持して使う
            const double centerYOffset = 80.0;
            enemyStatus.basePos = enemyStatus.pos;
            enemyStatus.centerPos = {enemyStatus.pos.x, enemyStatus.pos.y + centerYOffset};
        }
        break;

        default:
            // その他は enemyIO 初期値（basePos=centerPos=pos）をそのまま使用
            break;
        }

        enemyStatus.moveInit = 1;
    }

    switch (mvType)
    {
    // =========================================================
    // moveType = 1 : 上から降りてきて止まる
    //   - 出現直後は一定速度で下へ
    //   - ある Y 座標まで来たら停止
    // =========================================================
    case 1:
    {
        const double speed = 2.5;
        const double stopY = 120.0;

        if (enemyStatus.time == 0)
        {
            enemyStatus.vel = {0.0, speed};
        }

        if (enemyStatus.pos.y >= stopY)
        {
            enemyStatus.vel = {0.0, 0.0};
        }
    }
    break;

    // =========================================================
    // moveType = 2 : 出現時にプレイヤーを狙って一直線
    //   - 出現したフレームで playerPos を見て方向決定
    //   - その後は速度一定で直進
    // =========================================================
    case 2:
    {
        const double speed = 3.5;

        if (enemyStatus.time == 0)
        {
            Vec2d diff{playerPos.x - enemyStatus.pos.x, playerPos.y - enemyStatus.pos.y};
            double len = std::sqrt(diff.x * diff.x + diff.y * diff.y);

            if (len > 1e-6)
            {
                diff.x /= len;
                diff.y /= len;
            }
            else
            {
                diff = {0.0, 1.0};
            }

            enemyStatus.vel = {diff.x * speed, diff.y * speed};
        }
    }
    break;

    // =========================================================
    // moveType = 3 : ゆっくりホーミング
    //   - 毎フレーム playerPos 方向へ少しずつ向きを曲げる
    //   - vel を線形補間して「ヌルい追尾感」を作る
    // =========================================================
    case 3:
    {
        const double targetSpeed = 2.5;
        const double turnRate = 0.05;

        if (enemyStatus.time == 0)
        {
            enemyStatus.vel = {0.0, targetSpeed};
        }

        Vec2d diff{playerPos.x - enemyStatus.pos.x, playerPos.y - enemyStatus.pos.y};
        double len = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        if (len > 1e-6)
        {
            diff.x /= len;
            diff.y /= len;
        }
        else
        {
            diff = {0.0, 0.0};
        }

        Vec2d targetVel{diff.x * targetSpeed, diff.y * targetSpeed};

        enemyStatus.vel.x = (1.0 - turnRate) * enemyStatus.vel.x + turnRate * targetVel.x;
        enemyStatus.vel.y = (1.0 - turnRate) * enemyStatus.vel.y + turnRate * targetVel.y;
    }
    break;

    // =========================================================
    // moveType = 4 : 画面上部で「高速移動→停止」を繰り返す（ボス用）
    //   - targetY(centerPos.y) まで降りて停止
    //   - 以降：高速で左右へ移動 → 2〜3秒停止 を繰り返す
    // =========================================================
    case 4:
    {
        // 1) 下降フェーズ
        const double downSpeed = 2.5;
        const double targetY = enemyStatus.centerPos.y; // 初期化済み想定

        // 2) 横移動の仕様（小さめの移動幅）
        const double dashSpeed = 6.0;       // 素早く移動
        const double moveRange = 120.0;     // 中央から左右の振り幅（小さめ）
        const double centerX = 640.0 * 0.5; // 画面幅に合わせて
        const double leftX = centerX - moveRange;
        const double rightX = centerX + moveRange;

        // 3) 停止時間（2〜3秒程度）
        //    difficulty等で揺らしたいならここを変える
        const int stopMinF = 180; // 2秒 @60fps
        const int stopMaxF = 240; // 3秒 @60fps

        // 4) 内部状態（敵ごとに保持したいので static で簡易に）
        //    ※厳密には EnemyStatus に state/timer/dir を追加するのがベスト
        static int state = 0;     // 0:下降, 1:ダッシュ, 2:停止
        static int stopTimer = 0; // 停止残りフレーム
        static int dir = 1;       // +1:右へ, -1:左へ

        // 初回初期化
        if (enemyStatus.time == 0)
        {
            state = 0;
            stopTimer = 0;
            dir = 1;
            enemyStatus.vel = {0.0, downSpeed};
        }

        // targetYに到達したら横移動ループへ
        if (state == 0)
        {
            if (enemyStatus.pos.y >= targetY)
            {
                enemyStatus.pos.y = targetY;
                enemyStatus.vel = {0.0, 0.0};

                state = 1; // ダッシュ開始
            }
            break;
        }

        // 以降は y 固定
        enemyStatus.pos.y = targetY;
        enemyStatus.vel.y = 0.0;

        // 5) ダッシュ（高速移動）
        if (state == 1)
        {
            enemyStatus.vel.x = dashSpeed * (double)dir;

            // 端に到達したら「停止」へ
            if (dir > 0 && enemyStatus.pos.x >= rightX)
            {
                enemyStatus.pos.x = rightX;
                enemyStatus.vel.x = 0.0;

                // 2〜3秒停止（簡易：timeで擬似ランダム）
                stopTimer = stopMinF + (enemyStatus.time % (stopMaxF - stopMinF + 1));
                state = 2;
            }
            else if (dir < 0 && enemyStatus.pos.x <= leftX)
            {
                enemyStatus.pos.x = leftX;
                enemyStatus.vel.x = 0.0;

                stopTimer = stopMinF + (enemyStatus.time % (stopMaxF - stopMinF + 1));
                state = 2;
            }
        }
        // 6) 停止（2〜3秒）
        else // state == 2
        {
            enemyStatus.vel.x = 0.0;

            if (stopTimer > 0)
                stopTimer--;

            // 停止が終わったら向きを反転して次のダッシュへ
            if (stopTimer <= 0)
            {
                dir *= -1;
                state = 1;
            }
        }
    }
    break;

    // =========================================================
    // moveType = 5 : サイン波で降下（basePos.x を基準）
    //   - basePos.x を中心に sin 波で左右に揺れる
    //   - y は一定速度で降下
    // =========================================================
    case 5:
    {
        const double fallSpeed = 2.0;
        const double amp = 60.0;
        const double omega = 0.05;

        enemyStatus.pos.x = enemyStatus.basePos.x + std::sin(enemyStatus.time * omega) * amp;
        enemyStatus.vel = {0.0, fallSpeed};
    }
    break;

    // =========================================================
    // moveType = 6 : 距離を保つ追従（寄りすぎない追尾）
    //   - 目標距離 targetDist を保つように速度を作る
    //   - 近いと離れる / 遠いと近づく
    // =========================================================
    case 6:
    {
        const double targetDist = 180.0;
        const double maxSpeed = 2.2;
        const double response = 0.04;

        Vec2d diff{playerPos.x - enemyStatus.pos.x, playerPos.y - enemyStatus.pos.y};
        double d = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        if (d > 1e-6)
        {
            diff.x /= d;
            diff.y /= d;
        }
        else
        {
            diff = {0.0, 0.0};
        }

        double err = d - targetDist;
        Vec2d targetVel{diff.x * err * response, diff.y * err * response};

        double sp = std::sqrt(targetVel.x * targetVel.x + targetVel.y * targetVel.y);
        if (sp > maxSpeed && sp > 1e-6)
        {
            targetVel.x = targetVel.x / sp * maxSpeed;
            targetVel.y = targetVel.y / sp * maxSpeed;
        }

        enemyStatus.vel.x = enemyStatus.vel.x * 0.85 + targetVel.x * 0.15;
        enemyStatus.vel.y = enemyStatus.vel.y * 0.85 + targetVel.y * 0.15;
    }
    break;

    // =========================================================
    // moveType = 7 : 円運動（centerPos を中心）
    //   - centerPos を中心に cos/sin で円運動
    //   - pos を直接決めるため vel は 0
    // =========================================================
    case 7:
    {
        const double radius = 90.0;
        const double omega = 0.03;

        double ang = enemyStatus.time * omega;
        enemyStatus.pos.x = enemyStatus.centerPos.x + std::cos(ang) * radius;
        enemyStatus.pos.y = enemyStatus.centerPos.y + std::sin(ang) * radius;

        enemyStatus.vel = {0.0, 0.0};
    }
    break;

    // =========================================================
    // moveType = 8 : ダッシュ→停止→漂い（周期）
    //   - 周期ごとにプレイヤー方向へ短時間ダッシュ
    //   - その後停止し、わずかに漂う（単調さ回避）
    // =========================================================
    case 8:
    {
        const int cycle = 120;
        const int dashT = 30;
        const double dashSpeed = 4.0;

        int t = enemyStatus.time % cycle;

        if (t == 0)
        {
            Vec2d diff{playerPos.x - enemyStatus.pos.x, playerPos.y - enemyStatus.pos.y};
            double len = std::sqrt(diff.x * diff.x + diff.y * diff.y);

            if (len > 1e-6)
            {
                diff.x /= len;
                diff.y /= len;
            }
            else
            {
                diff = {0.0, 1.0};
            }

            enemyStatus.vel = {diff.x * dashSpeed, diff.y * dashSpeed};
        }
        else if (t == dashT)
        {
            enemyStatus.vel = {0.0, 0.0};
        }
        else if (t > dashT)
        {
            enemyStatus.vel.x *= 0.95;
            enemyStatus.vel.y *= 0.95;
            enemyStatus.vel.y += 0.012; // ほんの少し下へ流す
        }
    }
    break;

    // =========================================================
    // moveType = 9 : ワープ（一定間隔で座標を飛ばす）
    //   - warpInterval ごとに上部エリアへ瞬間移動
    //   - ワープ後に basePos/centerPos を更新（任意）
    // =========================================================
    case 9:
    {
        const int warpInterval = 90;
        const double minX = 80.0;
        const double maxX = 640.0 - 80.0;
        const double minY = 80.0;
        const double maxY = 200.0;

        enemyStatus.vel = {0.0, 0.0};

        if (enemyStatus.time > 0 && enemyStatus.time % warpInterval == 0)
        {
            double rx = (double)GetRand(10000) / 10000.0;
            double ry = (double)GetRand(10000) / 10000.0;

            enemyStatus.pos.x = minX + (maxX - minX) * rx;
            enemyStatus.pos.y = minY + (maxY - minY) * ry;

            // ワープ後に基準も更新しておくと、次の動きに繋げやすい
            enemyStatus.basePos = enemyStatus.pos;
            enemyStatus.centerPos = enemyStatus.pos;
        }
    }
    break;

    // =========================================================
    // moveType = 10 : 画面外周をなぞる（矩形に沿って移動）
    //   - 右→下→左→上 の順で矩形の辺を周回
    //   - 画面の“掃除機”みたいな圧を作れる
    // =========================================================
    case 10:
    {
        const double speed = 2.5;
        const double left = 60.0;
        const double right = 640.0 - 60.0;
        const double top = 60.0;
        const double bottom = 360.0; // 縦STGに合わせて調整

        if (enemyStatus.time == 0)
        {
            enemyStatus.vel = {speed, 0.0};
        }

        if (enemyStatus.pos.x >= right && enemyStatus.vel.x > 0.0)
        {
            enemyStatus.pos.x = right;
            enemyStatus.vel = {0.0, speed};
        }
        else if (enemyStatus.pos.y >= bottom && enemyStatus.vel.y > 0.0)
        {
            enemyStatus.pos.y = bottom;
            enemyStatus.vel = {-speed, 0.0};
        }
        else if (enemyStatus.pos.x <= left && enemyStatus.vel.x < 0.0)
        {
            enemyStatus.pos.x = left;
            enemyStatus.vel = {0.0, -speed};
        }
        else if (enemyStatus.pos.y <= top && enemyStatus.vel.y < 0.0)
        {
            enemyStatus.pos.y = top;
            enemyStatus.vel = {speed, 0.0};
        }
    }
    break;

    // =========================================================
    // moveType = 100 : 例：ボス初期停止
    //   - time==60 で停止するだけの簡易挙動
    // =========================================================
    case 100:
    {
        if (enemyStatus.time == 60)
        {
            enemyStatus.vel = {0.0, 0.0};
        }
    }
    break;

    // =========================================================
    // default : 未定義
    // =========================================================
    default:
        break;
    }

    // =========================================================
    // 位置更新（共通）
    // =========================================================
    enemyStatus.pos.x += enemyStatus.vel.x;
    enemyStatus.pos.y += enemyStatus.vel.y;
}

void Enemy::shootSpell(EnemyShootScript *enemyShootScript, BombManager *bMgr, BombInfo bombs[MAX_BOMBS], int time, int difficulty, Player player)
{
    switch (this->getSpellInfo().spellType)
    {
    case 1:
        enemyShootScript->Boss01Spell01(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 2:
        enemyShootScript->Boss01Spell02(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 3:
        enemyShootScript->Boss01Spell03(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 4:
        enemyShootScript->Boss01Spell04(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 5:
        enemyShootScript->Boss01Spell05(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 6:
        enemyShootScript->Boss01Spell06(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 7:
        enemyShootScript->Boss01Spell07(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 8:
        enemyShootScript->Boss01Spell08(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 9:
        enemyShootScript->Boss01Spell09(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 10:
        enemyShootScript->Boss01Spell10(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 11:
        enemyShootScript->Boss01Spell11(*this, *bMgr, bombs, time, difficulty, player);
        break;
    case 12:
        enemyShootScript->Boss01Spell12(*this, *bMgr, bombs, time, difficulty, player);
    default:
        break;
    }
}