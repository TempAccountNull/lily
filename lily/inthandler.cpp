#include "inthandler.h"
extern "C" void ExecuteCallback(tCallback * callback) { (*callback)(); }