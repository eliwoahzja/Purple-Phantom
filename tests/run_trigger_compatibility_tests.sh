#!/usr/bin/env bash
set -euo pipefail
task_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
task_build="$(mktemp -d "${TMPDIR:-/tmp}/phantom-compatibility-tests.XXXXXX")"
trap 'rm -rf -- "$task_build"' EXIT
"${CXX:-c++}" -std=c++17 -Wall -Wextra -Werror -pedantic \
    -fsanitize=address,undefined -fno-omit-frame-pointer -g \
    -I"$task_root/src/main/jni" "$task_root/tests/trigger_compatibility_tests.cpp" \
    "$task_root/src/main/jni/Trigger/TriggerCompatibility.cpp" \
    "$task_root/src/main/jni/Trigger/TriggerRuntime.cpp" -ldl \
    -o "$task_build/trigger_compatibility_tests"
"$task_build/trigger_compatibility_tests"
