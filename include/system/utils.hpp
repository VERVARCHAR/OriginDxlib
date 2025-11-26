#pragma once

#ifndef _DXLIB_HPP_
#define _DXLIB_HPP_
#include "DxLib.h"
#endif // _DXLIB_HPP_

#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_MANAGER_HPP_

#ifndef _UTILS_HPP_
#define _UTILS_HPP_
#include "system/utils.hpp"
#endif

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>
#include <codecvt>
#include <locale>
#include "nlohmann/json.hpp" // nlohmann/json
using json = nlohmann::json;

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

void SetUp();
bool Update();
void Draw();
void Wait();
int UpdateKey();

// Function for Debuging
void Debug(int);