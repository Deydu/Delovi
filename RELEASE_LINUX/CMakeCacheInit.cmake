set(CMAKE_BUILD_TYPE "Release" CACHE STRING " explicitly set by deydu" FORCE)
set(CMAKE_CXX_COMPILER "g++" CACHE FILEPATH " explicitly set by deydu" FORCE)
set(CMAKE_CXX_FLAGS "-pipe -march=native -O2 -DNDEBUG -DLINUX -fPIC -std=c++11 -Wfatal-errors -Wall -Weffc++ -fno-diagnostics-show-caret" CACHE STRING " explicitly set by deydu" FORCE)
set(CMAKE_C_COMPILER "gcc" CACHE FILEPATH " explicitly set by deydu" FORCE)
set(CMAKE_C_FLAGS "-pipe -march=native -O2 -DNDEBUG -DLINUX -fPIC -Wfatal-errors -Wall -pedantic -fno-diagnostics-show-caret" CACHE STRING " explicitly set by deydu" FORCE)
#set(CMAKE_VERBOSE_MAKEFILE "TRUE" CACHE BOOL " explicitly set by deydu" FORCE)
set(CMAKE_VERBOSE_MAKEFILE "FALSE" CACHE BOOL " explicitly set by deydu" FORCE)
