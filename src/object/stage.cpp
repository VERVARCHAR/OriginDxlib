#ifndef _STAGE_HPP_
#define _STAGE_HPP_
#include "object/stage.hpp"
#endif

#ifndef _ENEMY_IO_HPP_
#define _ENEMY_IO_HPP_
#include "system/enemyIO.hpp"
#endif

StageManager::StageManager(int _stage, int _time, int _difficulty)
{
    stage = _stage;
    time = _time;
    difficulty = _difficulty;
    latestEnemyId = 0;
    Vec2d initVec2d = {0, 0};
    EnemyStatus init = {
        {-1000, -1000},
        {-1000, -1000},
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        "Default"};

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemys[i] = new Enemy(init);
    }
}
StageManager::~StageManager()
{
    for (int i = 0; i < MAX_ENEMIES; ++i)
    {
        delete enemys[i]; // メモリ解放を忘れずに
        enemys[i] = nullptr;
    }
}

void StageManager::LoadFromVector(const std::vector<EnemyStatus> &src)
{
    enemyCount = min(static_cast<int>(src.size()), MAX_ENEMIES);
    for (int i = 0; i < enemyCount; ++i)
    {
        enemys[i]->setStatus(src[i]);
    }
    std::cout << "[EnemyManager]" << enemyCount << " enemies registered\n";
}

void StageManager::loadEnemy()
{
    // TODO : ローディング画面
    if (LoadEnemyDataFromJson("../../stageInfos/stage01/stage01.json", loadEnemies))
    {
        LoadFromVector(loadEnemies);
    }
    else
    {
        DrawFormatString(200, 200, GetColor(255, 255, 255), L"[ERROR]");
    }
    // return 1;
}

void StageManager::spwanEnemy(int index, EnemyStatus enemyStatus)
{
    enemys[index]->setStatus(enemyStatus);
}

int StageManager::getEmptyIndex()
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {

        if (!enemys[i]->getStatus().isAlive)
        {
            return i;
        }
    }
    return -1;
}

void StageManager::deleteEnemy(int index)
{
    EnemyStatus tmp = enemys[index]->getStatus();
    tmp.isAlive = false;
    enemys[index]->setStatus(tmp);
}

void StageManager::updateStage(BombManager *bMgr, BombInfo bombs[MAX_BOMBS], Player *player)
{
    int count = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemys[i]->enemyStatus.spwanTime == this->time)
        {
            enemys[i]->setIsAlive(true);
        }
        if (enemys[i] != nullptr && enemys[i]->enemyStatus.isAlive)
        {
            enemys[i]->enemyUpdate(this->time, difficulty, bMgr, bombs, enemyShootScript, *player);
        }
    }

    player->debugStatus();

    player->playerUpdate(*bMgr, bombs, this);
    bMgr->updateBombs(bombs);
    bMgr->drawBombs(bombs);
    printfDx(L"times : %d\n", time);
    printfDx(L"count : %d\n", count);

    time++;
}
