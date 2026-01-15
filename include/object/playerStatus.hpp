#pragma once

typedef struct Status
{
    int lives;
    int spells;
    double power;
    bool isSpells;
    bool invincible;
    bool isShoot;
    bool isShift;
    int invincibleTime;
    int grazeCount = 0;
} PlayerStatus;
