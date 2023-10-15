#pragma once

#include <stack>

#include "Tokenizer.h"
#include "Node.h"

#define ROOT_TAG_TEXT "Root Tag"

namespace MyML {
class Parser { //TODO: Fix parsing when getting non correct token
private:
	Token curr_token = Token(Token::Nul,"");
	std::size_t curr_index = 0;

	std::stack<Node*> contexts;
	Node* textContext;

	void advance();
	void setback();

	bool topTagIsRoot();

	Node* newNode(Node::Type type);

	static std::string truncateTrailingSideWhitespaces(const std::string& str);
	static std::string convertBackslashes(const std::string& str);
	static char charToItsEscapeSequence(const char ch);

	void parseExpr();
	Node::Type parseNumber(void* number); //returns either Node::Type::Integer of Node::Type::Float
	Node* parseAttribute();
	Node* parseTag();

	void printNodeWithChildren(const Node* node, const std::string& prefix, const bool isLeft);

public:
	Node* root;
	Tokenizer tokenizer;

	static void PrintNode(const Node* node);

	void PrintTree();

	void Parse(char* str);
	void Parse(const std::string& str);
};
}