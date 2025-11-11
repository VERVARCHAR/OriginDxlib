cmake_minimum_required(VERSION 3.25)
project(ORIGIN LANGUAGES CXX)

# ------------------------------
# 基本設定
# ------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DUNICODE -D_UNICODE)

# ------------------------------
# DxLib のルート検出
#   - 推奨: DXLIB_ROOT=C:/DxLib_VC を指定
#   - 互換: DXLIB_DIR(旧) でも受け付ける
#     例:
#       -DDXLIB_ROOT=C:/DxLib_VC
#       もしくは
#       -DDXLIB_DIR=C:/DxLib_VC
#       -DDXLIB_DIR=C:/DxLib_VC/libs （古い指定にも対応）
# ------------------------------
if(NOT DEFINED DXLIB_ROOT)
  if(DEFINED DXLIB_DIR)
    set(DXLIB_ROOT "${DXLIB_DIR}")
  elseif(DEFINED ENV{DXLIB_ROOT})
    set(DXLIB_ROOT "$ENV{DXLIB_ROOT}")
  elseif(DEFINED ENV{DXLIB_DIR})
    set(DXLIB_ROOT "$ENV{DXLIB_DIR}")
  else()
    message(FATAL_ERROR
      "DxLib の場所が未指定です。以下のいずれかを指定してください。\n"
      "  -DDXLIB_ROOT=C:/DxLib_VC   （推奨）\n"
      "  または既存互換: -DDXLIB_DIR=C:/DxLib_VC / C:/DxLib_VC/libs")
  endif()
endif()

# include と libs の両候補を解決（DxLib の配置差に対応）
set(_DXLIB_CAND_INC
  "${DXLIB_ROOT}/include"       # 推奨配置: C:/DxLib_VC/include
  "${DXLIB_ROOT}"               # 旧: DXLIB_ROOT が include 直指し
)
set(_DXLIB_CAND_LIB
  "${DXLIB_ROOT}/libs"          # 推奨配置: C:/DxLib_VC/libs
  "${DXLIB_ROOT}"               # 旧: DXLIB_ROOT が libs 直指し
)

set(DXLIB_INC_DIR "")
foreach(p IN LISTS _DXLIB_CAND_INC)
  if(EXISTS "${p}/DxLib.h")
    set(DXLIB_INC_DIR "${p}")
    break()
  endif()
endforeach()
if(NOT DXLIB_INC_DIR)
  message(FATAL_ERROR "DxLib.h が見つかりません。DXLIB_ROOT=${DXLIB_ROOT}\n"
                      "想定場所: ${DXLIB_ROOT}/include/DxLib.h または ${DXLIB_ROOT}/DxLib.h")
endif()

set(DXLIB_LIB_DIR "")
foreach(p IN LISTS _DXLIB_CAND_LIB)
  # どれか1つでも *.lib がある場所を libs とみなす
  if(EXISTS "${p}")
    set(DXLIB_LIB_DIR "${p}")
    break()
  endif()
endforeach()
if(NOT DXLIB_LIB_DIR)
  message(FATAL_ERROR "DxLib のライブラリディレクトリが見つかりません。DXLIB_ROOT=${DXLIB_ROOT}")
endif()

# ------------------------------
# /MT or /MD 切替（既定 /MT・/MTd）
# ------------------------------
option(DXLIB_USE_MD "Use /MD(/MDd) instead of /MT(/MTd)" OFF)
set(_rt "MT")
if(DXLIB_USE_MD)
  set(_rt "MD")
endif()

# ------------------------------
# DxLib のライブラリ（VS2015 + x64 系の命名に合わせた既存指定を維持）
#   ※ 実在するもののみリンクする（下の _filter_existing で除外）
# ------------------------------
set(DX_RELEASE_CORE
  "${DXLIB_LIB_DIR}/DxLibW_vs2015_x64_${_rt}.lib"
  "${DXLIB_LIB_DIR}/DxDrawFunc_vs2015_x64_${_rt}.lib"
  "${DXLIB_LIB_DIR}/DxUseCLibW_vs2015_x64_${_rt}.lib"
)
set(DX_DEBUG_CORE
  "${DXLIB_LIB_DIR}/DxLibW_vs2015_x64_${_rt}d.lib"
  "${DXLIB_LIB_DIR}/DxDrawFunc_vs2015_x64_${_rt}d.lib"
  "${DXLIB_LIB_DIR}/DxUseCLibW_vs2015_x64_${_rt}d.lib"
)

# 画像系（theora も含める）
set(DX_RELEASE_IMG
  "${DXLIB_LIB_DIR}/zlib_vs2015_x64_${_rt}.lib"
  "${DXLIB_LIB_DIR}/libpng_vs2015_x64_${_rt}.lib"
  "${DXLIB_LIB_DIR}/libjpeg_vs2015_x64_${_rt}.lib"
  "${DXLIB_LIB_DIR}/libtiff_vs2015_x64_${_rt}.lib"
  "${DXLIB_LIB_DIR}/libtheora_static_vs2015_x64_${_rt}.lib"
)
set(DX_DEBUG_IMG
  "${DXLIB_LIB_DIR}/zlib_vs2015_x64_${_rt}d.lib"
  "${DXLIB_LIB_DIR}/libpng_vs2015_x64_${_rt}d.lib"
  "${DXLIB_LIB_DIR}/libjpeg_vs2015_x64_${_rt}d.lib"
  "${DXLIB_LIB_DIR}/libtiff_vs2015_x64_${_rt}d.lib"
  "${DXLIB_LIB_DIR}/libtheora_static_vs2015_x64_${_rt}d.lib"
)

# 音声系
set(DX_RELEASE_AUDIO
  "${DXLIB_LIB_DIR}/ogg_static_vs2015_x64_${_rt}.lib"
  "${DXLIB_LIB_DIR}/vorbis_static_vs2015_x64_${_rt}.lib"
  "${DXLIB_LIB_DIR}/vorbisfile_static_vs2015_x64_${_rt}.lib"
  "${DXLIB_LIB_DIR}/opus_vs2015_x64_${_rt}.lib"
  "${DXLIB_LIB_DIR}/opusfile_vs2015_x64_${_rt}.lib"
  "${DXLIB_LIB_DIR}/celt_vs2015_x64_${_rt}.lib"
  "${DXLIB_LIB_DIR}/silk_common_vs2015_x64_${_rt}.lib"
)
set(DX_DEBUG_AUDIO
  "${DXLIB_LIB_DIR}/ogg_static_vs2015_x64_${_rt}d.lib"
  "${DXLIB_LIB_DIR}/vorbis_static_vs2015_x64_${_rt}d.lib"
  "${DXLIB_LIB_DIR}/vorbisfile_static_vs2015_x64_${_rt}d.lib"
  "${DXLIB_LIB_DIR}/opus_vs2015_x64_${_rt}d.lib"
  "${DXLIB_LIB_DIR}/opusfile_vs2015_x64_${_rt}d.lib"
  "${DXLIB_LIB_DIR}/celt_vs2015_x64_${_rt}d.lib"
  "${DXLIB_LIB_DIR}/silk_common_vs2015_x64_${_rt}d.lib"
)

# 物理（未使用なら空のままでOK）
set(DX_RELEASE_BULLET
  "${DXLIB_LIB_DIR}/libbulletcollision_vs2015_x64_${_rt}.lib"
  "${DXLIB_LIB_DIR}/libbulletdynamics_vs2015_x64_${_rt}.lib"
  "${DXLIB_LIB_DIR}/libbulletmath_vs2015_x64_${_rt}.lib"
)
set(DX_DEBUG_BULLET
  "${DXLIB_LIB_DIR}/libbulletcollision_vs2015_x64_${_rt}d.lib"
  "${DXLIB_LIB_DIR}/libbulletdynamics_vs2015_x64_${_rt}d.lib"
  "${DXLIB_LIB_DIR}/libbulletmath_vs2015_x64_${_rt}d.lib"
)

# 実在チェック → 無いものは除外
function(_filter_existing out_var)
  set(_out "")
  foreach(_f IN LISTS ARGN)
    if(EXISTS "${_f}")
      list(APPEND _out "${_f}")
    endif()
  endforeach()
  set(${out_var} "${_out}" PARENT_SCOPE)
endfunction()
_filter_existing(DX_RELEASE_LIBS ${DX_RELEASE_CORE} ${DX_RELEASE_IMG} ${DX_RELEASE_AUDIO} ${DX_RELEASE_BULLET})
_filter_existing(DX_DEBUG_LIBS   ${DX_DEBUG_CORE}   ${DX_DEBUG_IMG}   ${DX_DEBUG_AUDIO}   ${DX_DEBUG_BULLET})

if(NOT DX_RELEASE_LIBS AND NOT DX_DEBUG_LIBS)
  message(FATAL_ERROR
    "DxLib のライブラリが見つかりません。\n"
    "DXLIB_LIB_DIR=${DXLIB_LIB_DIR}\n"
    "指定ルート（DXLIB_ROOT）の見直しをお願いします。")
endif()

# ------------------------------
# 依存: nlohmann/json（単一ヘッダ・FetchContent）
# ------------------------------
include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

# ------------------------------
# ソース／ヘッダの収集
# ------------------------------
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/*.cpp
  ${CMAKE_SOURCE_DIR}/src/*.c
)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/*.h
  ${CMAKE_SOURCE_DIR}/src/*.hpp
  ${CMAKE_SOURCE_DIR}/include/*.h
  ${CMAKE_SOURCE_DIR}/include/*.hpp
)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

# ------------------------------
# include パス（※ DxLib は「INC_DIR」を入れるのが正解！）
# ------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  "${DXLIB_INC_DIR}"
)

# ------------------------------
# ライブラリリンク
# ------------------------------
# 構成別に DxLib 群
if(DX_DEBUG_LIBS)
  target_link_libraries(${PROJECT_NAME} "$<$<CONFIG:Debug>:${DX_DEBUG_LIBS}>")
endif()
if(DX_RELEASE_LIBS)
  target_link_libraries(${PROJECT_NAME} "$<$<NOT:$<CONFIG:Debug>>:${DX_RELEASE_LIBS}>")
endif()

# Win32 標準ライブラリ
target_link_libraries(${PROJECT_NAME}
  nlohmann_json::nlohmann_json           # ← JSON をリンク（ヘッダオンリー）
  dinput8 dxguid winmm imm32 version comctl32 ole32 oleaut32 setupapi gdi32 user32 shell32 advapi32
)

# 出力先
set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# MSVC ランタイム / 警告など
if(MSVC)
  if(DXLIB_USE_MD)
    set(msvc_rt "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
  else()
    set(msvc_rt "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  endif()
  set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "${msvc_rt}")
  target_compile_options(${PROJECT_NAME} PRIVATE /utf-8 /W4 /permissive-)
  target_compile_options(${PROJECT_NAME} PRIVATE /wd4828) # DxLib.h の文字コード警告を黙らせる
  add_compile_options(/execution-charset:utf-8)
endif()

# 例: 特定ファイルだけ Shift_JIS を読む（必要なら）
# set_source_files_properties(dxlib_wrapper.cpp PROPERTIES COMPILE_OPTIONS "/source-charset:shift_jis")
