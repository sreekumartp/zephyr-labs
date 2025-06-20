#!/usr/bin/env bash
set -euo pipefail

APP_PATH=${1:-}
if [[ -z "$APP_PATH" || ! -d "$APP_PATH" ]]; then
  echo "❌ Usage: $0 <app-path>"
  exit 1
fi

# Build the app
BUILD_DIR="$APP_PATH/build"
echo "🔧 Building $APP_PATH → $BUILD_DIR"
rm -rf "$BUILD_DIR"
west build -b native_sim "$APP_PATH" --build-dir "$BUILD_DIR"

# Run tests via twister inside build dir
pushd "$BUILD_DIR" >/dev/null

echo "🧪 Running Twister tests..."
if output=$(west twister \
  -p native_sim \
  -T "$PWD/../" \
  --clobber-output \
  --output-directory=twister-out 2>&1); then
  echo "✅ Tests executed successfully"
else
  if echo "$output" | grep -q "No testsuites found at the specified location"; then
    echo "⚠️ No tests found for $APP_PATH — skipping test execution"
  else
    echo "❌ Twister errored unexpectedly:"
    echo "$output"
    popd >/dev/null
    exit 1
  fi
fi

popd >/dev/null

echo "✅ Build (+ test) complete for $APP_PATH"
