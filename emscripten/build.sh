#!/bin/bash
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OPENCV_SRC="$ROOT/opencv"
OPENCV_BUILD="$ROOT/opencv/build_wasm"
CONTRIB="$ROOT/opencv_contrib"

mkdir -p "$ROOT/dist"

INCLUDES=(
  -I"$OPENCV_SRC/include"
  -I"$OPENCV_BUILD"
  -I"$OPENCV_SRC/modules/core/include"
  -I"$OPENCV_SRC/modules/imgproc/include"
  -I"$OPENCV_SRC/modules/calib3d/include"
  -I"$OPENCV_SRC/modules/features2d/include"
  -I"$OPENCV_SRC/modules/flann/include"
  -I"$OPENCV_SRC/modules/objdetect/include"
  -I"$CONTRIB/modules/aruco/include"
  -I"$ROOT/src"
)

LIBS=(
  -L"$OPENCV_BUILD/lib"
  -L"$OPENCV_BUILD/3rdparty/lib"
  -lopencv_objdetect
  -lopencv_calib3d
  -lopencv_features2d
  -lopencv_flann
  -lopencv_imgproc
  -lopencv_core
  -lzlib
)

em++ \
  "$ROOT/src/main.cpp" \
  "$ROOT/src/draw.cpp" \
  -O3 \
  -std=c++17 \
  -frtti \
  -fexceptions \
  "${INCLUDES[@]}" \
  "${LIBS[@]}" \
  -s WASM=1 \
  -s MODULARIZE=1 \
  -s EXPORT_NAME="webGenArucomapImage" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s DISABLE_EXCEPTION_CATCHING=0 \
  -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" \
  -s EXPORTED_FUNCTIONS="['_malloc','_free']" \
  --bind \
  -o "$ROOT/dist/main.js"

echo "Build OK → dist/main.js + dist/main.wasm"