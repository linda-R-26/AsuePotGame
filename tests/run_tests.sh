#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
build_dir=$(mktemp -d)
trap 'rm -rf "$build_dir"' EXIT
g++ -O2 ASuePotGame/tests/test_core.cpp -o "$build_dir/core"
"$build_dir/core"
g++ -O2 ASuePotGame/tests/balance_sim.cpp -o "$build_dir/balance"
python3 ASuePotGame/tests/check_balance.py "$build_dir/balance"
g++ -O2 ASuePotGame/tests/test_save.cpp -o "$build_dir/save"
"$build_dir/save"
g++ -c -O2 -ffreestanding -fno-builtin -Dentry=asue_entry -Dmemset=asue_memset -Dmemcpy=asue_memcpy ASuePotGame/project/main.cpp -o "$build_dir/app.o"
g++ -O2 ASuePotGame/tests/test_startup.cpp "$build_dir/app.o" -o "$build_dir/startup"
"$build_dir/startup"
python3 ASuePotGame/tests/check_pe.py
