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
    livesImageHandle = _livesImageHandle;
    spellImageHandle = _spellImageHandle;
    // TODO
    //  scoreImageHandle = LoadGraph();
    //  powerImageHandle = LoadGraph();
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
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (items[i].isActive)
        {
            //[DEBUG]
            printfDx(L"Item : %d is Active\n", i);
            items[i].pos.x += items[i].vel.x;
            items[i].pos.y += items[i].vel.y;
            if (items[i].pos.y > WINDOW_HEIGHT + 10)
            {
                items[i].isActive = false;
            }
            dist = calcSquaredDistance(playerPos, items[i].pos);
            double radius = static_cast<double>(items[i].radius);
            double pullRadius = (radius + 30.0);

            if (dist < pullRadius * pullRadius)
            {
                items[i].vel.x = 2 * (playerPos.x - items[i].pos.x) / sqrt(dist);
                items[i].vel.y = 2 * (playerPos.y - items[i].pos.y) / sqrt(dist);
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
            switch (items[i].itemType)
            {
            case ItemType::NONE:
                DrawBox(items[i].pos.x - 5, items[i].pos.y - 10, items[i].pos.x + 5, items[i].pos.y + 10, GetColor(0, 0, 0), TRUE);
                break;
            case ItemType::SCORE:
                DrawBox(items[i].pos.x - 5, items[i].pos.y - 10, items[i].pos.x + 5, items[i].pos.y + 10, GetColor(0, 0, 255), TRUE);
                break;
            case ItemType::POWER:
                DrawBox(items[i].pos.x - 5, items[i].pos.y - 10, items[i].pos.x + 5, items[i].pos.y + 10, GetColor(255, 0, 0), TRUE);
                break;
            case ItemType::EXTEND:
                DrawExtendGraph(items[i].pos.x - 25, items[i].pos.y - 25, items[i].pos.x + 25, items[i].pos.y + 25, livesImageHandle, TRUE);
                break;
            case ItemType::SPELL:
                DrawExtendGraph(items[i].pos.x - 25, items[i].pos.y - 25, items[i].pos.x + 25, items[i].pos.y + 25, spellImageHandle, TRUE);
                break;
            default:
                break;
            }

            DrawCircle(items[i].pos.x, items[i].pos.y, items[i].radius, GetColor(0, 255, 0), TRUE);
            DrawCircle(items[i].pos.x, items[i].pos.y, items[i].radius + 30, GetColor(0, 0, 255), FALSE);
            DrawLine(items[i].pos.x, items[i].pos.y, items[i].pos.x + items[i].vel.x, items[i].pos.y + items[i].vel.y, GetColor(255, 0, 255));
        }
    }
}

void ItemManager::spawnItem(ItemType _itemType, Vec2d _pos, Vec2d _vel)
{
    int id = getEmptyIndex();
    items[id].isActive = true;
    items[id].pos = _pos;
    items[id].vel = _vel;
    items[id].itemType = _itemType;
    items[id].radius = 5;
    items[id].value = 10;
}
