#include <cctype>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include "game"

using namespace std;

namespace zlynx { namespace minesweeper {
	static std::random_device rd;
	static std::mt19937 gen(rd());

	class Board {
	public:
		enum : uint8_t {
			HIDDEN_BIT = 0x80,
			HIDDEN_MASK = 0x7F,

			CLEAR = ' ',
			MINE = 'X',
			EXPLOSION = '*',
			UNKNOWN = '.',
			HIDDEN_CLEAR = CLEAR|HIDDEN_BIT,
			HIDDEN_MINE = MINE|HIDDEN_BIT,
		};

		Board(size_t width,  size_t height, size_t mines):
			m_width(width), m_height(height),
			m_mines(mines),
			m_items( (width+2)*(height+2), HIDDEN_CLEAR )
		{
			distribute_mines();
		}

		uint8_t get_hidden(size_t x, size_t y) const {
			return m_items.at( vector_position(x, y) );
		}
		uint8_t get(size_t x, size_t y) const {
			uint8_t r = get_hidden(x, y);
			return (r&HIDDEN_BIT) ? UNKNOWN : r;
		}
		void set(size_t x, size_t y, uint8_t val) {
			m_items.at( vector_position(x, y) ) = val;
		}
		size_t width() const { return m_width; }
		size_t height() const {  return m_height; }

		static bool is_mine(uint8_t val) {
			return (val & HIDDEN_MASK) == MINE;
		}

		size_t count_mines(size_t x, size_t y) {
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

		void reveal(size_t x, size_t y) {
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

		void reveal_all() {
			for(auto& c: m_items)
				c &= HIDDEN_MASK;
		}
	private:
		size_t m_width, m_height;
		size_t m_mines;
		vector<uint8_t> m_items;

		size_t vector_position(size_t x, size_t y) const {
			return (y)*(m_width+2) + (x);
		}

		void distribute_mines() {
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
	};

	Game::Game(const Options &options):
		m_board(make_unique<Board>(options.width(), options.height(), options.mines())),
		m_running(true)
	{
	}

	Game::~Game() {
	}

	bool Game::is_running() {
		return m_running;
	}

	void Game::tick() {
		render();
		inputs();
	}

	void Game::render() {
		const int indent_width =  3;
		string indent(indent_width, ' ');
		// Top border
		cout << indent << ' ';
		for(size_t x=0; x<m_board->width(); x++) {
			cout << (char)('A'+x);
		}
		cout << '\n';
		cout << indent << '+';
		for(size_t x=0; x<m_board->width(); x++) {
			cout << '-';
		}
		cout << '+' << '\n';

		// Board contents
		for(size_t y=1; y<=m_board->height(); y++) {
			cout << setw(indent_width) << y << '|';
			for(size_t x=1; x<=m_board->width(); x++) {
				cout << m_board->get(x, y);
			}
			cout << '|' << '\n';
		}
		// Bottom border
		cout << indent << '+';
		for(size_t x=0; x<m_board->width(); x++) {
			cout << '-';
		}
		cout << '+' << '\n';
		cout << indent << ' ';
		for(size_t x=0; x<m_board->width(); x++) {
			cout << (char)('A'+x);
		}
		cout << ' ' << '\n';
	}

	std::tuple<size_t, size_t, bool> Game::parse_location(const std::string& line) {
		size_t x, y;
		if(line.length() < 2)
			return make_tuple(0, 0, false);
		istringstream is(line);
		int xchar = is.get();
		x = toupper(xchar) - 'A' + 1;
		is >> y;
		if(x > m_board->width() || y > m_board->height())
			return make_tuple(0, 0, false);
		if(!is)
			return make_tuple(0, 0, false);
		return make_tuple(x, y, true);
	}

	void Game::inputs() {
		string input;
		do {
			cout << "Enter location (eg A1 or B9): ";
			if( getline(cin, input) ) {
				size_t x, y;
				bool ok;
				tie(x, y, ok) = parse_location(input);
				if(!ok)
					continue;
				m_board->reveal(x,y);
				if(m_board->is_mine(m_board->get(x,y))) {
					m_board->set(x,y,Board::EXPLOSION);
					m_board->reveal_all();
					m_running = false;
					render();
				}
				break;
			} else {
				if(cin.eof()) {
					m_running = false;
					break;
				}
				cin.clear();
				cin.ignore(4000,'\n');
				continue;
			}
		} while(true);
	}
}}
