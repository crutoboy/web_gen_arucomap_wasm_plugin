# web_gen_arucomap_wasm_plugin

Вспомогательный WebAssembly-модуль для проекта **[web_gen_arucomap](https://github.com/crutoboy/web_gen_arucomap)**.

Генерирует изображение карты ArUco-маркеров (аналог топика `/aruco_map/image` из Clover) прямо в браузере.

### Что делает модуль

- Принимает содержимое `map.txt`
- Рисует маркеры, оси
- Возвращает изображение (для отображения в основном приложении)

### Сборка

Для сборки нужны:

- [Emscripten SDK](https://emscripten.org/)
- OpenCV 4.10.0 + opencv_contrib (с модулем `aruco`)
- Node.js (для удобного запуска сборки)

#### 1. Установка Emscripten

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
cd ..
```

#### 2. Сборка OpenCV с поддержкой ArUco

```bash
# Клонируем стабильную версию 4.10.0
git clone --branch 4.10.0 --depth 1 https://github.com/opencv/opencv.git
git clone --branch 4.10.0 --depth 1 https://github.com/opencv/opencv_contrib.git

cd opencv

emcmake python3 platforms/js/build_js.py build_wasm \
  --cmake_option="-DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules" \
  --cmake_option="-DBUILD_opencv_aruco=ON" \
  --cmake_option="-DBUILD_TESTS=OFF" \
  --cmake_option="-DBUILD_PERF_TESTS=OFF" \
  --cmake_option="-DBUILD_EXAMPLES=OFF" \
  --build_wasm
```

> Сборка OpenCV занимает довольно много времени.

#### 3. Сборка самого модуля

Из корня репозитория:

```bash
npm run build
```

Скрипт `emscripten/build.sh` скомпилирует C++ код и положит результат в:

```
dist/main.js
dist/main.wasm
```

### Использование в основном проекте

Скопируйте собранные файлы в основной репозиторий:

```bash
cp dist/main.js   ../web_gen_arucomap/public/wasm/
cp dist/main.wasm ../web_gen_arucomap/public/wasm/
```

### Структура проекта

```
web_gen_arucomap_wasm_plugin/
├── src/
│   ├── main.cpp          # Точка входа + парсинг map.txt + биндинги
│   └── draw.cpp / draw.h # Отрисовка карты
├── emscripten/
│   └── build.sh          # Скрипт сборки
├── dist/                 # Результат сборки (main.js + main.wasm)
├── package.json
└── README.md
```

### Зависимости при сборке

В `.gitignore` уже добавлены:

- `opencv/`
- `opencv_contrib/`
- `emsdk/`
- `dist/`

### Лицензия

[MIT](LICENSE)
