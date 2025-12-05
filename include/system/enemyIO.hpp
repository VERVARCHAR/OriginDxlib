// enemyIO.hpp
#pragma once

#ifndef _UTILS_HPP_
#define _UTILS_HPP_
#include "system/utils.hpp"
#endif

#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_MANAGER_HPP_

#ifndef _ENEMY_HPP_
#define _ENEMY_HPP_
#include "object/enemy.hpp"
#endif // _ENEMY_HPP_s

#ifndef _ENEMY_SHOOT_SCRIPT_01_HPP_
#define _ENEMY_SHOOT_SCRIPT_01_HPP_
#include "object/enemyShootScripts/01.hpp"
#endif // _ENEMY_SHOOT_SCRIPT_01_HPP_

#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_
#include "object/player.hpp"
#endif // _PLAYER_HPP_

#ifndef _STAGE_HPP_
#define _STAGE_HPP_
#include "object/stage.hpp"
#endif

#ifndef _UI_HPP_
#define _UI_HPP_
#include "system/UI.hpp"
#endif

#include "system/logger.hpp"

// JSONから敵情報を読み取る関数
inline bool LoadEnemyDataFromJson(int stageIdx, const std::string &stageInfoPath, const std::string &bossInfoPath, std::vector<EnemyStatus> &outEnemies, std::vector<SpellInfo> &spells, int *allStageLength)
{
    outEnemies.clear();
    spells.clear();
    std::ifstream stageInfoFile(stageInfoPath);

    Logger::Log("Into LoadEnemyDataFromJson", LogLevel::Info);

    if (!stageInfoFile.is_open())
    {
        std::cerr << L"[ERROR] cant open file " << stageInfoPath << std::endl;
        return false;
    }
    std::cerr << "Pass stageInfofile\n";
    std::ifstream bossInfoFile(bossInfoPath);
    if (!bossInfoFile.is_open())
    {
        std::cerr << L"[ERROR] cant open file " << bossInfoPath << std::endl;
        return false;
    }
    std::cerr << "Pass bossInfofile\n";
    json jStage, jBoss;
    try
    {
        stageInfoFile >> jStage;
    }
    catch (const std::exception &e)
    {
        std::cerr << L"[ERROR] JSON Parsing Error:" << e.what() << std::endl;
        Logger::Log(std::string("(LoadEnemyDataFromJson)\tJSON Parsing Error: ") + e.what(), LogLevel::Error);
        return false;
    }

    try
    {
        bossInfoFile >> jBoss;
    }
    catch (const std::exception &e)
    {
        std::cerr << L"[ERROR] JSON Parsing Error:" << e.what() << std::endl;
        Logger::Log(std::string("(LoadEnemyDataFromJson)\tJSON Parsing Error: ") + e.what(), LogLevel::Error);
        return false;
    }
    Logger::Log("(LoadEnemyDataFromJson)\tPass parsing Json.", LogLevel::Info);
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    int idx = stageIdx - 1;
    Logger::Log("(LoadEnemyDataFromJson)\tstageIdx :" + std::to_string(stageIdx) + ", idx :" + std::to_string(idx), LogLevel::Info);
    *allStageLength = jStage["stages"].size();

    // "stages"配列から指定のステージデータを取得
    if (!jStage.contains("stages") || !jStage["stages"].is_array() || idx >= jStage["stages"].size())
    {
        std::cerr << "[ERROR] stage data for index " << idx << " not found\n";
        Logger::Log("(LoadEnemyDataFromJson)\tstage data for index " + to_string(idx) + " not found\n", LogLevel::Error);
        return false;
    }
    Logger::Log("(LoadEnemyDataFromJson)\tpass get stage data from json", LogLevel::Info);
    json stageData = jStage["stages"][idx];

    // 同様に"bosses"配列から該当ステージのボスデータ取得
    json bossData;
    if (!jBoss.contains("bosses") || !jBoss["bosses"].is_array())
    {
        std::cerr << "[ERROR] boss data for index " << idx << " not found\n";
        Logger::Log("(LoadEnemyDataFromJson)\tboss data for index " + to_string(idx) + " not found\n", LogLevel::Error);
        return false;
    }
    Logger::Log("(LoadEnemyDataFromJson)\tpass get boss data from json", LogLevel::Info);
    if (idx < jBoss["bosses"].size())
    {
        bossData = jBoss["bosses"][idx];
    }
    else
    {
        // ボスデータが見つからない場合の処理（ステージによってはボスがいない場合も想定）
        bossData = json::object();
    }
    Logger::Log("(LoadEnemyDataFromJson)\tpass to check boss size.", LogLevel::Info);

    // 敵データをパース
    if (!stageData.contains("enemies") || !stageData["enemies"].is_array())
    {
        std::cerr << "[ERROR] \"enemies\" array not found in stage data\n";
        Logger::Log("(LoadEnemyDataFromJson)\t\"enemies\" array not found in stage data\n", LogLevel::Error);
        return false;
    }
    Logger::Log("(LoadEnemyDataFromJson)\tpass to check boss array.", LogLevel::Info);

    for (auto &item : stageData["enemies"])
    {
        EnemyStatus e{};
        e.name = converter.from_bytes(item.value("name", ""));
        Logger::Log("(LoadEnemyDataFromJson)\tpush back enemy data name:" + (item.value("name", "")), LogLevel::Info);
        e.id = item.value("id", 0);
        Logger::Log("(LoadEnemyDataFromJson)\tpush back enemy data id:" + to_string(e.id), LogLevel::Info);
        e.type = item.value("type", 0);
        Logger::Log("(LoadEnemyDataFromJson)\tpush back enemy data type:" + to_string(e.type), LogLevel::Info);
        e.moveType = item.value("moveType", 0);
        Logger::Log("(LoadEnemyDataFromJson)\tpush back enemy data moveType:" + to_string(e.moveType), LogLevel::Info);
        e.lives = item.value("lives", 1);
        Logger::Log("(LoadEnemyDataFromJson)\tpush back enemy data lives:" + to_string(e.lives), LogLevel::Info);
        e.hp = item.value("hp", 10);
        Logger::Log("(LoadEnemyDataFromJson)\tpush back enemy data hp:" + to_string(e.hp), LogLevel::Info);
        e.maxHp = e.hp;
        e.radius = item.value("radius", 12);
        Logger::Log("(LoadEnemyDataFromJson)\tpush back enemy data radius:" + to_string(e.radius), LogLevel::Info);
        e.shootType = item.value("shootType", 0);
        Logger::Log("(LoadEnemyDataFromJson)\tpush back enemy data shootType:" + to_string(e.shootType), LogLevel::Info);
        e.spawnTime = item.value("spawnTime", 0);
        Logger::Log("(LoadEnemyDataFromJson)\tpush back enemy data spawnTime:" + to_string(e.spawnTime), LogLevel::Info);
        e.time = item.value("time", 0);
        Logger::Log("(LoadEnemyDataFromJson)\tpush back enemy data time:" + to_string(e.time), LogLevel::Info);
        e.spellCount = item.value("spellCount", 0);
        Logger::Log("(LoadEnemyDataFromJson)\tpush back enemy data spellCount:" + to_string(e.spellCount), LogLevel::Info);
        e.isAlive = item.value("isAlive", 0);
        Logger::Log("(LoadEnemyDataFromJson)\tpush back enemy data isAlive:" + to_string(e.isAlive), LogLevel::Info);
        e.isSpell = item.value("isSpell", 0);
        Logger::Log("(LoadEnemyDataFromJson)\tpush back enemy data isSpell:" + to_string(e.isSpell), LogLevel::Info);
        e.isInvincible = item.value("isInvincible", 0);
        Logger::Log("(LoadEnemyDataFromJson)\tpush back enemy data isInvincible:" + to_string(e.isInvincible), LogLevel::Info);
        e.invincibleTime = item.value("invincibleTime", 0);
        Logger::Log("(LoadEnemyDataFromJson)\tpush back enemy data invincibleTime:" + to_string(e.invincibleTime), LogLevel::Info);
        e.pos = {
            item["pos"].value("x", 0.0),
            item["pos"].value("y", 0.0)};
        e.vel = {
            item["vel"].value("x", 0.0),
            item["vel"].value("y", 0.0)};
        outEnemies.push_back(e);
        Logger::Log("(LoadEnemyDataFromJson)\tpush back enemy id" + to_string(e.id), LogLevel::Info);

        // ボスタイプ判定してスペル読み込み
        if (e.type >= 100)
        {
            if (bossData.contains("spells") && bossData["spells"].is_array())
            {
                for (auto &sItem : bossData["spells"])
                {
                    SpellInfo s{};
                    std::string spellName_str = sItem.value("spellName", "");
                    std::wstring spellName_w = converter.from_bytes(spellName_str);
                    Logger::Log("(LoadEnemyDataFromJson)\tpush back spell data spellName:" + sItem.value("spellName", ""), LogLevel::Info);

                    s.spellName = spellName_w;
                    s.spellType = sItem.value("spellType", 0);
                    Logger::Log("(LoadEnemyDataFromJson)\tpush back spell data spellType:" + to_string(s.spellType), LogLevel::Info);
                    s.isDurability = sItem.value("isDurability", 0);
                    Logger::Log("(LoadEnemyDataFromJson)\tpush back spell data isDurability:" + to_string(s.isDurability), LogLevel::Info);
                    s.durabilityTime = sItem.value("durabilityTime", 0);
                    Logger::Log("(LoadEnemyDataFromJson)\tpush back spell data durabilityTime:" + to_string(s.durabilityTime), LogLevel::Info);
                    spells.push_back(s);
                }
            }
        }
        printfDx(L"[INFO] spellCount = %d, spells.size() = %d\n", e.spellCount, (int)spells.size());
    }
    printfDx(L"[DEBUG] Loaded %d enemies\n", (int)outEnemies.size());
    if (outEnemies.empty())
    {
        printfDx(L"[ERROR] No enemies loaded from stage data\n");
    }
}

bool LoadTalkDataFromJson(const std::string &path, int stageIndex, std::vector<TalkData> &outTalks)
{
    outTalks.clear();
    std::ifstream ifs(path);
    if (!ifs.is_open())
    {
        std::cerr << "[ERROR] cannot open file " << path << std::endl;
        Logger::Log("(LoadTalkDataFromJson)\tcannot open file" + path, LogLevel::Error);
        return false;
    }
    json j;
    ifs >> j;
    int idx = stageIndex - 1;
    if (!j.contains("stages") || !j["stages"].is_array() || idx >= j["stages"].size())
    {
        std::cerr << "[ERROR] talk data for stage " << idx << " not found\n";
        Logger::Log("(LoadTalkDataFromJson)\ttalk data for stage " + to_string(idx) + " not found\n", LogLevel::Error);
        return false;
    }
    json talksData = j["stages"][idx];
    if (!talksData.contains("talks") || !talksData["talks"].is_array())
    {
        std::cerr << "[WARN] \"talks\" array is missing in stage " << idx << "\n";
        Logger::Log("(LoadTalkDataFromJson)\t\"talks\" array is missing in stage" + to_string(stageIndex), LogLevel::Error);
        return false;
    }
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    for (auto &item : talksData["talks"])
    {
        TalkData t{};

        t.talkString = converter.from_bytes(item.value("talkStr", ""));
        t.isTalkEnemy = item.value("isTalkEnemy", 0);
        Logger::Log("(LoadTalkDataFromJson) \t" + item.value("talkStr", ""), LogLevel::Info);
        outTalks.push_back(t);
    }
    std::cout << "[INFO] " << outTalks.size() << " talk lines loaded\n";
    return true;
}

// J敵の画像データ取得
// inline bool LoadEnemyImage(const std::string &path, int enemyImageHandle[32])
// {
//     enemyImageHandle[0] = LoadGraph(L"../../img/Enemy.png");
//     return true;
// }
