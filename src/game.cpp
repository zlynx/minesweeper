#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "game"

using namespace std;

namespace zlynx { namespace minesweeper {
	static std::random_device rd;
	static std::mt19937 gen(rd());

	class Board {
	public:
		enum {
			HIDDEN_BIT = 0x80,
			HIDDEN_MASK = 0x7F,

			CLEAR = ' ',
			MINE = 'X',
			UNKNOWN = '?',
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

		void reveal(size_t x, size_t y) {
		}
	private:
		size_t m_width, m_height;
		size_t m_mines;
		vector<uint8_t> m_items;

		size_t vector_position(size_t x, size_t y) const {
			return (y+1)*(m_width+2) + (x+1);
		}

		void distribute_mines() {
			uniform_int_distribution<size_t> dis_x(0, m_width);
			uniform_int_distribution<size_t> dis_y(0, m_height);
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
		m_running = false;
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
		for(size_t y=0; y<m_board->height(); y++) {
			cout << setw(indent_width) << y << '|';
			for(size_t x=0; x<m_board->width(); x++) {
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

	void Game::inputs() {
		cout << "Enter location (eg A1 or B9):";
		string input;
		if( getline(cin, input) ) {
		} else {
			m_running = false;
		}
	}
}}
