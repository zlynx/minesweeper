#include <random>
#include "board"

using namespace std;

namespace zlynx { namespace minesweeper {
	static std::random_device rd;
	static std::mt19937 gen(rd());

	Board::Board(size_t width,  size_t height, size_t mines):
		m_width(width), m_height(height),
		m_mines(mines),
		m_items( (width+2)*(height+2), HIDDEN_CLEAR )
	{
		distribute_mines();
	}

	Board::~Board() {
	}

	size_t Board::count_mines(size_t x, size_t y) const {
		size_t r = 0;
		if(x<1 || x>width()) return r;
		if(y<1 || y>height()) return r;

		r += is_mine(get_hidden(x-1, y-1));
		r += is_mine(get_hidden(x+0, y-1));
		r += is_mine(get_hidden(x+1, y-1));

		r += is_mine(get_hidden(x-1, y+0));
		r += is_mine(get_hidden(x+1, y+0));

		r += is_mine(get_hidden(x-1, y+1));
		r += is_mine(get_hidden(x+0, y+1));
		r += is_mine(get_hidden(x+1, y+1));

		return r;
	}

	void Board::reveal(size_t x, size_t y) {
		if(x<1 || x>width()) return;
		if(y<1 || y>height()) return;

		uint8_t& item = m_items.at( vector_position(x, y) );
		if( (item & HIDDEN_BIT) == 0) return;
		item &= HIDDEN_MASK;
		size_t count = count_mines(x,y);
		if(item == CLEAR) {
			if(count>0) {
				item = '0'+count;
			} else {
				// reveal more
				reveal(x-1, y-1);
				reveal(x+0, y-1);
				reveal(x+1, y-1);

				reveal(x-1, y+0);
				reveal(x+1, y+0);

				reveal(x-1, y+1);
				reveal(x+0, y+1);
				reveal(x+1, y+1);
			}
		}
	}

	void Board::reveal_all() {
		for(auto& c: m_items)
			c &= HIDDEN_MASK;
	}

	void Board::distribute_mines() {
		uniform_int_distribution<size_t> dis_x(1, m_width);
		uniform_int_distribution<size_t> dis_y(1, m_height);
		for(size_t i=0; i<m_mines; i++) {
			// Pick a random location.
			size_t x = dis_x(gen);
			size_t y = dis_y(gen);
			// Check for a mine.
			if( !is_mine(get(x, y)) ) {
				// Place a mine.
				set(x, y, HIDDEN_MINE);
			}
		}
	}
}}
