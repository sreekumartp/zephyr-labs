#!/bin/bash

# Exit on any error
set -e

# Input: app name
APP_NAME=$1

if [ -z "$APP_NAME" ]; then
  echo "❌ Usage: $0 <app_name>"
  exit 1
fi

APP_DIR="apps/$APP_NAME"

if [ -d "$APP_DIR" ]; then
  echo "⚠️  App '$APP_NAME' already exists at $APP_DIR"
  exit 1
fi

echo "🚀 Creating new app: $APP_NAME"
mkdir -p "$APP_DIR/src"
mkdir -p "$APP_DIR/.vscode"

# Create prj.conf
cat > "$APP_DIR/prj.conf" <<EOF
CONFIG_PRINTK=y
EOF

# Create CMakeLists.txt
cat > "$APP_DIR/CMakeLists.txt" <<EOF
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS \$ENV{ZEPHYR_BASE})
project($APP_NAME)
target_sources(app PRIVATE src/main.c)
EOF

# Create src/main.c
cat > "$APP_DIR/src/main.c" <<EOF
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

void main(void) {
    printk("Hello from $APP_NAME\\n");
}
EOF

# Create .vscode/tasks.json
cat > "$APP_DIR/.vscode/tasks.json" <<EOF
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Build",
      "type": "shell",
      "command": "west build -b native_sim . --build-dir build",
      "group": "build"
    },
    {
      "label": "Run",
      "type": "shell",
      "command": "./build/zephyr/zephyr.exe",
      "group": "test"
    }
  ]
}
EOF

# Create .vscode/settings.json
cat > "$APP_DIR/.vscode/settings.json" <<EOF
{
  "cmake.sourceDirectory": "\${workspaceFolder}",
  "cmake.buildDirectory": "\${workspaceFolder}/build"
}
EOF

# Create .vscode/launch.json
cat > "$APP_DIR/.vscode/launch.json" <<EOF
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug native_sim",
      "type": "cppdbg",
      "request": "launch",
      "program": "\${workspaceFolder}/build/zephyr/zephyr.exe",
      "stopAtEntry": false,
      "cwd": "\${workspaceFolder}",
      "MIMode": "gdb",
      "miDebuggerPath": "/usr/bin/gdb"
    }
  ]
}
EOF

echo "✅ App '$APP_NAME' created at '$APP_DIR'"

