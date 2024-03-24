# LLVM IR

- human-readable な LLVM IR をファイルから生成する:

  ```sh
  clang -S -emit-llvm -o hello.ll hello.c
  ```

- LLVM Bytecode を生成する:

  ```sh
  clang -c -emit-llvm -o hello.bc hello.c
  ```

## 型

### 整数型

`i32`といった具合に、`i` + ビット幅で整数型を表す。ビット幅は $1 \sim 2^{23}-1$ まで任意に指定できる。

具体例：

- `i1`: 1 ビットの整数型（ブール型）
- `i8`: 8 ビットの整数型（char 型）
- `i32`: 32 ビットの整数型（32 ビットの整数型）
- `i64`: 64 ビットの整数型（64 ビットの整数型）

### 浮動小数点型

- `float`: 32 ビットの浮動小数点型
- `double`: 64 ビットの浮動小数点型

### ポインタ型

C 言語のポインタ型と同様に、`*` + 型でポインタ型を表す。

具体例：

- `i32*`: 32 ビットの整数型へのポインタ型
- `float*`: 32 ビットの浮動小数点型へのポインタ型

### aggregate 型

aggregate とは集合体を意味する。構造体や配列などの複数の要素を持つ型を表す。

- **配列**: `[n x T]`: `T` 型の要素を `n` 個持つ配列
  - e.g. `[3 x i32]`: 32 ビットの整数型を 3 つ持つ配列
- **ベクター**: `<n x T>`: `T` 型の要素を `n` 個持つベクター
  - e.g. `<4 x i32>`: 32 ビットの整数型を 4 つ持つベクター
- **構造体**: `{T1, T2, ..., Tn}`: `T1, T2, ..., Tn` 型の要素を持つ構造体
  - e.g. `{i32, i32}`: 32 ビットの整数型を 2 つ持つ構造体

### その他

- `void`: 値を持たない型
- `label`: ラベル型
- `metadata`: メタデータ型
- `token`: トークン型

## 識別子

- **グローバル識別子**: `@` + 識別子名
  - e.g. 関数, グローバル変数, ...
- **ローカル識別子**: `%` + 識別子名
  - e.g. 型, ローカル変数, ...

さらに、識別子には次の 3 種類がある。

1. **名前付きの値**: 識別子名として、数字以外の文字列からスタートする識別子  
   e.g. `%foo`, `@DivideByZero`, `%a.really.long.identifier2`
2. **無名の値**: 識別子名として、数字からスタートする識別子  
   e.g. `%0`, `@1`, `%22`
3. **定数**: [Constants](https://llvm.org/docs/LangRef.html#constants) で定義される定数

https://llvm.org/docs/LangRef.html#id1810

## ラベル

`<ラベル名>:` でラベルを定義する。ラベルはアセンブリ言語のラベルと同様に、ジャンプ命令のターゲットとして使用される。

entry というラベルを定義する例：

```llvm
entry:
  ; ここにコードを書く
```

### ラベルへのジャンプ

ラベルへのジャンプは `br` 命令を使う。

- `br label <dest>`: ラベル `<dest>` にジャンプする
- `br i1 <cond>, label <iftrue>, label <iffalse>`: 条件 `<cond>` が真ならば `<iftrue>` に、偽ならば `<iffalse>` にジャンプする

例：

```llvm
Test:
  %cond = icmp eq i32 %a, %b                     ; cond = (a == b)
  br i1 %cond, label %IfEqual, label %IfUnequal  ; if (cond) goto IfEqual; else goto IfUnequal
IfEqual:
  ret i32 1
IfUnequal:
  ret i32 0
```

https://llvm.org/docs/LangRef.html#br-instruction

## コメント

`;` から行末までがコメントとなる。

```llvm
%0 = add i32 1, 2  ; 1 + 2 = 3
                   ; ここはコメント
```

## 文字列定数

文字列は `"` で囲む。エスケープには `\` を使う。

例：`"Hello, World!\n"`

## 参考文献

[Qiita@Anko_9801 - こわくない LLVM 入門！](https://qiita.com/Anko_9801/items/df4475fecbddd0d91ccc)

[llvm.org - LLVM Language Reference Manual](https://llvm.org/docs/LangRef.html)

[rui - 低レイヤを知りたい人のための C コンパイラ作成入門 | 付録 1：x86-64 命令セット チートシート](https://www.sigbus.info/compilerbook#%E4%BB%98%E9%8C%B21x86-64%E5%91%BD%E4%BB%A4%E3%82%BB%E3%83%83%E3%83%88-%E3%83%81%E3%83%BC%E3%83%88%E3%82%B7%E3%83%BC%E3%83%88)

[GitHub@stanislaw/LLVM-Cheatsheet - LLVM-Cheatsheet](https://github.com/stanislaw/LLVM-Cheatsheet)
