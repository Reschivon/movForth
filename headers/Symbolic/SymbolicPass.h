

#ifndef MOVFORTH_SYM_SYMBOLICPASS_H
#define MOVFORTH_SYM_SYMBOLICPASS_H

#include "../Symbolic/Structures.h"
#include "../Interpretation/Types.h"

namespace sym {
    Wordptr stack_analysis(mfc::Wordptr wordptr);
    Wordptr generate_ir(Wordptr wordptr);
}

#endif
