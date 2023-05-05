#i have never used cmake before send help

cmake_minimum_required(VERSION 3.16)
set(JQR_ITEM "3.8")
project(${JQR_ITEM} "C")

set(ITEM_HEADERS ${JQR_ITEM}_headers)
set(ITEM_SUBMISSION ${JQR_ITEM}_submission)
set(ITEM_TESTS ${JQR_ITEM}_test)
set(ITEM_EXEC ${JQR_ITEM}_exe)

add_library(${ITEM_SUBMISSION} SHARED "src/serialize.c" "include/serialize.h")

add_library(${ITEM_TESTS} SHARED "tests/general_tests.h" "tests/partial_write_tests.h" "tests/partial_write_tests.c" "tests/general_tests.c" "tests/edge_cases.h"   "tests/edge_cases.c"  "tests/helper_functions.c" "tests/helper_functions.h")


# add the executable
add_executable(${ITEM_EXEC} tests/main_tests.c "src/serialize.c")

target_link_libraries(${ITEM_EXEC} ${ITEM_SUBMISSION} ${ITEM_TESTS} cunit rt)
target_compile_options(${ITEM_EXEC} PUBLIC "-Wall" "-Wextra" "-g")
add_test(${JQR_ITEM}_tests                ${ITEM_EXEC})
