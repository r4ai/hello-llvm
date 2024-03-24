#!/usr/bin/env bash

make bf

mkdir -p dist

./bf <./input.bf >./dist/output.ll

# 最適化なし
llc ./dist/output.ll -o ./dist/output.s
clang ./dist/output.s -o ./dist/output

# 最適化あり
opt -S -O3 ./dist/output.ll -o ./dist/output-opt.ll
llc -O3 ./dist/output-opt.ll -o ./dist/output-opt.s
clang -O3 ./dist/output-opt.s -o ./dist/output-opt

# ベンチマーク
hyperfine "./dist/output <./input.bf" "./dist/output-opt <./input.bf"
