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

class UI
{
private:
    int titleHandle;
    int UIImageHandle;
    int lifeImageHandle;
    int spellImageHandle;
    int difficultyImageHandle[4];
    int logoHandle;
    int gameImageHandle[7];

public:
    UI();
    ~UI();
    int minLoadingTime;
    int bombsImageHandle[32];

    void startLoading()
    {
        minLoadingTime = 0;
    }
    void getImage();
    void getInGameImage();
    void loadTitleImages();
    void drawUI(StageInfo stageInfo);
    void loadingScreen();
    void talkUI(string, int);
    PlayerStatus getPlayerStatus();
    EnemyStatus getEnemyStatus();
};

void ShowLoadingTest();