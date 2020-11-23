# JunglesOsHelpers

This repo contains helpers for RTOSes. At this stage FreeRTOS is only supported. See below what helpers are currently
implemented.

The repo is structured in such a way that all the headers can be found inside `jungles_os_helpers` directory.

Inside `jungles_os_helpers` you can find dirs:
* `freertos` - for FreeRTOS helpers
* `generic_implementations` - which are not tight to any specific framework.
* `native` - which are helpers that can be run on host-side systems, supporting `std::thread`, 
`std::condition_variable` and such.

## Using the library

The library is meant to be used with `CMake`. The best approach to use it is:

```
include(FetchContent)
FetchContent_Declare(JunglesOsHelpersLib
    GIT_REPOSITORY https://github.com/KKoovalsky/JunglesOsHelpers.git
    GIT_TAG master
)

# Will create an INTERFACE library target called "JunglesOsHelpers"
FetchContent_MakeAvailable(JunglesOsHelpersLib)

```

When using freertos helpers make sure to `target_link_libraries(JunglesOsHelpers ...` with the library target
that includes `FreeRTOS.h`, `task.h`, ... - it must depend on the FreeRTOS kernel itself.
FreeRTOS is not downloaded because the headers may not be used, e.g. you would like to use only the generic 
implementations, or most probably your framework already provides FreeRTOS library. That resolves dependency issue
related to different versions of the same library.

The helpers are nested within corresponding namespaces: `jungles::generic::*`, `jungles::native::*` and 
`jungles::active::*`.

## Running tests

```
mkdir build && cd build
cmake -DJUNGLES_OS_HELPERS_ENABLE_TESTING:BOOL=ON ..
make
ctest
```

## API 

Make sure to check out the `test` directory to find out the capabilities of the helpers.

TODO: this section.
