#include <iostream>
#include <memory>
#include <functional>

#include "Array.h"
#include "Allocator.h"
#include "BinTree.h"
#include "Figure.h"
#include "Rhomb.h"
#include "Pentagon.h"
#include "Hexagon.h"
#include "Pipeline.h"

using namespace std;

Figure* CreateFigure(char type);

const size_t memory_page_size = 65536;
const size_t block_size = std::max({ sizeof(Rhomb), sizeof(Pentagon), sizeof(Hexagon) });

Allocator Figure::allocator = Allocator(memory_page_size, block_size);

int main() {
	BinTree<Figure> tree;

	function<Figure*()> gen_figure = []() -> Figure* {
		int type = rand() % 3;
		switch (type) {
		case 0:
			return new Pentagon(rand() % 100);
		case 1:
			return new Hexagon(rand() % 100);
		case 2:
			return new Rhomb(rand() % 100, rand() % 100);
		default: 
			return nullptr;
		};
	};

	auto insert_random_figure = [&]() {
		tree.Insert(gen_figure());
	};

	auto print_tree = [&]() {
		for (auto el : tree)
			cout << *el << ' ';
		cout << endl;
	};

	auto remove_less_than = [&](double sq) {
		auto cur = tree.begin();
		auto end = tree.end();

		while(cur != end) {
			cout << **cur;
			if (cur->Square() < sq) {
				cout << " - deleted" << endl;
				Figure &temp = **cur;
				++cur;
				tree.Remove(temp);
				continue;
			}
			++cur;
			cout << endl;
		}
	};

	const size_t actions_limit = 100;
	size_t current = 0;
	Pipeline<void()> pipeline(actions_limit);

	char command = 0;
	char fig_type = 0;

	while (1) {
		cout << "Enter a command <a/f/r/p/h/i>" << endl;
		cin >> command;
		if (cin.eof())
			break;
		if (command == 'a') {
			cout << "Enter a figure <r/p/h> d1 (d2) to insert" << endl;
			cin >> fig_type;
			if (fig_type != 'h' && fig_type != 'p' && fig_type != 'r')
				continue;
			tree.Insert(CreateFigure(fig_type));
		}
		else if (command == 'f') {
			cout << "Enter a figure <r/p/h> d1 (d2) to find" << endl;
			cin >> fig_type;
			if (fig_type != 'h' && fig_type != 'p' && fig_type != 'r')
				continue;
			Figure *temp_figure = CreateFigure(fig_type);
			auto temp = tree.Find(*temp_figure);
			if (temp == tree.end())
				cout << "Nothing found" << endl;
			else
				cout << "Found: " << **temp << endl;
			Figure::operator delete (temp_figure);
		}
		else if (command == 'p') {
			cout << "Current state of the tree: " << endl;
			tree.Print();
		}
		else if (command == 'c') {
			cout << "Enter some commands <i/p/d sq>...0: " << endl;
			char cmd;
			double sq = 0;
			while (1) {
				cin >> cmd;
				if (cmd == '0')
					break;
				else if (cmd == 'i')
					pipeline.Push(insert_random_figure);
				else if (cmd == 'p')
					pipeline.Push(print_tree);
				else if (cmd == 'r') {
					cin >> sq;
					pipeline.Push(bind(remove_less_than, sq));
				}
			}
		}
		else if (command == 'd') {
			pipeline.PerformAll();
			pipeline.Clear();
		}
		else if (command == 'e') {
			break;
		}
		else if (command == 'i') {
			cout << "inorder print: " << endl;
			for (auto el : tree)
				cout << *el << ' ';
			cout << endl;
		}
		else if (command == 'r') {
			cout << "Enter a figure <r/p/h> d1 (d2) to remove" << endl;
			cin >> fig_type;
			if (fig_type != 'h' && fig_type != 'p' && fig_type != 'r')
				continue;
			Figure *temp_figure = CreateFigure(fig_type);
			tree.Remove(*temp_figure);
			Figure::operator delete (temp_figure);
		}
		else if (command == 'h') {
			cout <<
				R"(commands:
	a -- add,
	f -- find
	r -- remove,
	p -- print,
	h -- help,
	i -- inorder print.
figures:
	p l -- pentgon by length of side,
	h l -- hexagon by length of side,
	r d1 d2 -- rhomb by two diagonals d1, d2.)" << endl;
		}
	}
}

Figure* CreateFigure(char type) {
	Figure *fig = nullptr;
	if (type == 'r')
		fig = new Rhomb(std::cin);
	else if (type == 'p')
		fig = new Pentagon(std::cin);
	else if (type == 'h')
		fig = new Hexagon(std::cin);
	return fig;
}