
#include <iostream>
#include <vector>
#include <map>

using namespace std;

map<char, std::pair<string, string> > graphics = {
	{'k', {"♔", "♚"}}, // King
	{'q', {"♕", "♛"}}, // queen
	{'r', {"♖", "♜"}}, // rook
	{'b', {"♗", "♝"}}, // bishop
	{'n', {"♘", "♞"}}, // knight
	{'p', {"♙", "♟"}}  // pawn
};


class Board {
public:
	char operator()(int x, int y) {
		return _data[x + _width * y];
	}

	Board() {
		_data.resize(_width * _height);
		for (int i = 0; i < _width; ++i) {
			_data[i + _width * 1] = 'p';
			_data[i + _width * (_height - 2)] = 'p';
		}
	}

	void print() {
		string light = "\e[48;2;100;100;100m";
		string dark = "\e[48;2;0;0;0m";
		for (int y = 0; y < _height; ++y) {
			for (int x = 0; x < _width; ++x) {
				if ((y % 2) != (x % 2)) {
					cout << dark;
				}
				else {
					cout << light;
				}
				auto c = (*this)(x, y);
				if (c == 0) {
					cout << "   ";
				}
				else {
					cout << " "s + graphics[c].first + " "s;
				}
			}
			cout << "\e[0m" << endl; // Reset colors and newline
		}
	}

private:
	vector <char> _data;
	int _width = 8;
	int _height = 8;
};

int main(int argc, char **argv) {
	Board board;

	board.print();

	return 0;
}

