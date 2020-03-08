cmake_minimum_required (VERSION 2.6)
project (vfuzz-runtime)

set(ExternalIncludeDirectories
    ../include
    ../libvfuzz-core/src
    ../third_party/fuzzing-headers/include/)
set(InternalIncludeDirectories
    src/
    ../third_party/cxxopts/include/
    ${ExternalIncludeDirectories})

set(LibvfuzzSources
    src/container/corpus/manager.cpp
    src/container/corpus/manager.h
    src/datasource/datasource.cpp
    src/datasource/datasource.h
    src/exec/execmanager.cpp
    src/exec/execmanager.h
    src/exec/executor.cpp
    src/exec/executor.h
    src/interface/callbacks.cpp
    src/interface/extfunctions.cpp
    src/interface/extfunctions.def
    src/interface/extfunctions.h
    src/interface/malloc_hooks.cpp
    src/interface/malloc_hooks.h
    src/main.cpp
    src/options/addrules.cpp
    src/options/addrules.h
    src/options/options.cpp
    src/options/options.h
    src/sensor/driver.cpp
    src/sensor/driver.h
    src/util/mallocfreetracer.h)

set_source_files_properties(src/interface/malloc_hooks.cpp PROPERTIES COMPILE_FLAGS -Wno-deprecated-declarations)

add_library(vfuzz-runtime STATIC ${LibvfuzzSources})
target_compile_options(vfuzz-runtime PRIVATE -DFUZZING_HEADERS_NO_IMPL)
target_include_directories(vfuzz-runtime PRIVATE ${InternalIncludeDirectories})
