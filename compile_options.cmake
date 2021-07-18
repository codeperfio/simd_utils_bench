# Disable frame pointer optimizations so profilers can get better call stacks
add_compile_options(-fno-omit-frame-pointer)

if(CLR_CMAKE_PLATFORM_UNIX)
  # this allows pretty-printers in gdb to work with clang

  add_compile_options(-Wno-unknown-warning-option)
  add_definitions(-DFMA)
  add_definitions(-DSSE)
  # add_compile_options(-msse4.2)

  add_compile_options(-flax-vector-conversions)

  if(C_AVX_FOUND)
    add_definitions(-DAVX)
    add_compile_options(-mavx2)
  endif()

  if(C_AVX512_FOUND)
    add_definitions(-DAVX512)
    add_compile_options(-march=skylake-avx512)
  endif()

endif(CLR_CMAKE_PLATFORM_UNIX)

if(CLR_CMAKE_PLATFORM_UNIX_ARM)
  add_compile_options(-mthumb)
  add_compile_options(-mfpu=vfpv3)
  add_compile_options(-march=armv7-a)
  add_definitions(-DARM)
  add_definitions(-DSSE)
  if(ARM_SOFTFP)
    add_definitions(-DARM_SOFTFP)
    add_compile_options(-mfloat-abi=softfp)
  endif(ARM_SOFTFP)
endif()

if(CLR_CMAKE_PLATFORM_UNIX_ARM64)
  add_definitions(-DARM)
  add_definitions(-DFMA)
  add_definitions(-DSSE)
  add_compile_options(-flax-vector-conversions)
  add_compile_options(-march=armv8-a+fp+simd+crypto+crc)
endif()
