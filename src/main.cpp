#include <memory>

#include "options"
#include "game"

using namespace std;
using namespace zlynx::minesweeper;

int main() {
	// Set up the game
	auto options = get_game_options();
	auto game = make_unique<Game>(options);
	// Run the game
	while( game->is_running() ) {
		game->tick();
	}
	// Exit the game
	return 0;
}
