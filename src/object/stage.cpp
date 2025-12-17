#ifndef _STAGE_HPP_
#define _STAGE_HPP_
#include "object/stage.hpp"
#endif

#ifndef _ENEMY_IO_HPP_
#define _ENEMY_IO_HPP_
#include "system/enemyIO.hpp"
#endif

#include <algorithm>
#include <sstream>
#include "system/logger.hpp"

StageManager::StageManager(int _stage, int _time, Difficulty _difficulty)
{
    init(_stage, _time, _difficulty);
}

StageManager::~StageManager()
{
    for (int i = 0; i < MAX_ENEMIES; ++i)
    {
        delete enemies[i];
        enemies[i] = nullptr;
    }
}

void StageManager::init(int _stage, int _time, Difficulty _difficulty)
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
        -1,
        -1,
        -1,
        -1,
        L"Default"};

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i] = new Enemy(init);
    }

    isPause = false;
    isTalk = false;
    isClearStage = false;
    isGameOver = false;
}

void StageManager::LoadFromVector(const std::vector<EnemyStatus> &srcEnemyStatus, const std::vector<SpellInfo> &srcSpellInfo)
{
    enemyCount = min(static_cast<int>(srcEnemyStatus.size()), MAX_ENEMIES);
    for (int i = 0; i < enemyCount; ++i)
    {
        enemies[i]->setStatus(srcEnemyStatus[i]);
        Logger::Log("(LoadFromVector) index " + to_string(i) + " enemy registered", LogLevel::Info);
        Logger::Log("(LoadFromVector) index enemy spawn time is " + to_string(enemies[i]->enemyStatus.spawnTime), LogLevel::Info);
    }

    int spellCount = min(static_cast<int>(srcSpellInfo.size()), MAX_ENEMIES);
    enemies[bossIndex]->spellInfo.clear();
    for (int i = 0; i < spellCount; i++)
    {
        enemies[bossIndex]->setSpellData(srcSpellInfo[i]);
    }
    std::cout << "[EnemyManager]" << enemyCount << " enemies registered\n";
    std::cout << "[EnemyManager]" << spellCount << " spellCount registered\n";

    Logger::Log("(LoadFromVector) " + to_string(enemyCount) + " enemies registered", LogLevel::Info);
    Logger::Log("(LoadFromVector) " + to_string(spellCount) + " spellCount registered", LogLevel::Info);
}

void StageManager::loadEnemy()
{
    // TODO 敵情報のパスのやり方変えたいね
    const char *stageFile;
    const char *bossFile;
    const char *talkFile;
    if (stageInfo.difficulty == Difficulty::EXTRA)
    {
        // エクストラモードの場合
        stageFile = "../../stageInfos/extraStage.json";
        bossFile = "../../stageInfos/extraBoss.json";
        talkFile = "../../stageInfos/extraTalk.json";
    }
    else
    {
        // ストーリーモード（EXTRA以外の難易度）の場合
        stageFile = "../../stageInfos/storyStage.json";
        bossFile = "../../stageInfos/storyBoss.json";
        talkFile = "../../stageInfos/storyTalk.json";
    }

    std::vector<SpellInfo> loadSpells;
    // TODO : ローディング画面
    // TODO : stageの値によってパスを変える(関数を別にしてもいいかも???)
    if (LoadEnemyDataFromJson(stageInfo.stage, stageFile, bossFile, loadEnemies, loadSpells, &allStageLength))
    {
        bossIndex = 0;
        for (size_t i = 0; i < loadEnemies.size(); ++i)
        {
            if (loadEnemies[i].type >= 100)
            {
                bossIndex = static_cast<int>(i);
                break;
            }
        }

        LoadFromVector(loadEnemies, loadSpells);

        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemies[i]->enemyStatus.type >= 100)
            {
                bossIndex = i;
                enemies[i]->enemyStatus.isInvincible = true;
                enemies[i]->enemyStatus.invincibleTime = 120;
                enemies[i]->enemyStatus.spellCount = 0;
            }
        }

        // TODO 会話内容もいろいろしないとなぁ
        // Bossのtypeを取得してtalk()に渡すかな??
        std::vector<TalkData> talkLines;
        if (LoadTalkDataFromJson(talkFile, stageInfo.stage, talkLines))
        {
            this->talkLineCount = static_cast<int>(talkLines.size());
            // 読み込んだ会話をStageManagerにセット
            for (size_t i = 0; i < talkLines.size(); ++i)
            {
                this->talkData[i].talkString = talkLines[i].talkString;
                this->talkData[i].isTalkEnemy = talkLines[i].isTalkEnemy ? true : false;
            }
            // 会話行数に応じて管理変数を初期化
            this->talkCount = 0;
            // 必要なら、会話が存在することを示すフラグのセット（例: isTalk = falseのまま開始待機）
        }
    }
    else
    {
        DrawFormatString(200, 200, GetColor(255, 255, 255), L"[ERROR] (loadEnemy)");
        return;
    }

    // LoadEnemyImage("../../img/EnemyProtoType01.png", enemyImageHandle);
    LoadDivGraph(L"..\\..\\assets\\enemy\\Enemy.png", 16, 4, 4, 256, 256, enemyImageHandle);
    Logger::Log("(loadEnemy) pass loadEnemy.", LogLevel::Info);
    // return 1;
}

void StageManager::spawnEnemy(int index, EnemyStatus enemyStatus)
{
    enemies[index]->setStatus(enemyStatus);
    Logger::Log("(loadEnemy) pass spawnEnemy : " + to_string(index), LogLevel::Info);
}

int StageManager::getEmptyIndex()
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {

        if (!enemies[i]->getStatus().isAlive)
        {
            return i;
        }
    }
    return -1;
}

void StageManager::deleteEnemy(int index)
{
    EnemyStatus tmp = enemies[index]->getStatus();
    tmp.isAlive = false;
    enemies[index]->setStatus(tmp);
}

void StageManager::updateStage(BombManager *bMgr, ItemManager *iMgr, BombInfo bombs[MAX_BOMBS], Player *player, Effecter *effecter)
{

    // TODO 会話するフレームも受け取りたいねぇ
    if (time == enemies[bossIndex]->enemyStatus.spawnTime + 60)
    {
        isTalk = true;
        bMgr->removeBomb(bombs, effecter);
    }

    // TODO
    // 会話中でない, ゲームタイマが0以上, ゲームオーバーでないなら更新処理を行う
    if (time >= 0 && !isPause && !isGameOver)
    {
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemies[i] == nullptr)
            {
                printfDx(L"[CRASH DETECT] enemies[%d] is nullptr\n", i);
                Logger::Log("(updateStage) enemies[" + to_string(i) + "] is nullptr\n", LogLevel::Error);
                continue;
            }

            if (enemies[i]->enemyStatus.spawnTime == this->time)
            {
                Logger::Log("(updateStage) pass spawnEnemy : " + to_string(i), LogLevel::Info);
                enemies[i]->setIsAlive(true);
                enemies[i]->setImageHandle(enemyImageHandle[enemies[i]->enemyStatus.type % 100]);
            }

            if (isTalk && i != bossIndex)
            {
                enemies[i]->enemyStatus.isAlive = false;
                // TODO effect
            }

            if (enemies[i] != nullptr && enemies[i]->enemyStatus.isAlive && !isTalk)
            {
                enemies[i]->enemyUpdate(this->time, &stageInfo, bMgr, bombs, enemyShootScript, player, effecter, iMgr);
            }
        }
        if (!isTalk)
        {
            stageInfo.score += 10;
            time++;
        }
    }

    // UI用にプレイヤーのステータス情報を格納
    stageInfo.nowStatus = player->getStatus();

    // [DEBUG]
    player->debugStatus();

    // ポーズ中でない，ゲームオーバーでないならプレイヤー，弾幕の更新処理をする
    if (!isPause && !isGameOver)
    {
        player->playerUpdate(bMgr, bombs, effecter);
        bMgr->updateBombs(bombs);
        iMgr->updateItems(this, player);

        getClearStage();
        getGameOver(player);

        // プレイヤー関連のキー入力
        player->getKeyInput(isTalk);
    }

    // 描画処理
    // エフェクト -> 敵 -> 自機 -> 弾幕

    effecter->effecterDraw();

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i] != nullptr && enemies[i]->enemyStatus.isAlive)
        {
            enemies[i]->enemyDraw();

            // [DEBUG]
            printfDx(L"enemy type:%d", enemies[i]->enemyStatus.type);
        }
    }

    player->playerDraw();
    bMgr->drawBombs(bombs);
    iMgr->drawItems();
    effecter->effecterUpdate();

    // ボス撃破処理
    if (isClearStage && time > 0)
    {
        bMgr->removeBomb(bombs, effecter);
        // TODO ステージクリア処理
        time = -120;
    }

    // ボス撃破時にゲームタイマーが0未満になるので，その間にリザルト
    if (time < 0)
    {
        if (time == -1)
        {
            if (Key[KEY_INPUT_RETURN] == 1)
            {
                time = 0;
                stageInfo.stage += 1;
                isClearStage = false;
                loadEnemy();
            }
        }
        else
        {
            time++;
        }
        if (stageInfo.stage == allStageLength)
        {
            DrawFormatString(300, 300, GetColor(255, 255, 255), L"Clear...");
            isStoryClear = true;
        }
        else
        {
            DrawFormatString(300, 300, GetColor(255, 255, 255), L"Go to the Next Stage...");
        }
    }

    // [DEBUG]
    printfDx(L"times : %d\n", time);
    printfDx(L"Boss Index : %d\n", bossIndex);
    printfDx(L"Clear : %d\n", isClearStage);
    printfDx(L"Stage : %d\n", stageInfo.stage);
}

void StageManager::getClearStage()
{
    if (enemies[bossIndex]->enemyStatus.lives == 0)
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