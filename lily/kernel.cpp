#include "kernel.h"
extern "C" void ExecuteCallback(tCallback * callback) { (*callback)(); }