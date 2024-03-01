#include "runtime.h"

namespace barray {
namespace runtime {
builder::dyn_var<void* (int)> malloc = builder::as_global("malloc");
builder::dyn_var<void* (int)> free = builder::as_global("free");

builder::dyn_var<void* (int)> cuda_malloc = builder::as_global("cuda_malloc");
builder::dyn_var<void* (int)> cuda_free = builder::as_global("cuda_free");

builder::dyn_var<void (void*, void*, int)> cuda_memcpy_to_device = builder::as_global("cuda_memcpy_to_device");
builder::dyn_var<void (void*, void*, int)> cuda_memcpy_to_host = builder::as_global("cuda_memcpy_to_host");
}
}
