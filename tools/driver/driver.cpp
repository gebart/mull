#include "Driver.h"
#include "ConfigParser.h"
#include "Config.h"
#include "ModuleLoader.h"

#include "SimpleTest/SimpleTestFinder.h"
#include "SimpleTest/SimpleTestRunner.h"

#include "GoogleTest/GoogleTestFinder.h"
#include "GoogleTest/GoogleTestRunner.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/YAMLParser.h"
#include "llvm/Support/TargetSelect.h"

using namespace Mutang;
using namespace llvm;

static const char *ExecutionResultToString(ExecutionStatus Status) {
  switch (Status) {
    case Failed:
      return "Failed";
    case Passed:
      return "Passed";
    case Invalid:
      llvm_unreachable("Must not reach here");
      break;
  }

  llvm_unreachable("Must not reach here");
}

int main(int argc, char *argv[]) {
  assert(argc == 2);

  ConfigParser Parser;
  auto Cfg = Parser.loadConfig(argv[1]);

  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  LLVMContext Ctx;
  ModuleLoader Loader(Ctx);

#if 0
  GoogleTestFinder TestFinder;
  GoogleTestRunner Runner;
#else
  SimpleTestFinder TestFinder;
  SimpleTestRunner Runner;
#endif

  Driver D(*Cfg.get(), Loader, TestFinder, Runner);

  int FailedCount = 0;
  auto Results = D.Run();
  for (auto &R : Results) {

    printf("Result for '%s'\n", R->getTestName().c_str());
    auto TestResult = R->getOriginalTestResult();
    printf("\tOriginal test '%s' in %lld nanoseconds\n", ExecutionResultToString(TestResult.Status), TestResult.RunningTime);

    if (TestResult.Status == Failed) {
      FailedCount++;
    }

    printf("\tMutants:\n");

    for (auto &MR : R->getMutationResults()) {
      auto MutationResult = MR->getExecutionResult();
      printf("\t\tMutant '%s' in %lld nanoseconds\n", ExecutionResultToString(MutationResult.Status), MutationResult.RunningTime);
    }

  }

  printf("Total amount of tests: %lu\n", Results.size());
  printf("Passed: %lu\n", Results.size() - FailedCount);
  printf("Failed: %d\n", FailedCount);

  return 0;
}
