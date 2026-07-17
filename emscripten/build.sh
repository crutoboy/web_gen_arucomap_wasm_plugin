#!/bin/bash
set -e

mkdir -p dist

emcc src/main.cpp \
  -O3 \
  -s WASM=1 \
  -s MODULARIZE=1 \
  -s EXPORT_NAME="webGenArucomapImage" \
  -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" \
  -s ALLOW_MEMORY_GROWTH=1 \
  --bind \
  -o dist/main.js