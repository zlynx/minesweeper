set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_executable(minesweeper
	main.cpp
	game.cpp
)

target_include_directories(minesweeper
	PRIVATE
	../include
)
