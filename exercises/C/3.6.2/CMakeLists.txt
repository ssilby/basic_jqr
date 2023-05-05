#i have never used cmake before send help

cmake_minimum_required(VERSION 3.16)
set(JQR_ITEM "3.6.2")
project(${JQR_ITEM} "C")

set(ITEM_HEADERS ${JQR_ITEM}_headers)
set(ITEM_SUBMISSION ${JQR_ITEM}_submission)
set(ITEM_TESTS ${JQR_ITEM}_test)
set(ITEM_EXEC ${JQR_ITEM}_exe)

add_library(${ITEM_SUBMISSION} SHARED "src/fixme_pls.c" "include/fixme_pls.h")

add_library(${ITEM_TESTS} SHARED "tests/secure_test.c")


# add the executable
add_executable(${ITEM_EXEC} tests/main_tests.c "include/fixme_pls.h" "src/fixme_pls.c")

target_link_libraries(${ITEM_EXEC} ${ITEM_SUBMISSION} ${ITEM_TESTS} cunit)
target_compile_options(${ITEM_EXEC} PUBLIC "-Wall" "-Wextra" "-g" "-O0")
add_test(${JQR_ITEM}_tests                ${ITEM_EXEC})
