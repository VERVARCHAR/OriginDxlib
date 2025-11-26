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

// JSONから敵情報を読み取る関数
inline bool LoadEnemyDataFromJson(const std::string &stageInfoPath, const std::string &bossInfoPath, std::vector<EnemyStatus> &outEnemies, std::vector<SpellInfo> &spells)
{
    outEnemies.clear();
    std::ifstream stageInfoFile(stageInfoPath);
    if (!stageInfoFile.is_open())
    {
        std::cerr << L"[ERROR] cant open file " << stageInfoPath << std::endl;
        return false;
    }

    std::ifstream bossInfoFile(bossInfoPath);
    if (!bossInfoFile.is_open())
    {
        std::cerr << L"[ERROR] cant open file " << bossInfoPath << std::endl;
        return false;
    }

    json j1, j2;
    try
    {
        stageInfoFile >> j1;
    }
    catch (const std::exception &e)
    {
        std::cerr << L"[ERROR] JSON Parsing Error:" << e.what() << std::endl;
        return false;
    }

    try
    {
        bossInfoFile >> j2;
    }
    catch (const std::exception &e)
    {
        std::cerr << L"[ERROR] JSON Parsing Error:" << e.what() << std::endl;
        return false;
    }

    if (!j1.contains("enemies") || !j1["enemies"].is_array())
    {
        std::cerr << L"[WARN] is not exist array \"enemies\"\n";
        return false;
    }

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    for (const auto &item : j1["enemies"])
    {
        EnemyStatus e{};
        e.pos.x = item.value("pos", json{{"x", 0.0}, {"y", 0.0}})["x"];
        e.pos.y = item.value("pos", json{{"x", 0.0}, {"y", 0.0}})["y"];
        e.vel.x = item.value("vel", json{{"x", 0.0}, {"y", 0.0}})["x"];
        e.vel.y = item.value("vel", json{{"x", 0.0}, {"y", 0.0}})["y"];
        e.type = item.value("type", 0);
        e.lives = item.value("lives", 1);
        e.maxHp = e.hp = item.value("hp", 10);
        e.radius = item.value("radius", 8);
        e.shootType = item.value("shootType", 0);
        e.spwanTime = item.value("spwanTime", 0); // JSONに無くてもOK
        e.time = item.value("time", 0);
        e.id = static_cast<short>(item.value("id", 0));
        e.isAlive = item.value("isAlive", 0);

        std::string name_str = item.value("name", "");
        std::wstring name_w = converter.from_bytes(name_str);
        e.name = name_w;
        e.spellCount = 0;

        // TODO type > 100ならスペル情報や会話内容を取得したい
        outEnemies.push_back(e);

        if (e.id >= 100)
        {

            for (const auto &sItems : j2["spells"])
            {
                SpellInfo s{};
                std::string spellName_str = sItems.value("spellName", "");
                std::wstring spellName_w = converter.from_bytes(spellName_str);

                s.spellName = spellName_w;
                s.spellType = sItems.value("spellType", 0);
                s.isDurability = sItems.value("isDurability", 0);
                s.durabilityTime = sItems.value("durabilityTime", 0);
                spells.push_back(s);
            }
        }
    }

    std::cout << L"[INFO] " << outEnemies.size() << L"loaded enemies\n";

    return true;
}

inline bool LoadTalkDataFromJson(const std::string &path, std::vector<TalkData> &talkData)
{
    talkData.clear();
    std::ifstream ifs(path);
    if (!ifs.is_open())
    {
        std::cerr << L"[ERROR] cant open file " << path << std::endl;
        return false;
    }

    json j;
    try
    {
        ifs >> j;
    }
    catch (const std::exception &e)
    {
        std::cerr << L"[ERROR] JSON Parsing Error:" << e.what() << std::endl;
        return false;
    }

    if (!j.contains("talks") || !j["talks"].is_array())
    {
        std::cerr << L"[WARN] is not exist array \"enemies\"\n";
        return false;
    }

    for (const auto &item : j["talks"])
    {
        TalkData t{};
        t.talkString = item.value("talkStr", "");

        // t.talkString = item.value("pos", json{{"x", 0.0}, {"y", 0.0}})["x"];
        t.isTalkEnemy = item.value("isTalkEnemy", 0);

        // TODO type > 100ならスペル情報や会話内容を取得したい
        talkData.push_back(t);
    }

    std::cout << L"[INFO] " << talkData.size() << L"loaded enemies\n";

    return true;
}

// J敵の画像データ取得
// inline bool LoadEnemyImage(const std::string &path, int enemyImageHandle[32])
// {
//     enemyImageHandle[0] = LoadGraph(L"../../img/Enemy.png");
//     return true;
// }
