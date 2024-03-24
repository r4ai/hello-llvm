# 4. Brainf\*ck compiler

## 実験メモ

### ポインタ

ポインタは、`i8*` として表現される。`i8` は 8 ビットの整数型を表し、`*` はポインタ型を表す。また一般的な 64 ビットシステムではポインタ型のサイズは 8 バイトである。

```c
#include <stdlib.h>

int main() {
  char *data = (char *)calloc(30000, sizeof(char));
  char *ptr = data;
  free(data);
}
```

yields (simplified):

```llvm
define i32 @main() {
  %1 = alloca ptr, align 8  ; `data` 変数のアドレスを格納する領域をスタックメモリ上に確保
  %2 = alloca ptr, align 8  ; `ptr` 変数のアドレスを格納する領域をスタックメモリ上に確保

  ; `calloc` 関数で 30000 バイトのヒープメモリを確保し、そのアドレスを `data` に格納
  %3 = call noalias ptr @calloc(i64 noundef 30000, i64 noundef 1)
  store ptr %3, ptr %1, align 8

  ; `ptr` に `data` のアドレスを格納
  %4 = load ptr, ptr %1, align 8
  store ptr %4, ptr %2, align 8

  ; `free` 関数で `data` のアドレスに確保されたヒープメモリを解放
  %5 = load ptr, ptr %1, align 8
  call void @free(ptr noundef %5)

  ; 返り値として 0 を返す
  ret i32 0
}

declare noalias ptr @calloc(i64 noundef, i64 noundef)

declare void @free(ptr noundef)
```

See: [samples/pointer.c](./samples/pointer.c), [samples/pointer.ll](./samples/pointer.ll)

### ポインタのインクリメントとデクリメント

```c
#include <stdlib.h>

int main() {
  char *data = (char *)calloc(30000, sizeof(char));
  char *ptr = data;

  ++ptr;

  ++*ptr;

  free(data);
  return 0;
}
```

yields (simplified):

```llvm
define i32 @main() {
  %1 = alloca i32, align 4      ; `*ptr[1]` の値を格納する領域をスタックメモリ上に確保
  %2 = alloca ptr, align 8      ; `data` 変数のアドレスを格納する領域をスタックメモリ上に確保
  %3 = alloca ptr, align 8      ; `ptr` 変数のアドレスを格納する領域をスタックメモリ上に確保
  store i32 0, ptr %1, align 4  ; `*ptr[1]` の値を格納する領域に 0 を格納

  ; `calloc` 関数で 30000 バイトのヒープメモリを確保し、そのアドレスを `data` に格納
  %4 = call noalias ptr @calloc(i64 noundef 30000, i64 noundef 1) #3
  store ptr %4, ptr %2, align 8

  ; `ptr` に `data` のアドレスを格納
  %5 = load ptr, ptr %2, align 8
  store ptr %5, ptr %3, align 8

  ; `ptr` をインクリメント
  %6 = load ptr, ptr %3, align 8                 ; `tmp6 = ptr`
  %7 = getelementptr inbounds i8, ptr %6, i32 1  ; `tmp7 = tmp6 + 1`
  store ptr %7, ptr %3, align 8                  ; `ptr = tmp7`

  ; `*ptr` をインクリメント
  %8 = load ptr, ptr %3, align 8                 ; `tmp8 = ptr`
  %9 = load i8, ptr %8, align 1                  ; `tmp9 = *tmp8`
  %10 = add i8 %9, 1                             ; `tmp10 = tmp9 + 1`
  store i8 %10, ptr %8, align 1                  ; `*tmp8 = tmp10`

  ; `free` 関数で `data` のアドレスに確保されたヒープメモリを解放
  %11 = load ptr, ptr %2, align 8
  call void @free(ptr noundef %11) #4

  ; 返り値として 0 を返す
  ret i32 0
}
```

See: [samples/pointer-inc-dec.c](./samples/pointer-inc-dec.c), [samples/pointer-inc-dec.ll](./samples/pointer-inc-dec.ll)

### putchar 関数

```c
#include <stdio.h>

int main() {
  char character = 'a';
  char *ptr = &character;
  putchar(*ptr);
}
```

yields (simplified):

```llvm
define i32 @main() {
  ; `char character = 'a';`
  %1 = alloca i8, align 1
  store i8 97, ptr %1, align 1

  ; `char *ptr = &character;`
  %2 = alloca ptr, align 8
  store ptr %1, ptr %2, align 8

  ; `putchar(*ptr);`
  %3 = load ptr, ptr %2, align 8          ; `tmp3 = ptr`
  %4 = load i8, ptr %3, align 1           ; `tmp4 = *tmp3`
  %5 = sext i8 %4 to i32                  ; `tmp5 = (i32)tmp4`
  %6 = call i32 @putchar(i32 noundef %5)  ; `tmp6 = putchar(tmp5)`

  ret i32 0
}

declare i32 @putchar(i32 noundef)
```

See: [samples/putchar.c](./samples/putchar.c), [samples/putchar.ll](./samples/putchar.ll)

### Loop

```c
int main() {
  int i = 0;

  while (i < 3) {
    ++i;
  }
}
```

yields (simplified):

```llvm
define i32 @main() {
  %1 = alloca i32, align 4         ; int retValue = 0  // 返り値用の値
  %2 = alloca i32, align 4         ; int i = 0
  store i32 0, ptr %1, align 4     ; retValue = 0
  store i32 0, ptr %2, align 4     ; i = 0
  br label %3

3:                                                ; preds = %6, %0
  %4 = load i32, ptr %2, align 4   ; tmp4 = i
  %5 = icmp slt i32 %4, 3          ; tmp5 = tmp4 < 3
  br i1 %5, label %6, label %9     ; if (tmp5) goto %6; else goto %9

6:                                                ; preds = %3
  %7 = load i32, ptr %2, align 4   ; tmp7 = i
  %8 = add nsw i32 %7, 1           ; tmp8 = tmp7 + 1
  store i32 %8, ptr %2, align 4    ; i = tmp8
  br label %3                      ; goto %3

9:                                                ; preds = %3
  %10 = load i32, ptr %1, align 4  ; tmp10 = retValue  // tmp10 = 0
  ret i32 %10                      ; return tmp10
}
```

## Brainf\*ck の仕様

### 要素

Brainf\*ck は以下の要素から構成される。

- 現在のメモリを差し出す命令ポインタ。初期値はメモリの先頭
- 少なくとも 30000 個の要素があるメモリ。初期値は 0
- 入出力のストリーム

### 命令

| 命令 | 動作                                                           |
| ---- | -------------------------------------------------------------- |
| `+`  | 現在のメモリをインクリメントする                               |
| `-`  | 現在のメモリをデクリメントする                                 |
| `>`  | 命令ポインタを 1 つ右に移動させる                              |
| `<`  | 命令ポインタを 1 つ左に移動させる                              |
| `.`  | 現在のメモリの値を文字コードとみなし出力する                   |
| `,`  | 入力を求め、入力された値を現在のメモリに代入する               |
| `[`  | 現在のメモリが 0 ならば、対応する`]`の直後にジャンプする       |
| `]`  | 現在のメモリが 0 でないならば、対応する`[`の直後にジャンプする |

## 実行方法

1. `input.bf`に Brainf\*ck のプログラムを記述する
2. `make run` を実行する

例：

```sh
$ echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.+.+.>++++++++++." > input.bf

$ make
./bf < ./input.bf > ./output.ll
llc ./output.ll -o ./output.s
clang ./output.s -o ./output
./output
ABC
```

## 最適化

LLVM の最適化の有無による実行速度の比較結果（Benchmark 1 が最適化なしで、Benchmark 2 が最適化あり）：

```txt
Benchmark 1: ./dist/output <./input.bf
  Time (mean ± σ):      1.174 s ±  0.010 s    [User: 1.168 s, System: 0.002 s]
  Range (min … max):    1.163 s …  1.191 s    10 runs

Benchmark 2: ./dist/output-opt <./input.bf
  Time (mean ± σ):     707.6 ms ±  19.3 ms    [User: 703.1 ms, System: 0.0 ms]
  Range (min … max):   690.2 ms … 751.9 ms    10 runs

Summary
  ./dist/output-opt <./input.bf ran
    1.66 ± 0.05 times faster than ./dist/output <./input.bf
```

ベンチマークに利用したスクリプト：[bench.sh](./bench.sh)

## 参考文献

https://itchyny.hatenablog.com/entry/2017/02/27/100000
