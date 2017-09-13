# unreachable
if(USE_SYSCALL)
add_test(NAME "unreachable_syscall"
	COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/../runner.sh ${CMAKE_BINARY_DIR} unreachable.wasm ${CMAKE_CURRENT_LIST_DIR}/unreachable_syscall.expect
)
else(USE_SYSCALL)
if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
add_test(NAME "unreachable"
	COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/../runner.sh ${CMAKE_BINARY_DIR} unreachable.wasm ${CMAKE_CURRENT_LIST_DIR}/unreachable_debug.expect
)
else(${CMAKE_BUILD_TYPE} STREQUAL Debug)
add_test(NAME "unreachable"
	COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/../runner.sh ${CMAKE_BINARY_DIR} unreachable.wasm ${CMAKE_CURRENT_LIST_DIR}/unreachable_release.expect
)
endif(${CMAKE_BUILD_TYPE} STREQUAL Debug)
endif(USE_SYSCALL)
# nop
add_test(NAME "nop"
	COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/../runner.sh ${CMAKE_BINARY_DIR} nop.wasm ${CMAKE_CURRENT_LIST_DIR}/nop.expect
)