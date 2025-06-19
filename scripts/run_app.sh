#!/bin/bash

# Usage: ./scripts/run_app.sh <app_name>
# Only works for simulator boards like native_sim or qemu_x86

APP_NAME=$1

if [ -z "$APP_NAME" ]; then
  echo "Usage: $0 <app_name>"
  exit 1
fi

BUILD_DIR="build/${APP_NAME}"

if [ ! -d "$BUILD_DIR" ]; then
  echo "❌ Build directory does not exist: $BUILD_DIR"
  exit 1
fi

echo "🚀 Running app '${APP_NAME}'..."
west build -t run -d "$BUILD_DIR"
if [ $? -ne 0 ]; then
  echo "❌ Failed to run app '${APP_NAME}'"
  exit 1
fi