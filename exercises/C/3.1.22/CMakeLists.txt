#i have never used cmake before send help

cmake_minimum_required(VERSION 3.16)
set(JQR_ITEM "3.1.22")
project(${JQR_ITEM} "C")

set(ITEM_HEADERS ${JQR_ITEM}_headers)
set(ITEM_SUBMISSION ${JQR_ITEM}_submission)
set(ITEM_EXEC ${JQR_ITEM}_exe)

add_library(${ITEM_SUBMISSION} SHARED "src/game.c")

add_library(${ITEM_TESTS} SHARED "tests/game_test.c"  )


# add the executable

add_executable(${ITEM_EXEC} tests/main_tests.c "tests/game_test.h" "tests/game_test.c" "src/game.c")
target_link_libraries(${ITEM_EXEC} ${ITEM_SUBMISSION} ${ITEM_TESTS} cunit pthread)

target_compile_options(${ITEM_EXEC} PUBLIC "-Wall" "-Wextra" "-g")
#add_test(${JQR_ITEM}_tests                ${ITEM_EXEC})


