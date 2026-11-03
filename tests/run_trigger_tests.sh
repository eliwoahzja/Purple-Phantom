#!/usr/bin/env bash
set -euo pipefail

task_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
task_build="$(mktemp -d "${TMPDIR:-/tmp}/phantom-trigger-tests.XXXXXX")"
trap 'rm -rf -- "$task_build"' EXIT

"${CXX:-c++}" -std=c++17 -Wall -Wextra -Werror -pedantic \
    -fsanitize=address,undefined -fno-omit-frame-pointer -g \
    -I"$task_root/src/main/jni" \
    "$task_root/tests/trigger_core_tests.cpp" \
    -o "$task_build/trigger_core_tests"
"$task_build/trigger_core_tests"
