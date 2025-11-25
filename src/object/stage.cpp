#ifndef _STAGE_HPP_
#define _STAGE_HPP_
#include "object/stage.hpp"
#endif

#ifndef _ENEMY_IO_HPP_
#define _ENEMY_IO_HPP_
#include "system/enemyIO.hpp"
#endif

#include <algorithm>

StageManager::StageManager(int _stage, int _time, int _difficulty)
{
    init(_stage, _time, _difficulty);
}
StageManager::~StageManager()
{
    for (int i = 0; i < MAX_ENEMIES; ++i)
    {
        delete enemys[i]; // メモリ解放を忘れずに
        enemys[i] = nullptr;
    }
}

void StageManager::init(int _stage, int _time, int _difficulty)
{
    stageInfo.stage = _stage;
    stageInfo.difficulty = _difficulty;
    stageInfo.score = 0;

    time = _time;
    isTalk = false;
    latestEnemyId = 0;
    isTalk = false;
    talkCount = 0;
    // string talkString[20] = {'\0'};
    // talkWho[20] = {0};

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
        -1,
        -1,
        -1,
        "Default"};

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemys[i] = new Enemy(init);
    }

    isPause = false;
    isTalk = false;
    isClearStage = false;
    isGameOver = false;
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
    char Paths[8][256] = {
        "../../stageInfos/stage00/stage00.json",
        "../../stageInfos/stage01/stage01.json",
        "../../stageInfos/stage02/stage02.json",
        "../../stageInfos/stage03/stage03.json",
        "../../stageInfos/stage04/stage04.json",
        "../../stageInfos/stage05/stage05.json",
        "../../stageInfos/stage06/stage06.json",
        "../../stageInfos/stage07/stage07.json",
    };

    // TODO : ローディング画面
    // TODO : stageの値によってパスを変える(関数を別にしてもいいかも???)
    if (LoadEnemyDataFromJson(Paths[stageInfo.stage], loadEnemies))
    {
        LoadFromVector(loadEnemies);

        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemys[i]->enemyStatus.type >= 100)
            {
                bossIndex = i;
            }
        }

        // TODO 会話内容もいろいろしないとなぁ
        // Bossのtypeを取得してtalk()に渡すかな??
        talk(100);
    }
    else
    {
        DrawFormatString(200, 200, GetColor(255, 255, 255), L"[ERROR]");
    }

    // LoadEnemyImage("../../img/EnemyProtoType01.png", enemyImageHandle);
    LoadDivGraph(L"..\\..\\img\\Enemy.png", 16, 4, 4, 256, 256, enemyImageHandle);

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
    // TODO 会話するフレームも受け取りたいねぇ
    if (time == 480)
    {
        isTalk = true;
        bMgr->removeBomb(bombs);
    }

    // TODO
    if (!isTalk && time >= 0 && !isPause && !isGameOver)
    {
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemys[i]->enemyStatus.spwanTime == this->time)
            {
                enemys[i]->setIsAlive(true);
                enemys[i]->setImageHandle(enemyImageHandle[enemys[i]->enemyStatus.type % 100]);
            }
            if (enemys[i] != nullptr && enemys[i]->enemyStatus.isAlive)
            {
                enemys[i]->enemyUpdate(this->time, this->stageInfo.difficulty, bMgr, bombs, enemyShootScript, player);
            }
        }
        stageInfo.score += 10;
        time++;
    }

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemys[i] != nullptr && enemys[i]->enemyStatus.isAlive)
        {
            enemys[i]->enemyDraw();
            printfDx(L"enemy type:%d", enemys[i]->enemyStatus.type);
        }
    }

    stageInfo.nowStatus = player->getStatus();
    player->debugStatus();

    if (!isPause && !isGameOver)
    {
        player->playerUpdate(*bMgr, bombs);
        bMgr->updateBombs(bombs);
        getClearStage();
        getGameOver(player);
    }

    player->playerDraw();
    bMgr->drawBombs(bombs);

    if (isClearStage)
    {
        bMgr->removeBomb(bombs);
        // TODO ステージクリア処理
        stageInfo.stage += 1;
        time = -120;
        isClearStage = false;
        loadEnemy();
    }

    if (time < 0)
    {
        DrawFormatString(300, 300, GetColor(255, 255, 255), L"Go to the Next Stage...");
        time++;
    }

    printfDx(L"times : %d\n", time);

    printfDx(L"Boss Index : %d\n", bossIndex);
    printfDx(L"Clear : %d\n", isClearStage);
}

void StageManager::getClearStage()
{
    if (enemys[bossIndex]->enemyStatus.lives == 0)
    {
        isClearStage = true;
    }
}

void StageManager::getGameOver(Player *player)
{
    if (player->getStatus().lives == 0)
    {
        isGameOver = true;
    }
}

// TODO : 会話データをJSONでもなんでもいいから外部から取得しないとだね
void StageManager::talk(int type)
{
    switch (type)
    {
    case 100:
        talkString[0] = "talk01";
        talkWho[0] = 0;
        talkString[1] = "talk02";
        talkWho[1] = 0;
        talkString[2] = "talk03";
        talkWho[2] = 1;
        talkString[3] = "talk04";
        talkWho[3] = 0;
        talkString[4] = "talk05";
        talkWho[4] = 1;
        talkString[5] = "talk06";
        talkWho[5] = 0;
        talkString[6] = "talk07";
        talkWho[6] = 1;
        break;

    default:
        break;
    }
}