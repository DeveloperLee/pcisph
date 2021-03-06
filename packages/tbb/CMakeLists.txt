cmake_minimum_required (VERSION 2.8)
project(tbb CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

include_directories(include src src/rml/include)

option(TBB_BUILD_SHARED          "Build TBB shared library" ON)
option(TBB_BUILD_STATIC          "Build TBB static library" ON)
option(TBB_BUILD_TBBMALLOC       "Build TBB malloc library" ON)
option(TBB_BUILD_TBBMALLOC_PROXY "Build TBB malloc proxy library" ON)

if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
endif()

file(GLOB tbb_src "${CMAKE_CURRENT_SOURCE_DIR}/src/tbb/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/old/*.cpp")
list(APPEND tbb_src ${CMAKE_CURRENT_SOURCE_DIR}/src/rml/client/rml_tbb.cpp)
file(GLOB to_remove "${CMAKE_CURRENT_SOURCE_DIR}/src/old/test*.cpp")
list(REMOVE_ITEM tbb_src ${to_remove})

set(tbbmalloc_static_src
  src/tbbmalloc/backend.cpp
  src/tbbmalloc/large_objects.cpp
  src/tbbmalloc/backref.cpp
  src/tbbmalloc/tbbmalloc.cpp
  src/tbbmalloc/frontend.cpp
  src/tbb/itt_notify.cpp)

set(tbbmalloc_src ${tbbmalloc_static_src})

set(tbbmalloc_proxy_src
  src/tbbmalloc/proxy.cpp
  src/tbbmalloc/tbb_function_replacement.cpp)

if (NOT APPLE)
  add_definitions(-DDO_ITT_NOTIFY)
else()
  # Disable annoying "has no symbols" warnings
  set(CMAKE_AR ${CMAKE_CURRENT_SOURCE_DIR}/build/ar_filter.sh)
  set(CMAKE_RANLIB ${CMAKE_CURRENT_SOURCE_DIR}/build/ranlib_filter.sh)
endif()

if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DUSE_PTHREAD")
  if(NOT CMAKE_CXX_FLAGS MATCHES "-mno-rtm")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mrtm")
  endif()
  if (APPLE)
    set(ARCH_PREFIX "mac")
  else()
    set(ARCH_PREFIX "lin")
  endif()
  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_PREFIX "${ARCH_PREFIX}64")
  else()
    set(ARCH_PREFIX "${ARCH_PREFIX}32")
  endif()
  set(ENABLE_RTTI "-frtti -fexceptions ")
  set(DISABLE_RTTI "-fno-rtti -fno-exceptions ")
elseif(WIN32)
  cmake_minimum_required (VERSION 3.1)
  enable_language(ASM_MASM)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GS- /Zc:wchar_t /Zc:forScope /DUSE_WINTHREAD")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_CRT_SECURE_NO_DEPRECATE /D_WIN32_WINNT=0x0600 /volatile:iso")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4267 /wd4800 /wd4146 /wd4244 /wd4018")

  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    list(APPEND tbb_src src/tbb/intel64-masm/atomic_support.asm
      src/tbb/intel64-masm/itsx.asm src/tbb/intel64-masm/intel64_misc.asm)
    list(APPEND tbbmalloc_src src/tbb/intel64-masm/atomic_support.asm)
    set(CMAKE_ASM_MASM_FLAGS "/DEM64T=1")
    set(ARCH_PREFIX "win64")
  else()
    list(APPEND tbb_src src/tbb/ia32-masm/atomic_support.asm
      src/tbb/ia32-masm/itsx.asm src/tbb/ia32-masm/lock_byte.asm)
    list(APPEND tbbmalloc_src src/tbb/ia32-masm/atomic_support.asm)
    set(ARCH_PREFIX "win32")
  endif()
  set(ENABLE_RTTI "/EHsc /GR ")
  set(DISABLE_RTTI "/EHs- /GR- ")
endif()

# Linker export definitions
if (WIN32)
  add_custom_command(OUTPUT tbb.def
    COMMAND ${CMAKE_CXX_COMPILER} /TC /EP ${CMAKE_CURRENT_SOURCE_DIR}/src/tbb/${ARCH_PREFIX}-tbb-export.def  -I ${CMAKE_CURRENT_SOURCE_DIR}/include > tbb.def
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/src/tbb/${ARCH_PREFIX}-tbb-export.def
    COMMENT "Preprocessing tbb.def"
  )

  add_custom_command(OUTPUT tbbmalloc.def
    COMMAND ${CMAKE_CXX_COMPILER} /TC /EP ${CMAKE_CURRENT_SOURCE_DIR}/src/tbbmalloc/${ARCH_PREFIX}-tbbmalloc-export.def  -I ${CMAKE_CURRENT_SOURCE_DIR}/include >   tbbmalloc.def
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/src/tbbmalloc/${ARCH_PREFIX}-tbbmalloc-export.def
    COMMENT "Preprocessing tbbmalloc.def"
  )
else()
  add_custom_command(OUTPUT tbb.def
    COMMAND ${CMAKE_CXX_COMPILER} -xc++ -E ${CMAKE_CURRENT_SOURCE_DIR}/src/tbb/${ARCH_PREFIX}-tbb-export.def  -I ${CMAKE_CURRENT_SOURCE_DIR}/include -o tbb.def
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/src/tbb/${ARCH_PREFIX}-tbb-export.def
    COMMENT "Preprocessing tbb.def"
  )

  add_custom_command(OUTPUT tbbmalloc.def
    COMMAND ${CMAKE_CXX_COMPILER} -xc++ -E ${CMAKE_CURRENT_SOURCE_DIR}/src/tbbmalloc/${ARCH_PREFIX}-tbbmalloc-export.def  -I ${CMAKE_CURRENT_SOURCE_DIR}/include -o   tbbmalloc.def
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/src/tbbmalloc/${ARCH_PREFIX}-tbbmalloc-export.def
    COMMENT "Preprocessing tbbmalloc.def"
  )
endif()

add_custom_target(tbb_def_files DEPENDS tbb.def tbbmalloc.def)

# TBB library
if (TBB_BUILD_STATIC)
  add_library(tbb_static STATIC ${tbb_src})
  set_property(TARGET tbb_static APPEND PROPERTY COMPILE_DEFINITIONS "__TBB_BUILD=1")
  set_property(TARGET tbb_static APPEND_STRING PROPERTY COMPILE_FLAGS ${ENABLE_RTTI})
  install(TARGETS tbb_static ARCHIVE DESTINATION lib)
endif()

if (TBB_BUILD_SHARED)
  add_library(tbb SHARED ${tbb_src})
  set_property(TARGET tbb APPEND PROPERTY COMPILE_DEFINITIONS "__TBB_BUILD=1")
  set_property(TARGET tbb APPEND_STRING PROPERTY COMPILE_FLAGS ${ENABLE_RTTI})
  add_dependencies(tbb tbb_def_files)
  if (APPLE)
    set_property(TARGET tbb APPEND PROPERTY LINK_FLAGS "-Wl,-exported_symbols_list,tbb.def")
  elseif(UNIX)
    set_property(TARGET tbb APPEND PROPERTY LINK_FLAGS "-Wl,-version-script,tbb.def")
  elseif(WIN32)
    set_property(TARGET tbb APPEND PROPERTY LINK_FLAGS "/DEF:tbb.def")
  endif()
  install(TARGETS tbb DESTINATION lib)
endif()

if(CMAKE_COMPILER_IS_GNUCC)
  # Quench a warning on GCC
  set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/tbb/governor.cpp COMPILE_FLAGS "-Wno-missing-field-initializers ")
elseif(MSVC)
  # Quench a warning on MSVC
  set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/tbb/scheduler.cpp COMPILE_FLAGS "/wd4458 ")
endif()

if(TBB_BUILD_TBBMALLOC)
  # TBB malloc library
  if (TBB_BUILD_STATIC)
    add_library(tbbmalloc_static STATIC ${tbbmalloc_static_src})
    set_property(TARGET tbbmalloc_static APPEND PROPERTY COMPILE_DEFINITIONS "__TBBMALLOC_BUILD=1")
    set_property(TARGET tbbmalloc_static APPEND_STRING PROPERTY COMPILE_FLAGS ${DISABLE_RTTI})
    install(TARGETS tbbmalloc_static ARCHIVE DESTINATION lib)
  endif()

  if (TBB_BUILD_SHARED)
    add_library(tbbmalloc SHARED ${tbbmalloc_src})
    set_property(TARGET tbbmalloc APPEND PROPERTY COMPILE_DEFINITIONS "__TBBMALLOC_BUILD=1")
    set_property(TARGET tbbmalloc APPEND_STRING PROPERTY COMPILE_FLAGS ${DISABLE_RTTI})
    add_dependencies(tbbmalloc tbb_def_files)
    if (APPLE)
      set_property(TARGET tbbmalloc APPEND PROPERTY LINK_FLAGS "-Wl,-exported_symbols_list,tbbmalloc.def")
    elseif(UNIX)
      set_property(TARGET tbbmalloc APPEND PROPERTY LINK_FLAGS "-Wl,-version-script,tbbmalloc.def")
    elseif(WIN32)
      set_property(TARGET tbbmalloc APPEND PROPERTY LINK_FLAGS "/DEF:tbbmalloc.def")
    endif()
    install(TARGETS tbbmalloc DESTINATION lib)
  endif()
endif()

if(TBB_BUILD_TBBMALLOC_PROXY)
  # TBB malloc proxy library
  if (TBB_BUILD_STATIC)
    add_library(tbbmalloc_proxy_static STATIC ${tbbmalloc_proxy_src})
    set_property(TARGET tbbmalloc_proxy_static APPEND PROPERTY COMPILE_DEFINITIONS "__TBBMALLOC_BUILD=1")
    set_property(TARGET tbbmalloc_proxy_static APPEND_STRING PROPERTY COMPILE_FLAGS ${DISABLE_RTTI})
    link_libraries(tbbmalloc_proxy_static tbbmalloc)
    install(TARGETS tbbmalloc_proxy_static ARCHIVE DESTINATION lib)
  endif()

  if (TBB_BUILD_SHARED)
    add_library(tbbmalloc_proxy SHARED ${tbbmalloc_proxy_src})
    set_property(TARGET tbbmalloc_proxy APPEND PROPERTY COMPILE_DEFINITIONS "__TBBMALLOC_BUILD=1")
    set_property(TARGET tbbmalloc_proxy APPEND_STRING PROPERTY COMPILE_FLAGS ${DISABLE_RTTI})
    link_libraries(tbbmalloc_proxy tbbmalloc)
    install(TARGETS tbbmalloc_proxy DESTINATION lib)
  endif()
endif()

install(DIRECTORY include/tbb DESTINATION include)
