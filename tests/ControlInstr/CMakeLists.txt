# unreachable
if(USE_SYSCALL)
add_test(NAME "unreachable"
	COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/../runner.sh ${CMAKE_BINARY_DIR} unreachable.wasm ${CMAKE_CURRENT_LIST_DIR}/unreachable_syscall.expect
)
else(USE_SYSCALL)
add_test(NAME "unreachable"
	COMMAND ${CMAKE_BINARY_DIR}/WasmVM "${CMAKE_BINARY_DIR}/tests/unreachable.wasm"
)	
endif(USE_SYSCALL)