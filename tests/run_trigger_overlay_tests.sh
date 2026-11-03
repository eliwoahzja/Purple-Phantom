#!/usr/bin/env bash
set -euo pipefail
task_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
task_build="$(mktemp -d "${TMPDIR:-/tmp}/phantom-overlay-tests.XXXXXX")"
trap 'rm -rf -- "$task_build"' EXIT
task_native="$task_root/src/main/jni"
"${CXX:-c++}" -std=c++17 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
    -I"$task_native" -I"$task_native/ImGui" \
    "$task_root/tests/trigger_overlay_tests.cpp" \
    "$task_native/Trigger/TriggerRuntime.cpp" \
    "$task_native/ImGui/imgui.cpp" "$task_native/ImGui/imgui_draw.cpp" \
    "$task_native/ImGui/imgui_tables.cpp" "$task_native/ImGui/imgui_widgets.cpp" \
    -o "$task_build/trigger_overlay_tests"
"$task_build/trigger_overlay_tests"
