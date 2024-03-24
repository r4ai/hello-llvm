#include <stdio.h>

int stack[30000];
int top = -1;

void push(int value) { stack[++top] = value; }

int pop() { return stack[top--]; }

int main() {
  int idx = 0;

  int dataIdx = ++idx;
  int ptrIdx = ++idx;

  // clang-format off
  printf("define dso_local i32 @main() {\n");
  printf("  %%%d = alloca ptr, align 8\n", dataIdx);  // char *data
  printf("  %%%d = alloca ptr, align 8\n", ptrIdx);  // char *ptr

  // data = (char *)calloc(30000, sizeof(char));
  printf("  %%%d = call noalias ptr @calloc(i64 noundef 30000, i64 noundef 1)\n", ++idx);
  printf("  store ptr %%%d, ptr %%%d, align 8\n", idx, dataIdx);

  // ptr = data;
  printf("  store ptr %%%d, ptr %%%d, align 8\n", idx, ptrIdx);
  // clang-format on

  int bracketCount = 0;
  char c;
  while ((c = getchar()) != EOF) {
    switch (c) {
    case '>': {
      int tmpPtrIdx = ++idx;
      int incrementedTmpPtrIdx = ++idx;
      printf("  %%%d = load ptr, ptr %%%d, align 8\n", tmpPtrIdx, ptrIdx);
      printf("  %%%d = getelementptr inbounds i8, ptr %%%d, i32 1\n",
             incrementedTmpPtrIdx, tmpPtrIdx);
      printf("  store ptr %%%d, ptr %%%d, align 8\n", incrementedTmpPtrIdx,
             ptrIdx);
      break;
    }
    case '<': {
      int tmpPtrIdx = ++idx;
      int decrementedTmpPtrIdx = ++idx;
      printf("  %%%d = load ptr, ptr %%%d, align 8\n", tmpPtrIdx, ptrIdx);
      printf("  %%%d = getelementptr inbounds i8, ptr %%%d, i32 -1\n",
             decrementedTmpPtrIdx, tmpPtrIdx);
      printf("  store ptr %%%d, ptr %%%d, align 8\n", decrementedTmpPtrIdx,
             ptrIdx);
      break;
    }
    case '+': {
      int tmpPtrIdx = ++idx;
      int valueIdx = ++idx;
      int incrementedValueIdx = ++idx;
      printf("  %%%d = load ptr, ptr %%%d, align 8\n", tmpPtrIdx, ptrIdx);
      printf("  %%%d = load i8, ptr %%%d, align 1\n", valueIdx, tmpPtrIdx);
      printf("  %%%d = add i8 %%%d, 1\n", incrementedValueIdx, valueIdx);
      printf("  store i8 %%%d, ptr %%%d, align 1\n", incrementedValueIdx,
             tmpPtrIdx);
      break;
    }
    case '-': {
      int tmpPtrIdx = ++idx;
      int valueIdx = ++idx;
      int decrementedValueIdx = ++idx;
      printf("  %%%d = load ptr, ptr %%%d, align 8\n", tmpPtrIdx, ptrIdx);
      printf("  %%%d = load i8, ptr %%%d, align 1\n", valueIdx, tmpPtrIdx);
      printf("  %%%d = sub i8 %%%d, 1\n", decrementedValueIdx, valueIdx);
      printf("  store i8 %%%d, ptr %%%d, align 1\n", decrementedValueIdx,
             tmpPtrIdx);
      break;
    }
    case '.': {
      int tmpPtrIdx = ++idx;
      int valueIdx = ++idx;
      int valueI32Idx = ++idx;
      printf("  %%%d = load ptr, ptr %%%d, align 8\n", tmpPtrIdx, ptrIdx);
      printf("  %%%d = load i8, ptr %%%d, align 1\n", valueIdx, tmpPtrIdx);
      printf("  %%%d = sext i8 %%%d to i32\n", valueI32Idx, valueIdx);
      printf("  %%%d = call i32 @putchar(i32 %%%d)\n", ++idx, valueI32Idx);
      break;
    }
    case '[': {
      push(++bracketCount);
      printf("  br label %%loop-cond-%d\n", bracketCount);
      printf("\n");
      printf("loop-cond-%d:\n", bracketCount);
      int tmpPtrIdx = ++idx;
      printf("  %%%d = load ptr, ptr %%%d, align 8\n", tmpPtrIdx, ptrIdx);
      int valueIdx = ++idx;
      printf("  %%%d = load i8, ptr %%%d, align 1\n", valueIdx, tmpPtrIdx);
      int condIdx = ++idx;
      printf("  %%%d = icmp ne i8 %%%d, 0\n", condIdx, valueIdx);
      printf("  br i1 %%%d, label %%loop-body-%d, label %%loop-end-%d\n",
             condIdx, bracketCount, bracketCount);
      printf("\n");
      printf("loop-body-%d:\n", bracketCount);
      break;
    }
    case ']': {
      int count = pop();
      printf("  br label %%loop-cond-%d\n", count);
      printf("\n");
      printf("loop-end-%d:\n", count);
      break;
    }
    }
  }

  printf("  %%%d = load ptr, ptr %%%d, align 8\n", ++idx, dataIdx);
  printf("  call void @free(ptr noundef %%%d)\n", idx);
  printf("  ret i32 0\n");
  printf("}\n\n");

  // calloc
  printf("declare noalias ptr @calloc(i64 noundef, i64 noundef)\n\n");

  // free
  printf("declare void @free(ptr noundef)\n\n");

  // putchar
  printf("declare i32 @putchar(i32 noundef)\n");

  return 0;
}
