#!/usr/bin/env sh
set -eu

mkdir -p build/ftui build/ftht

cxx="${CXX:-c++}"
cxxflags="${CXXFLAGS:--std=c++17}"
ftui_libs="$(pkg-config --cflags --libs cairo x11)"

for src in examples/ftui/*.cpp; do
    name="$(basename "$src" .cpp)"
    "$cxx" "$src" -o "build/ftui/$name" $cxxflags $ftui_libs -pthread
done

for src in examples/ftht/*.cpp; do
    name="$(basename "$src" .cpp)"
    "$cxx" "$src" -o "build/ftht/$name" $cxxflags $ftui_libs -pthread
done

"$cxx" examples/ft_combined_smoke.cpp -o build/ft_combined_smoke $cxxflags $ftui_libs -pthread
