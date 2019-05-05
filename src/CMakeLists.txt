add_executable(minesweeper
	main.cpp
	game.cpp
	board.cpp
)

target_include_directories(minesweeper
	PRIVATE
	../include
)
