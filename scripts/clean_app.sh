#!/bin/bash

# Usage: ./scripts/clean_app.sh <app_name>
# Example: ./scripts/clean_app.sh my_timer_app

APP_NAME=$1

if [ -z "$APP_NAME" ]; then
  echo "Usage: $0 <app_name>"
  exit 1
fi

BUILD_DIR="build/${APP_NAME}"

if [ -d "$BUILD_DIR" ]; then
  echo "🧹 Cleaning build directory for ${APP_NAME}: $BUILD_DIR"
  rm -rf "$BUILD_DIR"
  echo "✅ Clean complete."
else
  echo "ℹ️ Build directory does not exist: $BUILD_DIR"
fi

