#!/bin/bash

# Usage: ./scripts/build_app.sh <app_name> <board>
# Example: ./scripts/build_app.sh my_timer_app native_sim

set -e

APP_NAME=$1
BOARD=$2

if [ -z "$APP_NAME" ] || [ -z "$BOARD" ]; then
  echo "Usage: $0 <app_name> <board>"
  exit 1
fi

BUILD_DIR="build/${APP_NAME}"

echo "=== Building ${APP_NAME} for board ${BOARD} ==="
west build -b "${BOARD}" "apps/${APP_NAME}" -d "${BUILD_DIR}"

