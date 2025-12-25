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

#ifndef _STAGE_HPP_
#define _STAGE_HPP_
#include "object/stage.hpp"
#endif

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>
#include "nlohmann/json.hpp" // nlohmann/json
using json = nlohmann::json;

class UI
{
private:
    int preLoadingHandle;
    int loadingHandle;
    int loadingSakuraHandle[12];
    int titleHandle;
    int UIImageHandle;
    int lifeImageHandle;
    int spellImageHandle;
    int difficultyImageHandle[4];
    int logoHandle;
    int gameImageHandle[7];
    int stageImageHandle[7];
    int playerImageHandle;
    int bossCutInImageHandle;
    int uiFont;
    int talkFont;
    int highScore;

public:
    UI();
    ~UI();
    int minLoadingTime;
    int bombsImageHandle[32];
    int time = 0;

    void
    startLoading()
    {
        minLoadingTime = 0;
    }
    void getImage();
    void getInGameImage();
    void loadPreLoadingImages();
    void drawPreLoading();
    void drawTitle();
    void drawUI(StageInfo stageInfo);
    void drawBossStatus(EnemyStatus enemyStatus);
    void drawSpellCardText(EnemyStatus enemyStatus, SpellInfo spellInfo, int time);
    void loadingScreen();
    void talkUI(std::wstring, int);
    void drawBorderAllGetItem();
    PlayerStatus getPlayerStatus();
    EnemyStatus getEnemyStatus();

    int getLifeImageHandle()
    {
        return lifeImageHandle;
    }
    int getSpellImageHandle()
    {
        return spellImageHandle;
    }

    int getEnemyCutInHandle()
    {
        return bossCutInImageHandle;
    }
};

void ShowLoadingTest();