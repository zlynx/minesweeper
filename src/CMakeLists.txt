add_executable(minesweeper
	main.cpp
	game.cpp
	board.cpp
)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -mcpu=native -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set_property(TARGET minesweeper PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
target_compile_features(minesweeper PUBLIC cxx_std_17)
target_include_directories(minesweeper
	PRIVATE
	../include
)
