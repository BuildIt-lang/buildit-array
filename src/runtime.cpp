#include "runtime.h"

namespace barray {
namespace runtime {
builder::dyn_var<void* (int)> malloc = builder::as_global("malloc");
builder::dyn_var<void* (int)> free = builder::as_global("free");
}
}
