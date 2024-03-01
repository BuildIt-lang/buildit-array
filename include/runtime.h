#ifndef BARRAY_RUNTIME_H
#define BARRAY_RUNTIME_H

#include "builder/dyn_var.h"

namespace barray {
namespace runtime {

extern builder::dyn_var<void* (int)> malloc;
extern builder::dyn_var<void* (int)> free;


}
}

#endif


