#ifndef _ITEMS_HPP_
#define _ITEMS_HPP_
#include "object/item.hpp"
#endif

#include <cmath>

double calcSquaredDistance(Vec2d pos1, Vec2d pos2)
{
    double dx = pos1.x - pos2.x;
    double dy = pos1.y - pos2.y;
    return dx * dx + dy * dy;
}

static inline uint32_t hash32(uint32_t x)
{
    // SplitMix32系の軽い混ぜ
    x += 0x9e3779b9u;
    x ^= x >> 16;
    x *= 0x85ebca6bu;
    x ^= x >> 13;
    x *= 0xc2b2ae35u;
    x ^= x >> 16;
    return x;
}

static inline uint32_t makeDropSeed(int time, int itemType)
{
    uint32_t s = 0x332;
    s ^= hash32(0x512);
    s ^= hash32((uint32_t)time);
    s ^= hash32((uint32_t)itemType);
    return hash32(s);
}

ItemManager::ItemManager()
{
    init();
}
ItemManager::~ItemManager()
{
}

void ItemManager::init()
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        items[i].isActive = false;
        items[i].pos = {-100, -100};
        items[i].vel = {0, 0};
        items[i].itemType = ItemType::NONE;
        items[i].radius = -1;
        items[i].value = -1;
    }
}

void ItemManager::loadImagehandle(int _livesImageHandle, int _spellImageHandle)
{
    // livesImageHandle = _livesImageHandle;
    // spellImageHandle = _spellImageHandle;
    // TODO
    // scoreImageHandle = LoadGraph();
    // powerImageHandle = LoadGraph();
    LoadDivGraph(L"..\\..\\assets\\others\\items.png", 4, 4, 1, 16, 16, itemImageHandle);
}

int ItemManager::getEmptyIndex()
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (!items[i].isActive)
        {
            return i;
        }
    }
    return -1;
}

void ItemManager::updateItems(StageManager *sMgr, Player *player)
{
    Vec2d playerPos = player->getPosition();
    double dist = 0;
    double pullPower = 0;

    bool allGetFlag = borderGetAllItem(player);

    for (int i = 0; i < MAX_ITEMS; i++)
    {

        if (items[i].isActive)
        {
            dist = calcSquaredDistance(playerPos, items[i].pos);
            double radius = static_cast<double>(items[i].radius);
            double pullRadius = (radius + 30.0);
            if (allGetFlag)
            {
                pullPower = 15;
                items[i].vel.x = pullPower * (playerPos.x - items[i].pos.x) / sqrt(dist);
                items[i].vel.y = pullPower * (playerPos.y - items[i].pos.y) / sqrt(dist);
            }
            else
            {
                pullPower = 10;
                //[DEBUG]
                // printfDx(L"Item : %d is Active\n", i);
            }

            items[i].pos.x += items[i].vel.x;
            items[i].pos.y += items[i].vel.y;

            if (items[i].pos.y > WINDOW_HEIGHT + 10 || items[i].pos.x < 5 || items[i].pos.x > (int)(WINDOW_WIDTH * 0.6) + 22)
            {
                items[i].isActive = false;
            }

            if (dist < pullRadius * pullRadius)
            {
                items[i].vel.x = pullPower * (playerPos.x - items[i].pos.x) / sqrt(dist);
                items[i].vel.y = pullPower * (playerPos.y - items[i].pos.y) / sqrt(dist);
                if (dist < radius * radius)
                {
                    items[i].isActive = false;

                    switch (items[i].itemType)
                    {
                    case ItemType::SCORE:
                        sMgr->setScore(items[i].value);
                        break;
                    case ItemType::POWER:
                        player->setPower(items[i].value);
                        break;
                    case ItemType::EXTEND:
                        player->setExtend();
                        break;
                    case ItemType::SPELL:
                        player->setSpell();
                        break;

                    default:
                        break;
                    }
                }
            }
        }
    }
}

void ItemManager::drawItems()
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (items[i].isActive)
        {
            // switch (items[i].itemType)
            // {
            // case ItemType::NONE:
            //     DrawBox(items[i].pos.x - 5, items[i].pos.y - 10, items[i].pos.x + 5, items[i].pos.y + 10, GetColor(0, 0, 0), TRUE);
            //     break;
            // case ItemType::SCORE:
            //     DrawBox(items[i].pos.x - 5, items[i].pos.y - 10, items[i].pos.x + 5, items[i].pos.y + 10, GetColor(0, 0, 255), TRUE);
            //     break;
            // case ItemType::POWER:
            //     DrawBox(items[i].pos.x - 5, items[i].pos.y - 10, items[i].pos.x + 5, items[i].pos.y + 10, GetColor(255, 0, 0), TRUE);
            //     break;
            // case ItemType::EXTEND:
            //     DrawExtendGraph(items[i].pos.x - 25, items[i].pos.y - 25, items[i].pos.x + 25, items[i].pos.y + 25, livesImageHandle, TRUE);
            //     break;
            // case ItemType::SPELL:
            //     DrawExtendGraph(items[i].pos.x - 25, items[i].pos.y - 25, items[i].pos.x + 25, items[i].pos.y + 25, spellImageHandle, TRUE);
            //     break;
            // default:
            //     break;
            // }
            DrawGraph(items[i].pos.x, items[i].pos.y, itemImageHandle[static_cast<int>(items[i].itemType)], TRUE);
            // DrawCircle(items[i].pos.x, items[i].pos.y, items[i].radius, GetColor(0, 255, 0), TRUE);
            // DrawCircle(items[i].pos.x, items[i].pos.y, items[i].radius + 30, GetColor(0, 0, 255), FALSE);
            // DrawLine(items[i].pos.x, items[i].pos.y, items[i].pos.x + items[i].vel.x, items[i].pos.y + items[i].vel.y, GetColor(255, 0, 255));
        }
    }
}

void ItemManager::spawnItem(ItemType _itemType, Vec2d _pos, Vec2d _vel, int amount, int time)
{
    const uint32_t seed = makeDropSeed(time, static_cast<int>(_itemType));
    std::mt19937 rng(seed);

    // 散らばり半径（好みで調整）
    std::uniform_real_distribution<double> angleDist(0.0, 2.0 * 3.14);
    std::uniform_real_distribution<double> radiusDist(0.0, 1.0); // 後でsqrtして面積一様にする

    for (int i = 0; i < amount; i++)
    {
        int id = getEmptyIndex();
        items[id].isActive = true;

        // 面積一様：r = R * sqrt(u)
        double a = angleDist(rng);
        double r = 18.0 * std::sqrt(radiusDist(rng)); // 18.0は散らばり量
        Vec2d offset{r * std::cos(a), r * std::sin(a)};

        // 少しだけ上に寄せる、とかも決定論的に可能
        offset.y -= 6.0;

        items[id].pos = {_pos.x + offset.x, _pos.y + offset.y};
        items[id].vel = _vel;
        items[id].itemType = _itemType;
        items[id].radius = 8;

        switch (_itemType)
        {
        case ItemType::SCORE:
            items[id].value = 10;
            break;
        case ItemType::POWER:
            items[id].value = 0.01;
            break;
        case ItemType::EXTEND:
            items[id].value = 1;
            break;
        case ItemType::SPELL:
            items[id].value = 1;
            break;
        default:
            break;
        }
    }
}

bool ItemManager::borderGetAllItem(Player *player)
{
    if (player->getPosition().y < 150)
    {
        return true;
    }
    return false;
}