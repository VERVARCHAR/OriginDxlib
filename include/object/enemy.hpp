#pragma once

#ifndef _DXLIB_HPP_
#define _DXLIB_HPP_
#include "DxLib.h"
#endif // _DXLIB_HPP_

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

class EnemyShootScript;
class Player;

typedef struct EnemyStatus
{
    Vec2d pos;
    Vec2d vel;
    int type; // 種類
    int lives;
    int maxHp;
    int hp; // 体力
    int radius;
    int shootType;
    int spwanTime;
    int time;
    short id;
    bool isAlive;
    bool isSpell;
    int spellCount;
    int spellStartTime;
    bool isInvicible;
    int invicibleTime;
    std::wstring name; // 名前
};

typedef struct SpellInfo
{
    std::wstring spellName;
    int spellType;
    bool isDurability;
    int durabilityTime;
};

class Enemy
{
private:
    int imageHandle;

protected:
public:
    bool isTalking = false;
    Enemy(EnemyStatus enemyStatus);
    ~Enemy();

    void init(EnemyStatus _enemyStatus);

    void setPosition(int _x, int _y);
    Vec2d getPosition();
    EnemyStatus enemyStatus;
    std::vector<SpellInfo> spellInfo;
    EnemyStatus getStatus()
    {
        return enemyStatus;
    }
    SpellInfo getSpellInfo()
    {
        return spellInfo.at(0);
    }

    void setStatus(EnemyStatus _status)
    {
        enemyStatus = _status;
    }

    void setSpellData(SpellInfo _spellInfo)
    {
        spellInfo.push_back(_spellInfo);
    }

    int getTime() { return enemyStatus.time; };

    int getId() { return enemyStatus.id; };
    void setIsAlive(bool flag) { enemyStatus.isAlive = flag; };
    void getBMgrData(BombManager &_BombManager);
    void enemyDraw();
    void setImageHandle(int handle) { this->imageHandle = handle; };
    void enemyUpdate(int, int, BombManager *, BombInfo[MAX_BOMBS], EnemyShootScript, Player *player, Effecter *effecter);
    void shootBomb(EnemyShootScript, BombManager *bMgr, BombInfo bombs[MAX_BOMBS], int time, int dificulty, Player player);
    bool getOnScreen();
    bool getIsAlive(int index);
    bool isHitPlayer(Player *player);
    void enemyMove();
    Vec2d getPos(int index);
};