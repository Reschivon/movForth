
cmake_minimum_required(VERSION 3.16)

project(movForth)

set(CMAKE_CXX_STANDARD 17)

# Now build our tools
add_executable(movForth

        main.cpp

        sources/Print.cpp
        sources/SystemExec.cpp

        sources/Interpretation/iWord.cpp
        sources/Interpretation/iData.cpp
        sources/Interpretation/Interpreter.cpp

        sources/Symbolic/sWord.cpp
        sources/Symbolic/Instruction.cpp
        sources/Symbolic/Block.cpp
        sources/Symbolic/Pass.cpp
        sources/Symbolic/Pass_TranslateToBasicBlocks.cpp
        sources/Symbolic/Pass_BBStackGraph.cpp
        sources/Symbolic/Pass_ComputeMatchingPairs.cpp
        sources/Symbolic/Pass_WordStackGraph.cpp

        sources/Generation/IRGenerator.cpp
        sources/Generation/FBuilder.cpp
        sources/Generation/ObjectGenerator.cpp

        #        DirtyPass/Inliner.cpp
        #        DirtyPass/IPO.h
        #        DirtyPass/InlineSimple.cpp
        #        DirtyPass/LegacyPassManager.cpp
        )


SET(LLVM13 "/home/reschivon/Hard-Drive/clang+llvm-12.0.1-x86_64-linux-gnu-ubuntu-")
# add_library(LLVM "${LLVM13}")
find_package(LLVM REQUIRED CONFIG PATHS)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs
        # support core irreader native passes codegen
        aarch64asmparser aarch64asmprinter aarch64codegen aarch64desc aarch64disassembler aarch64info
        aarch64utils amdgpuasmparser amdgpuasmprinter amdgpucodegen amdgpudesc
        amdgpudisassembler amdgpuinfo amdgpuutils analysis armasmparser armasmprinter armcodegen armdesc
        armdisassembler arminfo armutils asmparser asmprinter  avrasmparser avrasmprinter avrcodegen avrdesc
        avrdisassembler avrinfo binaryformat bitreader bitwriter bpfasmparser bpfasmprinter bpfcodegen
        bpfdesc bpfdisassembler bpfinfo codegen core coroutines coverage debuginfocodeview debuginfodwarf

        # executionengine engine

        debuginfomsf debuginfopdb demangle dlltooldriver  fuzzmutate globalisel
        hexagonasmparser hexagoncodegen hexagondesc hexagondisassembler hexagoninfo instcombine instrumentation
        interpreter ipo irreader lanaiasmparser lanaiasmprinter lanaicodegen lanaidesc lanaidisassembler
        lanaiinfo libdriver lineeditor linker lto mc mcdisassembler mcjit mcparser mipsasmparser mipsasmprinter
        mipscodegen mipsdesc mipsdisassembler mipsinfo mirparser msp430asmprinter msp430codegen msp430desc
        msp430info native nativecodegen nvptxasmprinter nvptxcodegen nvptxdesc nvptxinfo objcarcopts object
        objectyaml option orcjit passes powerpcasmparser powerpcasmprinter powerpccodegen powerpcdesc
        powerpcdisassembler powerpcinfo profiledata runtimedyld scalaropts selectiondag sparcasmparser
        sparcasmprinter sparccodegen sparcdesc sparcdisassembler sparcinfo support symbolize
        systemzasmparser systemzasmprinter systemzcodegen systemzdesc systemzdisassembler systemzinfo tablegen
        target transformutils vectorize webassemblyasmprinter webassemblycodegen webassemblydesc
        webassemblydisassembler webassemblyinfo windowsmanifest x86asmparser x86asmprinter x86codegen x86desc
        x86disassembler x86info x86utils xcoreasmprinter xcorecodegen xcoredesc xcoredisassembler xcoreinfo

        # all-targets
)

# execute_process(COMMAND llvm-config --libs all OUTPUT_VARIABLE llvm_libraries)

# Link against LLVM libraries
message(STATUS "Linking LLVM libraries ${llvm_libs}")
target_link_libraries(movForth ${llvm_libs})

SET(LLVM_ENABLE_DUMP ON)
