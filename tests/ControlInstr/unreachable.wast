(module
  (memory (data "test\n"))
  (func $start
    i32.const 1
	i32.const 0
	i32.const 5
	i32.const 1
	unreachable
	drop
  )
  (start $start)
)