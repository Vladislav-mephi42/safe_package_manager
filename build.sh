#!/bin/bash
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR" || { echo "Не удалось перейти в $BUILD_DIR"; exit 1; }

# Явно включаем экспорт compile_commands.json
cmake .. -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    || { echo "CMake завершился с ошибкой"; exit 1; }

make -j$(nproc) || { echo "Сборка завершилась с ошибкой"; exit 1; }

# Создаём symlink в корень проекта для Clangd
ln -sf "$BUILD_DIR/compile_commands.json" "$PROJECT_ROOT/" 2>/dev/null || true

echo "Сборка успешно завершена!"