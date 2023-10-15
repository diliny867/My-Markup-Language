
#include <iostream>
#include <string>
#include <fstream>

#include "Parser.h"
#include "Tokenizer.h"

int main() {
	MyML::Parser parser;

	std::string example;
	std::getline(std::ifstream("G:/Prog/Other/Cpp/My Markup Language/My Markup Language/src/Parse Example.txt"), example, '\0');

	parser.Parse(example);

	parser.tokenizer.PrintTokens();
	parser.PrintTree();

	return 0;
}