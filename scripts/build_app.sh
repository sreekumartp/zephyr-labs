#!/usr/bin/env bash
set -euo pipefail

APP_PATH=${1:-}
if [[ -z "$APP_PATH" || ! -d "$APP_PATH" ]]; then
  echo "❌ Usage: $0 <app-path>"
  exit 1
fi

# Build the main app
BUILD_DIR="$APP_PATH/build"
echo "🔧 Building main application: $APP_PATH → $BUILD_DIR"
rm -rf "$BUILD_DIR"
west build -b native_sim "$APP_PATH" --build-dir "$BUILD_DIR"

# Check for tests within the main application directory
if [ -d "$APP_PATH/test" ] || [ -f "$APP_PATH/testcase.yaml" ]; then
  echo "🧪 Running application tests for $APP_PATH..."
  pushd "$BUILD_DIR" >/dev/null
  if output=$(west twister \
    -p native_sim \
    -T "$PWD/../" \
    --clobber-output \
    --output-directory=twister-out 2>&1); then
    echo "✅ Application tests executed successfully"
  else
    if echo "$output" | grep -q "No testsuites found at the specified location"; then
      echo "⚠️ No tests found for $APP_PATH — skipping test execution"
    else
      echo "❌ Twister errored unexpectedly for application tests:"
      echo "$output"
      popd >/dev/null
      exit 1
    fi
  fi
  popd >/dev/null
else
  echo "ℹ️ No tests found for application $APP_PATH."
fi

# Explicitly run tests for the event_bus component
EVENT_BUS_TEST_PATH="components/event_bus/tests"
if [ -d "$EVENT_BUS_TEST_PATH" ]; then
  echo "============================================================"
  echo "🧪 Running component tests for event_bus..."
  if west twister \
    -p native_sim \
    -T "$EVENT_BUS_TEST_PATH" \
    --clobber-output \
    --inline-logs; then
    echo "✅ event_bus tests executed successfully"
  else
    echo "❌ event_bus tests failed."
    exit 1
  fi
else
  echo "ℹ️ event_bus test directory not found, skipping."
fi

echo "✅ Build and test script complete for $APP_PATH"

