# 1. just-return

ただ 0 を返すだけの関数を定義する。

```c title=just-return.c
int main() { return 0; }
```

このプログラムを `clang -S -emit-llvm -O0 just-return.c -o just-return.ll` よりコンパイルすると、次の IR が生成される。

```llvm title=just-return.ll
; ModuleID = 'just-return.c'
source_filename = "just-return.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Homebrew clang version 17.0.6"}
```

さらにこれを `llc just-return.ll` でアセンブリに変換すると、次のアセンブリが生成される。

```asm title=just-return.s
	.text
	.file	"just-return.c"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$0, -4(%rbp)
	xorl	%eax, %eax
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.ident	"Homebrew clang version 17.0.6"
	.section	".note.GNU-stack","",@progbits
```

さらにこれを `clang just-return.s -o just-return` で実行ファイルにコンパイルし、`./just-return` で実行すると、何も出力されずに終了する。`echo $status` もしくは Bash の場合は `echo $?` で終了ステータスを確認すると、`0` が返ってくることを確認できる。

## 解説

### モジュールとターゲット定義

- `; ModuleID = 'just-return.c'`: コメントであり、この LLVM モジュールが just-return.c ソースファイルから生成されたことを示しています。
- `source_filename = "just-return.c"`: このモジュールの元となったソースファイルの名前です。
  - https://llvm.org/docs/LangRef.html#source-filename
- `target datalayout = "..."`: ターゲットアーキテクチャのデータレイアウトを記述します。これには、エンディアン、ポインタサイズ、アライメントの情報などが含まれます。
  - https://llvm.org/docs/LangRef.html#langref-datalayout
- `target triple = "x86_64-unknown-linux-gnu"`: コンパイルターゲットの「トリプル」を指定します。これは、プロセッサアーキテクチャ、ベンダー、オペレーティングシステムを表します。

```llvm
; ModuleID = 'just-return.c'
source_filename = "just-return.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"
```

### 関数定義 (重要)

- `define dso_local i32 @main() #0 { ... }`: `main` 関数の定義
  - `dso_local`: この関数が現在の DSO（Dynamic Shared Object）に局所的であることを示す
  - `i32`: 関数が 32 ビットの整数型を返すことを示す
  - `@main()`: 関数の名前。`@` はこの関数がグローバル識別子であることを示す
  - `#0`: 関数属性のセットを参照するメタデータタグ
- `{ ... }`: 関数本体
  - `%1 = alloca i32, align 4`: 32 ビットの整数型の領域を確保し、そのアドレスを `%1` に格納
    - https://llvm.org/docs/LangRef.html#alloca-instruction
  - `store i32 0, ptr %1, align 4`: `%1` に 0 を格納
    - https://llvm.org/docs/LangRef.html#store-instruction
  - `ret i32 0`: 0 を返して関数を終了
    - https://llvm.org/docs/LangRef.html#ret-instruction
- `attributes #0 = { ... }`: 関数属性のセット
  - `noinline`: この関数はインライン展開されるべきではない。
  - `nounwind`: この関数は例外を発生させない。
  - `optnone`: この関数に対して最適化を行うべきではない。
  - `uwtable`: この関数は Unwind Table を持つべきで、例外処理やスタックアンワインドに必要。
  - その他のターゲット固有やパフォーマンスに関連する属性。

```llvm
; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
```

### モジュール属性

> [!warning]
> 以下は ChatGPT の解説である

この LLVM IR の一部は、メタデータとモジュールの特定のフラグやコンパイラ情報を定義しています。ここでそれぞれの行が何を意味しているのかを説明します：

メタデータ定義:

- `!llvm.module.flags = !{!0, !1, !2, !3, !4}`: この行はモジュールのフラグを定義するためのメタデータです。`!0`から`!4`までの各エントリーは、モジュール全体に適用される様々な設定や性質を示します。

- `!llvm.ident = !{!5}`: この行は、この LLVM IR を生成したツール（この場合はコンパイラ）の識別情報を含むメタデータを定義します。

各メタデータエントリーの解説:

- `!0 = !{i32 1, !"wchar_size", i32 4}`: `wchar_t`型のサイズを定義します。ここでは 4 バイト（32 ビット）と指定されています。これは、異なるプラットフォームやコンパイラ間で`wchar_t`のサイズが異なることがあるため、重要です。

- `!1 = !{i32 8, !"PIC Level", i32 2}`: 「Position Independent Code (PIC)」のレベルを指定します。`i32 2`は、おそらく高いレベルの PIC が要求されていることを意味します。PIC は、共有ライブラリなどでアドレスの依存性を避けるために使用されます。

- `!2 = !{i32 7, !"PIE Level", i32 2}`: 「Position Independent Executable (PIE)」のレベルを指定します。これもまた、実行可能ファイルがメモリ上の任意の位置にロードされることを許容するためのものです。

- `!3 = !{i32 7, !"uwtable", i32 2}`: このフラグは、Unwind Table の使用を示します。これは、例外が発生した場合や他の非正常なプログラム終了の際に、スタックを正しく巻き戻すための情報を保持します。

- `!4 = !{i32 7, !"frame-pointer", i32 2}`: このフラグは、フレームポインタの保持に関するポリシーを定義します。フレームポインタは、関数のコールスタックをトレースする際に有用な情報を提供します。`i32 2`は、恐らくフレームポインタを保持することを強制していることを意味します。

- `!5 = !{!"Homebrew clang version 17.0.6"}`: この LLVM IR を生成したコンパイラのバージョンとその名称を示します。ここでは「Homebrew clang version 17.0.6」となっており、Homebrew を通じてインストールされた Clang コンパイラのバージョン 17.0.6 であることを意味します。

これらのメタデータは、コンパイル時の特定の条件や要求、および生成されたコードの性質に関する重要な情報を提供します。これにより、LLVM の最適化パスや他のツールが、ソースコードをより効果的に処理するためのコンテキストを得ることができます。
