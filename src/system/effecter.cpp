
#ifndef _EFFECTER_HPP_
#define _EFFECTER_HPP_
#include "system/effecter.hpp"
#endif

Effecter::Effecter()
{
    init();
}

Effecter::~Effecter()
{
    // 今は特に何もしない（DxLib_End で解放）
}

void Effecter::init()
{
    for (int i = 0; i < MAX_EFFECTS; ++i)
    {
        effects[i].isAlive = false;
    }
}

void Effecter::loadEffecter()
{
    SetUseASyncLoadFlag(TRUE);

    // TODO: 実際のパスはプロジェクト構成に合わせて
    // hitSparkHandle = LoadGraph("../../../img/effect/hitSpark.png");
    LoadDivGraph(L"..\\..\\assets\\effects\\enemyDeadSakura.png", 9, 3, 3, 341, 341, enemyExplodeHandle[0]);
    // LoadDivGraph(L"..\\..\\assets\\effects\\enemyDeadEffect02.png", 10, 10, 1, 240, 240, enemyExplodeHandle[1]);
    // LoadDivGraph(L"..\\..\\assets\\effects\\enemyDeadEffect03.png", 10, 10, 1, 240, 240, enemyExplodeHandle[2]);
    LoadDivGraph(L"..\\..\\assets\\effects\\sakuraEffect02.png", 12, 4, 3, 256, 256, bulletVanishHandle);
    LoadDivGraph(L"..\\..\\assets\\effects\\playerDeadEffect.png", 10, 10, 1, 360, 360, playerExplodeHandle);
    LoadDivGraph(L"..\\..\\assets\\effects\\effectParticle.png", 16, 4, 4, 256, 256, particleHandle);
    LoadDivGraph(L"..\\..\\assets\\effects\\sakuraEffect.png", 12, 4, 3, 256, 256, sakuraEffect);

    seShot = LoadSoundMem(L"..\\..\\assets\\SE\\se_beam06.mp3");
    ChangeVolumeSoundMem(128, seShot);
    seEnemyDead = LoadSoundMem(L"..\\..\\assets\\SE\\enemy_vanish.wav");
    seBossDead = LoadSoundMem(L"..\\..\\assets\\SE\\boss_dead.wav");
    sePlayerDead = LoadSoundMem(L"..\\..\\assets\\SE\\player_dead.wav");
    seVanish = LoadSoundMem(L"..\\..\\assets\\SE\\bomb_vanish.wav");
    seGraze = LoadSoundMem(L"..\\..\\assets\\SE\\graze.wav");
    seSpell = LoadSoundMem(L"..\\..\\assets\\SE\\spell.wav");
    // seBomb = LoadSoundMem("../../../se/bomb.wav");
    SetUseASyncLoadFlag(FALSE);
}

void Effecter::effecterUpdate()
{
    for (int i = 0; i < MAX_EFFECTS; ++i)
    {
        if (!effects[i].isAlive)
            continue;

        Effect &e = effects[i];
        e.time++;

        // アニメコマ進行
        if (e.divNum > 1 && e.time % e.frameInterval == 0)
        {
            e.frame++;
        }

        if (e.time > e.lifeTime)
        {
            e.isAlive = false;
        }
    }
}

void Effecter::effecterDraw()
{
    for (int i = 0; i < MAX_EFFECTS; ++i)
    {
        if (!effects[i].isAlive)
            continue;
        Effect &e = effects[i];

        int handle = -1;
        switch (e.type)
        {
        case EffectType::HitSpark:
            // handle = hitSparkHandle;
            break;
        case EffectType::EnemyExplode:
            handle = enemyExplodeHandle[0][(int)(e.time / 3)];
            break;
        case EffectType::BulletVanish:
            handle = bulletVanishHandle[(int)(e.time / 6)];
            break;
        case EffectType::PlayerExplode:
            handle = playerExplodeHandle[(int)(e.time / 6)];
            break;
        case EffectType::Spell:
            // handle = playerExplodeHandle[(int)(e.time / 6)];
            break;
        case EffectType::EnemySpell:

            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
            DrawBox(10, 10, 790, 710, GetColor(0, 0, 0), TRUE);
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

            if (e.time < 60)
            {
                e.pos.x -= 4;
                e.pos.y += 2;
            }

            if (e.time > 90)
            {
                e.pos.y -= 5;
            }
            handle = enemyCutInhandle;
            break;
        case EffectType::Graze:
            // handle = playerExplodeHandle[(int)(e.time / 6)];
            break;
        // case EffectType::PlayerExplode:
        //     handle = playerExplodeHandle[(int)(e.time / 6)];
        //     break;
        default:
            break;
        }
        if (handle < 0)
            continue;

        // 単純に DrawRotaGraph で描画
        DrawRotaGraphF(
            (float)e.pos.x,
            (float)e.pos.y,
            e.scale,
            0.0,
            handle,
            TRUE);
    }
}

int Effecter::getEmptyIndex()
{
    for (int i = 0; i < MAX_EFFECTS; ++i)
    {
        if (!effects[i].isAlive)
            return i;
    }
    return -1;
}

void Effecter::spawnEffect(EffectType type, const Vec2d &pos)
{
    int idx = getEmptyIndex();
    if (idx < 0)
        return;

    Effect &e = effects[idx];
    e.isAlive = true;
    e.type = type;
    e.pos = pos;
    e.time = 0;
    e.frame = 0;
    e.divNum = 1; // 暫定
    e.frameInterval = 3;
    e.scale = 1.0f;
    e.alpha = 255;

    switch (type)
    {
    case EffectType::HitSpark:
        e.lifeTime = 60;
        break;
    case EffectType::EnemyExplode:
        e.lifeTime = 60;
        e.scale = 0.5f;
        e.alpha = 128;
        break;
    case EffectType::BulletVanish:
        e.lifeTime = 60;
        break;
    case EffectType::PlayerExplode:
        e.lifeTime = 60;
        break;
    case EffectType::Spell:
        e.lifeTime = 120;
        break;
    case EffectType::Graze:
        e.lifeTime = 10;
        break;
    case EffectType::EnemySpell:
        e.lifeTime = 120;
        break;
    default:
        e.lifeTime = 60;
        break;
    }
}

void Effecter::playHitSpark(const Vec2d &pos)
{
    spawnEffect(EffectType::HitSpark, pos);
    PlaySoundMem(seShot, DX_PLAYTYPE_BACK); // 例: 被弾音でもOK
}

void Effecter::playEnemyExplode(const Vec2d &pos)
{
    spawnEffect(EffectType::EnemyExplode, pos);
    PlaySoundMem(seEnemyDead, DX_PLAYTYPE_BACK);
}

void Effecter::playBossExplode(const Vec2d &pos)
{
    spawnEffect(EffectType::EnemyExplode, pos);
    PlaySoundMem(seBossDead, DX_PLAYTYPE_BACK);
}

void Effecter::playBulletVanish(const Vec2d &pos)
{
    PlaySoundMem(seVanish, DX_PLAYTYPE_BACK);
    spawnEffect(EffectType::BulletVanish, pos);
}

void Effecter::playPlayerExplode(const Vec2d &pos)
{
    spawnEffect(EffectType::PlayerExplode, pos);
    PlaySoundMem(sePlayerDead, DX_PLAYTYPE_BACK);
}

void Effecter::playSpell(const Vec2d &pos)
{
    spawnEffect(EffectType::Spell, pos);
    PlaySoundMem(seSpell, DX_PLAYTYPE_BACK);
}

void Effecter::playEnemySpell(const Vec2d &pos)
{
    // spawnEffect(EffectType::Spell, pos);
    spawnEffect(EffectType::EnemySpell, pos);
    PlaySoundMem(seSpell, DX_PLAYTYPE_BACK);
}

void Effecter::playGraze(const Vec2d &pos)
{
    spawnEffect(EffectType::Graze, pos);
    PlaySoundMem(seGraze, DX_PLAYTYPE_BACK);
}

// SE だけ鳴らすAPIも一応用意
void Effecter::playSE_Shot() { PlaySoundMem(seShot, DX_PLAYTYPE_BACK); }
void Effecter::playSE_EnemyDead() { PlaySoundMem(seEnemyDead, DX_PLAYTYPE_BACK); }
void Effecter::playSE_BossDead() { PlaySoundMem(seBossDead, DX_PLAYTYPE_BACK); }
void Effecter::playSE_PlayerDead() { PlaySoundMem(sePlayerDead, DX_PLAYTYPE_BACK); }
void Effecter::playSE_Bomb() { PlaySoundMem(seBomb, DX_PLAYTYPE_BACK); }
void Effecter::PlaySE_BombVanish() { PlaySoundMem(seVanish, DX_PLAYTYPE_BACK); };
;
