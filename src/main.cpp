
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
	struct BoardData {
		char type = 0;
		int player = 0;
	};

	BoardData operator()(int x, int y) const {
		return _data[x + _width * y];
	}


	Board() {
		_data.resize(_width * _height);
		for (int i = 0; i < _width; ++i) {
			_data[i + _width * 1] = {'p', 1};
			_data[i + _width * (_height - 2)] = {'p', 2};
		}

		setLine(0, "rnbqkbnr", 1);
		setLine(7, "rnbqkbnr", 2);
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
				auto c = (*this)(x, y).type;
				auto player = (*this)(x, y).player;
				if (c == 0) {
					cout << "   ";
				}
				else {
					auto g = (player == 1)? graphics[c].first: graphics[c].second;
					cout << " "s + g + " "s;
				}
			}
			cout << "\e[0m" << endl; // Reset colors and newline
		}
	}

private:

	BoardData &operator()(int x, int y) {
		return _data[x + _width * y];
	}

	void setLine(int y, string content, int player) {
		for (int x = 0; x < _width && x < content.size(); ++x) {
			(*this)(x, y) = {content[x], player};
		}
	}

	vector <BoardData> _data;
	int _width = 8;
	int _height = 8;
};

int main(int argc, char **argv) {
	Board board;

	board.print();

	return 0;
}

