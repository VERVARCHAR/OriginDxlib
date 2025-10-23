cmake_minimum_required(VERSION 3.25)
project(ORIGIN LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DUNICODE -D_UNICODE)

# --- DXLib ルート ---
if(NOT DXLIB_DIR)
  if(DEFINED ENV{DXLIB_DIR})
    set(DXLIB_DIR "$ENV{DXLIB_DIR}")
  else()
    message(FATAL_ERROR "DXLIB_DIR を指定してください（例: -DDXLIB_DIR=C:/DxLib_VC/libs）")
  endif()
endif()

# ヘッダ確認
if(NOT EXISTS "${DXLIB_DIR}/DxLib.h")
  message(FATAL_ERROR "DxLib.h が見つかりません: ${DXLIB_DIR}/DxLib.h")
endif()

# ----/MT or /MD 切替（既定 /MT・/MTd）----
option(DXLIB_USE_MD "Use /MD(/MDd) instead of /MT(/MTd)" OFF)
set(_rt "MT")
if(DXLIB_USE_MD)
  set(_rt "MD")
endif()

# ---- VS2015 + x64 のみを使用 ----
# コア（Unicode 版を選択）
set(DX_RELEASE_CORE
  "${DXLIB_DIR}/DxLibW_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/DxDrawFunc_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/DxUseCLibW_vs2015_x64_${_rt}.lib"
)
set(DX_DEBUG_CORE
  "${DXLIB_DIR}/DxLibW_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/DxDrawFunc_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/DxUseCLibW_vs2015_x64_${_rt}d.lib"
)

# 画像系
set(DX_RELEASE_IMG
  "${DXLIB_DIR}/zlib_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/libpng_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/libjpeg_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/libtiff_vs2015_x64_${_rt}.lib"
)
set(DX_DEBUG_IMG
  "${DXLIB_DIR}/zlib_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/libpng_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/libjpeg_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/libtiff_vs2015_x64_${_rt}d.lib"
)

# 音声(OGG/Vorbis/Opus/CELT)
set(DX_RELEASE_AUDIO
  "${DXLIB_DIR}/ogg_static_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/vorbis_static_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/vorbisfile_static_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/opus_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/opusfile_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/celt_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/silk_common_vs2015_x64_${_rt}.lib"
)
set(DX_DEBUG_AUDIO
  "${DXLIB_DIR}/ogg_static_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/vorbis_static_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/vorbisfile_static_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/opus_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/opusfile_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/celt_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/silk_common_vs2015_x64_${_rt}d.lib"
)

# 物理(Bullet) — 使わないなら丸ごと削ってOK
set(DX_RELEASE_BULLET
  "${DXLIB_DIR}/libbulletcollision_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/libbulletdynamics_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/libbulletmath_vs2015_x64_${_rt}.lib"
)
set(DX_DEBUG_BULLET
  "${DXLIB_DIR}/libbulletcollision_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/libbulletdynamics_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/libbulletmath_vs2015_x64_${_rt}d.lib"
)

# 画像系（★ theora を追加）
set(DX_RELEASE_IMG
  "${DXLIB_DIR}/zlib_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/libpng_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/libjpeg_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/libtiff_vs2015_x64_${_rt}.lib"
  "${DXLIB_DIR}/libtheora_static_vs2015_x64_${_rt}.lib"   # ←これを追加
)
set(DX_DEBUG_IMG
  "${DXLIB_DIR}/zlib_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/libpng_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/libjpeg_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/libtiff_vs2015_x64_${_rt}d.lib"
  "${DXLIB_DIR}/libtheora_static_vs2015_x64_${_rt}d.lib"  # ←これを追加
)


# 実在チェック（Release/Debug 共通で持っていないものは自動で除外）
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
  message(FATAL_ERROR "VS2015 x64 ${_rt} 系のライブラリが見つかりません。DXLIB_DIR=${DXLIB_DIR} を確認してください。")
endif()

# --- ソース自動収集 ---
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.c
)

# --- ヘッダ（IDE用表示、必須ではない） ---
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.h
    ${CMAKE_SOURCE_DIR}/src/*.hpp
    ${CMAKE_SOURCE_DIR}/include/*.h
)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})


# ---- 実行ファイル ----
# add_executable(${PROJECT_NAME} WIN32 src/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE "${DXLIB_DIR}")

# 構成ごとにリンク
if(DX_DEBUG_LIBS)
  target_link_libraries(${PROJECT_NAME} "$<$<CONFIG:Debug>:${DX_DEBUG_LIBS}>")
endif()
if(DX_RELEASE_LIBS)
  target_link_libraries(${PROJECT_NAME} "$<$<NOT:$<CONFIG:Debug>>:${DX_RELEASE_LIBS}>")
endif()

# Win32 標準ライブラリ
target_link_libraries(${PROJECT_NAME}
  dinput8 dxguid winmm imm32 version comctl32 ole32 oleaut32 setupapi gdi32 user32 shell32 advapi32
)

# 出力先
set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# MSVC ランタイムを /MT or /MD に合わせる
if(MSVC)
  if(DXLIB_USE_MD)
    set(msvc_rt "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
  else()
    set(msvc_rt "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  endif()
  set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "${msvc_rt}")
  target_compile_options(${PROJECT_NAME} PRIVATE /utf-8 /W4 /permissive-)
endif()
