
#ifndef MOVFORTH_IPO_H
#define MOVFORTH_IPO_H

#include <functional>
#include <vector>

namespace llvm {

    struct InlineParams;
    class StringRef;
    class ModuleSummaryIndex;
    class ModulePass;
    class Pass;
    class Function;
    class BasicBlock;
    class GlobalValue;
    class raw_ostream;


//===----------------------------------------------------------------------===//
/// createFunctionInliningPass - Return a new pass object that uses a heuristic
/// to inline direct function calls to small functions.
///
/// The Threshold can be passed directly, or asked to be computed from the
/// given optimization and size optimization arguments.
///
/// The -inline-threshold command line option takes precedence over the
/// threshold given here.

    Pass *createFunctionInliningPass();
    Pass *createFunctionInliningPass(int Threshold);
    Pass *createFunctionInliningPass(unsigned OptLevel, unsigned SizeOptLevel,
                                     bool DisableInlineHotCallSite);
    Pass *createFunctionInliningPass(InlineParams &Params);

} // End llvm namespace

#endif //MOVFORTH_IPO_H
