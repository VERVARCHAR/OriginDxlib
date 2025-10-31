#ifndef _STAGE_HPP_
#define _STAGE_HPP_
#include "object/stage.hpp"
#endif

StageManager::StageManager(int _stage, int _time, int _difficulty)
{
    stage = _stage;
    time = _time;
    difficulty = _difficulty;
    Vec2d initVec2d = {0, 0};
    EnemyStatus init = {
        {0, 0},
        {0, 0},
        0,
        0,
        0,
        0,
        0,
        0,
        false,
        "Default"};
    for (int i = 0; i < MAX_ENEMYS; i++)
    {
        enemys[i] = new Enemy(init);
    }
}
StageManager::~StageManager()
{
    for (int i = 0; i < MAX_ENEMYS; ++i)
    {
        delete enemys[i]; // メモリ解放を忘れずに
        enemys[i] = nullptr;
    }
}

void StageManager::loadEnemy(int index, EnemyStatus enemyStatus)
{
    enemys[index]->setStatus(enemyStatus);
}

int StageManager::getEmptyIndex()
{
    for (int i = 0; i < MAX_ENEMYS; i++)
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
    if (time % 60 == 0)
    {
        for (int i = 0; i < 10; i++)
        {
            EnemyStatus init = {
                {300 + 10 * i, 200},
                {0, 2},
                0,
                1,
                1,
                10,
                0,
                time + i,
                true,
                "Name"};
            loadEnemy(getEmptyIndex(), init);
        }
    }

    for (int i = 0; i < MAX_ENEMYS; i++)
    {
        if (enemys[i] != nullptr && enemys[i]->getStatus().isAlive)
        {
            enemyShootScript.BombType01(*enemys[i], *bMgr, bombs, time, difficulty);
            enemys[i]->enemyUpdate(time, difficulty, *bMgr, bombs, enemyShootScript);
        }
    }

    player->debugStatus();
    // printfDx(L"%d\n", enemy.getHP());

    player->playerUpdate(*bMgr, bombs);
    bMgr->updateBombs(bombs);
    bMgr->drawBombs(bombs);
    printfDx(L"times : %d\n", time);

    time++;
}