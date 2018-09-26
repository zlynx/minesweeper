#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include "game"
#include "board"

using namespace std;

namespace zlynx { namespace minesweeper {
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
