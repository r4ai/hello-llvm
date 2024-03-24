# 2. helloworld

"Hello, World!" を出力するプログラムを作成する。

```c title=helloworld.c
#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Hello, World!\n");
  return 0;
}
```

これを `clang -S -emit-llvm -O3 helloworld.` でコンパイルすると、次の LLVM IR が生成される。

```llvm title=helloworld.ll
; ModuleID = 'helloworld.c'
source_filename = "helloworld.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@str = private unnamed_addr constant [14 x i8] c"Hello, World!\00", align 1

; Function Attrs: nofree nounwind uwtable
define dso_local i32 @main(i32 noundef %0, ptr nocapture noundef readnone %1) local_unnamed_addr #0 {
  %3 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str)
  ret i32 0
}

; Function Attrs: nofree nounwind
declare noundef i32 @puts(ptr nocapture noundef readonly) local_unnamed_addr #1

attributes #0 = { nofree nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"Homebrew clang version 17.0.6"}
```

さらに、`llc helloworld.ll && clang -no-pie helloworld.s -o helloworld && ./helloworld` を実行すると、`Hello, World!` が出力される。

```sh
$ llc helloworld.ll
$ clang -no-pie helloworld.s -o helloworld
$ ./helloworld
Hello, World!
```

## 解説

> [!warning]
> 以下の解説は ChatGPT によって生成したものに多少の修正を加えたものである

この LLVM IR コードは、`Hello, World!`を出力する単純な C プログラムを表しています。各命令と構成要素を一行ずつ詳細に解説します。

### グローバル変数

- `@str = private unnamed_addr constant [14 x i8] c"Hello, World!\00", align 1`は、14 バイトの文字列`Hello, World!`とヌル終端文字`\00`を含むグローバル定数を定義しています。`private`はこの変数がこのモジュール内でのみアクセス可能であることを示し、`unnamed_addr`はこの変数のアドレスが一意である必要がないことを示しています。`align 1`はこのデータが 1 バイト境界に配置されることを意味します。

### `main`関数

- `define dso_local i32 @main(i32 noundef %0, ptr nocapture noundef readnone %1) local_unnamed_addr #0 {...}`は、プログラムのエントリーポイントである`main`関数を定義します。`i32`は戻り値の型、`%0`は引数の数、`%1`は引数の配列を指します。属性に`nofree`、`nounwind`、`uwtable`があります。
  - `%3 = tail call i32 @puts(ptr nonnull dereferenceable(1) @str)`は、`puts`関数を呼び出し、`@str`グローバル変数を渡して`Hello, World!`を出力します。`tail call`は、これが関数の最後の操作であり、戻り値がそのまま`main`から返されることを最適化のヒントとしてコンパイラに伝えます。
  - `ret i32 0`は、`main`関数から 0 を返し、プログラムが正常に終了したことを示します。

### `puts`関数

- `declare noundef i32 @puts(ptr nocapture noundef readonly) local_unnamed_addr #1`は、`puts`関数の宣言です。この関数は標準ライブラリの一部で、与えられた文字列を出力します。`noundef`は引数が未定義でないことを保証し、`nocapture`はこのポインタが関数によって保存されないことを示し、`readonly`は引数がこの関数によって変更されないことを示します。

### 属性

- `attributes #0`と`attributes #1`は、関数に適用される属性を定義します。これには、`nofree`、`nounwind`、`uwtable`などの属性が含まれます。これらの属性は、関数が例外を投げない、フリー操作を行わない、アンワインドテーブルがあるなどの特性をコンパイラに伝えます。

## 参考文献

https://qiita.com/Anko_9801/items/df4475fecbddd0d91ccc#%E6%96%87%E5%AD%97%E5%88%97%E9%96%A2%E6%95%B0%E5%91%BC%E3%81%B3%E5%87%BA%E3%81%97
