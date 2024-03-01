#ifndef BARRAY_RUNTIME_H
#define BARRAY_RUNTIME_H

#include "builder/dyn_var.h"

namespace barray {
namespace runtime {

extern builder::dyn_var<void* (int)> malloc;
extern builder::dyn_var<void* (int)> free;

extern builder::dyn_var<void* (int)> cuda_malloc;
extern builder::dyn_var<void* (int)> cuda_free;

extern builder::dyn_var<void (void*, void*, int)> cuda_memcpy_to_device;
extern builder::dyn_var<void (void*, void*, int)> cuda_memcpy_to_host;

}
}

#endif


