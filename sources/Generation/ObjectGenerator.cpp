
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/Host.h"

#include "llvm/IR/LegacyPassManager.h"

#include "../../headers/Generation/ObjectGenerator.h"
#include "../../headers/SystemExec.h"
#include "../../headers/Print.h"
#include "../../headers/Generation/IRGenerator.h"

using namespace mov;
using namespace llvm;

bool ObjectGenerator::generate(const std::string &filename, IRModule mod) {
    Module &module = *mod.get_module();

    auto TargetTriple = sys::getDefaultTargetTriple();
    println("Target Triple Detected as ", TargetTriple);

    // Don't need
//    InitializeAllTargets();
//    InitializeAllTargetInfos();
//    InitializeAllTargetMCs();
//    InitializeAllAsmParsers();
//    InitializeAllAsmPrinters();

// really only need this one
//    LLVMInitializeX86TargetInfo();
//    LLVMInitializeX86TargetMC();
//    LLVMInitializeX86AsmParser();
//    LLVMInitializeX86AsmPrinter();

    InitializeNativeTarget();
    InitializeNativeTargetAsmParser();
    InitializeNativeTargetAsmPrinter();

    std::string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!Target) {
        println(Error);
        return true;
    }

    auto CPU = "skylake";
    auto Features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    module.setDataLayout(TargetMachine->createDataLayout());
    module.setTargetTriple(TargetTriple);

    std::error_code EC;
    raw_fd_ostream dest(filename, EC, sys::fs::OpenFlags::OF_None);

    if (EC) {
       println("Could not open file: ", EC.message());
        return true;
    }

    legacy::PassManager pass;
    // auto FileType = TargetMachine::CGFT_ObjectFile;
    auto FileType = CGFT_AssemblyFile;

    if (TargetMachine->addPassesToEmitFile(pass, dest, &dest, FileType)) {
        println("TargetMachine can't emit a file of this type");
        return true;
    }

    pass.run(module);
    dest.flush();

    return false;
}

void ObjectGenerator::link(const std::string &assembly_name, const std::string &executable_name) {
    println();
    println("==========[Linking]===========");
    auto command = "clang++ " + assembly_name + " -stdlib=libstdc++ -o " + executable_name;
    println(exec(command));
}
