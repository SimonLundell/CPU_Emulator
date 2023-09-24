cmake_minimum_required(VERSION 3.16.3)

project(M6502Lib)

if(MSVC)
  add_compile_options(/MP) # Multiprocess when building
  add_compile_options(/W4 /wd4201 /WX) # Warning l4, warnings as errors
else()
  add_compile_options(-W -Wall -Werror) # All warnings, all warnings as errors
endif()

# Get all source files
file(GLOB M6502_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# GTEST Stuff
include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
# GTEST Stuff end

add_library(M6502Lib ${M6502_SOURCES})

target_include_directories(M6502Lib PUBLIC "${PROJECT_SOURCE_DIR}/include")

add_executable(M6502Test ${M6502_SOURCES})
add_dependencies(M6502Test M6502Lib)

target_link_libraries(M6502Test gtest_main)
target_link_libraries(M6502Test M6502Lib)
