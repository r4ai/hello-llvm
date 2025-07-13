// https://qiita.com/uint256_t/items/7d4556cb8f5997b9e95c#%E6%94%AF%E9%85%8D%E6%9C%A8-dominator-tree

int main() {
  int a, b;

  // bb 1
  a = 1;

  if (a < 1) {
    // bb 2
    if (a < 2) {
      // bb 3
      a = 2;
    } else {
      // bb 4
      a = 3;
    }
  } else {
    // bb 5
    a = 3;
  }

  // bb 6
  b = a;
}
