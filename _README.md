# DXLib + VSCode + CMake Template

## 前提
- VSCode
- CMake (3.25+)
- Visual Studio Build Tools 2022 (MSVC/Windows SDK)
- DXLib をローカルに展開（例: `C:/Dev/DXLib`）

## 使い方
1. `.vscode/settings.json` の `DXLIB_DIR` を自分の環境に合わせて変更
2. VSCode コマンドパレット → **CMake: Configure**
3. **Build** → `build/bin/DxTemplate.exe` が生成
4. **Run and Debug** で起動（`launch.json`）

## よくある調整
- 32bit を使う場合: `DXLIB_ARCH` を `x86` にする
- Visual Studio ジェネレータを使う: `cmake.generator` を `"Visual Studio 17 2022"` に変更
