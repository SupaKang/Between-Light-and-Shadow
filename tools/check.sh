#!/bin/bash
# Pre-commit quality gate: warnings build, ASan/UBSan tests, clang-tidy + cppcheck on changed sources.
# Usage: tools/check.sh [--all]   (default: only changed src/ tests/ .cpp/.h files)
set -euo pipefail
cd "$(dirname "$0")/.."
LLVM=/opt/homebrew/opt/llvm/bin
fail=0

echo "== build (warnings)"
[ -d build ] || cmake -S . -B build >/dev/null
cmake --build build -j8 2>&1 | grep -E 'warning:|error:' && fail=1 || true

# Apple clang's ASan runtime hangs at startup on macOS 26; use Homebrew LLVM.
echo "== test (ASan+UBSan)"
[ -d build-asan ] || cmake -S . -B build-asan -DBLS_SANITIZE=ON -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_COMPILER="$LLVM/clang++" >/dev/null
cmake --build build-asan --target BattleRulesTest -j8 >/dev/null
./build-asan/BattleRulesTest >/dev/null 2>&1 || { echo "BattleRulesTest failed under sanitizers"; fail=1; }

if [ "${1:-}" = "--all" ]; then
  files=$(ls src/*.cpp)
else
  files=$( { git diff --name-only HEAD; git ls-files --others --exclude-standard; } \
    | grep -E '^(src|tests)/.*\.(cpp|h)$' | sort -u || true)
fi
cpp=$(echo "$files" | grep -E '\.cpp$' || true)

if [ -n "$cpp" ]; then
  echo "== clang-tidy"
  "$LLVM/clang-tidy" -p build --quiet --extra-arg=-isysroot"$(xcrun --show-sdk-path)" $cpp 2>/dev/null \
    | grep -E 'warning:|error:' && fail=1 || true
  echo "== cppcheck"
  cppcheck --enable=warning,performance,portability --std=c++17 --quiet --error-exitcode=1 $cpp || fail=1
fi

[ $fail -eq 0 ] && echo "[PASS] check" || { echo "[FAIL] check"; exit 1; }
