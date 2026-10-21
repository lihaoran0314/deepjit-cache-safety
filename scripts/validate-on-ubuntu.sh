#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BASE_COMMIT="18fe520b0473fc45d32d2f0e86923da707de19ab"
PATCH="$ROOT/patches/0001-Fix-disk-cache-publication-and-symlink-safe-cleanup.patch"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

git clone https://github.com/deepseek-ai/DeepJIT.git "$WORK/DeepJIT"
git -C "$WORK/DeepJIT" checkout "$BASE_COMMIT"
git -C "$WORK/DeepJIT" apply "$PATCH"

g++ -std=c++20 -O2 -Wall -Wextra -Werror \
    -I"$WORK/DeepJIT/include" \
    "$ROOT/tests/cache_safety_smoke.cpp" \
    -ldw -ldl -pthread \
    -o "$WORK/cache_safety_smoke"

"$WORK/cache_safety_smoke"
