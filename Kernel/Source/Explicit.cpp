// Explicitly instantiate all (most) templates and include every header file to catch any potential errors
// Really, this should be replaced with unit tests... but that's work!

#if !defined(NDEBUG)
#include "Cpu.hpp"
#include "Loader.hpp"
#include "Runtime.hpp"
#include "Sync/Lock.hpp"
#include "Sync/Waiter.hpp"
#endif
