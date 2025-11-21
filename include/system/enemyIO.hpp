// enemyIO.hpp
#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>
#include "nlohmann/json.hpp" // nlohmann/json
using json = nlohmann::json;

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
inline bool LoadEnemyDataFromJson(const std::string &path, std::vector<EnemyStatus> &outEnemies)
{
    outEnemies.clear();
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

    if (!j.contains("enemies") || !j["enemies"].is_array())
    {
        std::cerr << L"[WARN] is not exist array \"enemies\"\n";
        return false;
    }

    for (const auto &item : j["enemies"])
    {
        EnemyStatus e{};
        e.pos.x = item.value("pos", json{{"x", 0.0}, {"y", 0.0}})["x"];
        e.pos.y = item.value("pos", json{{"x", 0.0}, {"y", 0.0}})["y"];
        e.vel.x = item.value("vel", json{{"x", 0.0}, {"y", 0.0}})["x"];
        e.vel.y = item.value("vel", json{{"x", 0.0}, {"y", 0.0}})["y"];
        e.type = item.value("type", 0);
        e.lives = item.value("lives", 1);
        e.hp = item.value("hp", 10);
        e.radius = item.value("radius", 8);
        e.shootType = item.value("shootType", 0);
        e.spwanTime = item.value("spwanTime", 0); // JSONに無くてもOK
        e.time = item.value("time", 0);
        e.id = static_cast<short>(item.value("id", 0));
        e.isAlive = item.value("isAlive", 0);

        std::string name = item.value("name", "");
        std::strncpy(e.name, name.c_str(), sizeof(e.name) - 1);
        e.name[sizeof(e.name) - 1] = '\0';

        // TODO type > 100ならスペル情報や会話内容を取得したい
        outEnemies.push_back(e);
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
        std::string str = item.value("talkStr", "");
        std::strncpy(t.talkString, str.c_str(), sizeof(t.talkString) - 1);
        e.name[sizeof(t.talkString) - 1] = '\0';

        // t.talkString = item.value("pos", json{{"x", 0.0}, {"y", 0.0}})["x"];
        t.isTalkEnemy = item.value("isTalkEnemy", 0);

        // TODO type > 100ならスペル情報や会話内容を取得したい
        talkData.push_back(e);
    }

    std::cout << L"[INFO] " << talkData.size() << L"loaded enemies\n";

    return true;
}

// J敵の画像データ取得
inline bool LoadEnemyImage(const std::string &path, int enemyImageHandle[32])
{

    return true;
}
