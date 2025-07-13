
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <unordered_map>
#include <vector>

using namespace llvm;

namespace {

// 値番号を表す型
using ValueNumber = unsigned;

// 命令のハッシュキー（オペコードとオペランドの値番号の組）
struct ExpressionKey {
  unsigned opcode;
  std::vector<ValueNumber> operands;

  bool operator==(const ExpressionKey &other) const {
    return opcode == other.opcode && operands == other.operands;
  }
};

// ExpressionKeyのハッシュ関数
struct ExpressionKeyHash {
  size_t operator()(const ExpressionKey &key) const {
    size_t hash = std::hash<unsigned>()(key.opcode);
    for (auto num : key.operands) {
      hash ^= std::hash<ValueNumber>()(num) + 0x9e3779b9 + (hash << 6) +
              (hash >> 2);
    }
    return hash;
  }
};

// 新しいパスマネージャー用のパス実装
class LocalValueNumberingPass : public PassInfoMixin<LocalValueNumberingPass> {
private:
  // 値から値番号へのマッピング
  std::unordered_map<Value *, ValueNumber> value_to_number;
  // 式から値番号へのマッピング
  std::unordered_map<ExpressionKey, ValueNumber, ExpressionKeyHash>
      expr_to_number;
  // 値番号から代表値へのマッピング
  std::unordered_map<ValueNumber, Value *> number_to_value;
  // 次に割り当てる値番号
  ValueNumber next_number;

public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    bool Changed = false;

    // 各基本ブロックに対してLVNを実行
    for (BasicBlock &BB : F) {
      Changed |= processBasicBlock(BB);
    }

    if (Changed) {
      // CFGは変更しないが、命令は変更される
      PreservedAnalyses PA;
      PA.preserveSet<CFGAnalyses>();
      return PA;
    }

    return PreservedAnalyses::all();
  }

  // デバッグ用の静的名前
  static StringRef name() { return "LocalValueNumberingPass"; }

private:
  bool processBasicBlock(BasicBlock &basic_block) {
    bool changed = false;

    // 状態をリセット
    value_to_number.clear();
    expr_to_number.clear();
    number_to_value.clear();
    next_number = 1;

    // 削除予定の命令を記録
    std::vector<Instruction *> to_delete_instructions;

    // 基本ブロック内の各命令を処理
    for (auto &instruction : basic_block) {
      // サポートする命令タイプをチェック
      if (!isSupportedInstruction(&instruction)) {
        // サポートしない命令には新しい値番号を割り当て
        assignNewNumber(&instruction);
        continue;
      }

      // 命令の式キーを作成
      auto key = createExpressionKey(&instruction);

      // 同じ式が既に存在するかチェック
      auto it = expr_to_number.find(key);
      if (it != expr_to_number.end()) {
        // 冗長な計算を発見
        auto existing_num = it->second;
        auto *existing_value = number_to_value[existing_num];

        // 命令を既存の値で置換
        instruction.replaceAllUsesWith(existing_value);
        to_delete_instructions.push_back(&instruction);
        changed = true;

        errs() << "(llvm-tutor) Found redundant instruction: " << instruction
               << " replaced with value number: " << existing_num << "\n";
      } else {
        // 新しい式として登録
        auto new_num = assignNewNumber(&instruction);
        expr_to_number[key] = new_num;
      }
    }

    // 冗長な命令を削除
    for (Instruction *instruction : to_delete_instructions) {
      instruction->eraseFromParent();
    }

    return changed;
  }

  bool isSupportedInstruction(Instruction *I) {
    // シンプルな実装のため、基本的な二項演算と比較演算のみサポート
    return isa<BinaryOperator>(I) || isa<CmpInst>(I);
  }

  ExpressionKey createExpressionKey(Instruction *I) {
    ExpressionKey key;
    key.opcode = I->getOpcode();

    // オペランドの値番号を取得
    for (auto &U : I->operands()) {
      auto *V = U.get();
      auto num = getOrAssignNumber(V);
      key.operands.push_back(num);
    }

    // 可換演算の場合、オペランドをソート（正規化）
    if (I->isCommutative() && key.operands.size() == 2) {
      if (key.operands[0] > key.operands[1]) {
        std::swap(key.operands[0], key.operands[1]);
      }
    }

    return key;
  }

  ValueNumber getOrAssignNumber(Value *value) {
    auto it = value_to_number.find(value);
    if (it != value_to_number.end()) {
      return it->second;
    }
    return assignNewNumber(value);
  }

  ValueNumber assignNewNumber(Value *value) {
    ValueNumber num = next_number++;
    value_to_number[value] = num;
    number_to_value[num] = value;
    return num;
  }
};

} // anonymous namespace

// パスの登録用のコード
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

// パスをパスビルダーに登録
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "SimpleLVN", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            // パイプライン解析コールバックを登録
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "simple-lvn") {
                    FPM.addPass(LocalValueNumberingPass());
                    return true;
                  }
                  return false;
                });

            // 解析結果の出力用コールバックを登録
            PB.registerAnalysisRegistrationCallback(
                [](FunctionAnalysisManager &FAM) {
                  // 必要に応じて解析パスを登録
                });

            // 最適化パイプラインへの自動挿入（オプション）
            PB.registerPeepholeEPCallback(
                [](FunctionPassManager &FPM, OptimizationLevel Level) {
                  if (Level != OptimizationLevel::O0) {
                    FPM.addPass(LocalValueNumberingPass());
                  }
                });
          }};
}
