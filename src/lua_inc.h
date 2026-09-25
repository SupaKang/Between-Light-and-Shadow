// The one place that includes Lua. Lua is compiled as C++ (CMakeLists.txt), so no extern "C":
// errors and yields raised inside bindings unwind as exceptions and run C++ destructors.
#pragma once
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
