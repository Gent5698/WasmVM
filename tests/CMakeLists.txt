set(WASM2WAST ${CMAKE_CURRENT_LIST_DIR}/../third_party/wast2wasm)
add_subdirectory(ControlInstr)
# unreachable
add_custom_target(buildtests ALL
	COMMAND ${WASM2WAST} ${CMAKE_CURRENT_LIST_DIR}/ControlInstr/unreachable.wast -o unreachable.wasm	
)