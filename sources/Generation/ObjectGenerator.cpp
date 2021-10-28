
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"


#include "../../headers/Generation/ObjectGenerator.h"
// #include "../../DirtyPass/LegacyPassManager.h"
#include "llvm/IR/LegacyPassManager.h"
#include "../../headers/SystemExec.h"
#include "../../headers/Print.h"
#include "../../headers/Generation/IRGenerator.h"

using namespace mov;
using namespace llvm;

bool ObjectGenerator::generate(const std::string &filename, IRModule mod) {
    Module &module = *mod.get_module().get();

    auto TargetTriple = sys::getDefaultTargetTriple();

    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    std::string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!Target) {
        println(Error);
        return true;
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    module.setDataLayout(TargetMachine->createDataLayout());
    module.setTargetTriple(TargetTriple);

    std::error_code EC;
    raw_fd_ostream dest(filename, EC, sys::fs::OpenFlags::F_None);

    if (EC) {
       println("Could not open file: ", EC.message());
        return true;
    }

    legacy::PassManager pass;
    // auto FileType = TargetMachine::CGFT_ObjectFile;
    auto FileType = TargetMachine::CGFT_AssemblyFile;

    if (TargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
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
