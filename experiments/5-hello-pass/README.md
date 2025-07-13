# 5. Hello Pass

LLVM IRの解析と変換を行うパスを実装する。

## 実行方法

1. Set environment variables

   ```sh
   set --export LLVM_DIR /home/linuxbrew/.linuxbrew/opt/llvm
   ```

2. Build the project

   ```sh
   cmake -DHL_LLVM_INSTALL_DIR=$LLVM_DIR -S . -B build
   cmake --build build
   ```

3. Run the pass

   ```sh
   $ cd build

   # Emit the LLVM IR from the input C file
   $ $LLVM_DIR/bin/clang -O1 -S -emit-llvm ../inputs/functions.c -o functions.ll

   # Run the pass on the generated LLVM IR
   $ $LLVM_DIR/bin/opt -load-pass-plugin ./src/libHelloWorld.so -passes=hello-world -disable-output functions.ll
   (llvm-tutor) Hello from: foo
   (llvm-tutor)   number of arguments: 1
   (llvm-tutor) Hello from: bar
   (llvm-tutor)   number of arguments: 2
   (llvm-tutor) Hello from: fez
   (llvm-tutor)   number of arguments: 3
   (llvm-tutor) Hello from: main
   (llvm-tutor)   number of arguments: 2
   ```

## 参考文献

https://github.com/banach-space/llvm-tutor
