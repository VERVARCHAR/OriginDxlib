#pragma once

#ifndef _DXLIB_HPP_
#define _DXLIB_HPP_
#include "DxLib.h"
#endif // _DXLIB_HPP_

#ifndef _STAGE_HPP_
#define _STAGE_HPP_
#include "object/stage.hpp"
#endif

#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_MANAGER_HPP_

#ifndef _ENEMY_SHOOT_SCRIPT_01_HPP_
#define _ENEMY_SHOOT_SCRIPT_01_HPP_
#include "object/enemyShootScripts/EnemyShootScripts.hpp"
#endif // _ENEMY_SHOOT_SCRIPT_01_HPP_

#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_
#include "object/player.hpp"
#endif // _PLAYER_HPP_

#ifndef _UTILS_HPP_
#define _UTILS_HPP_
#include "system/utils.hpp"
#endif

#ifndef _EFFECTER_HPP_
#define _EFFECTER_HPP_
#include "system/effecter.hpp"
#endif

enum class ItemType
{
    NONE,
    SCORE,
    POWER,
    EXTEND,
    SPELL,
};

typedef struct Items
{
    ItemType itemType;
    Vec2d pos, vel;
    bool isActive;
    int radius;
    int value;
};

#define MAX_ITEMS 1000

class ItemManager
{
private:
    int scoreImageHandle;
    int powerImageHandle;
    int livesImageHandle;
    int spellImageHandle;
    Items items[MAX_ITEMS];

public:
    ItemManager();
    ~ItemManager();

    void init();
    int getEmptyIndex();
    void updateItems(StageManager *sMgr, Player *player);
    void drawItems();
    void loadImagehandle(int _livesImageHandle, int _spellImageHandle);
    void spawnItem(ItemType _itemType, Vec2d _pos, Vec2d _vel);
};
