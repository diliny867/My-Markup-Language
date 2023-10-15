#pragma once

#include <vector>

#include "Token.h"

#define EXCLUDE_NUL_BIT 1
#define EXCLUDE_ERROR_BIT 2
#define EXCLUDE_COMMENT_BIT 4
#define EXCLUDES EXCLUDE_NUL_BIT | EXCLUDE_ERROR_BIT | EXCLUDE_COMMENT_BIT

namespace MyML {
class Tokenizer {
	bool in_tag = false;
	char* start = nullptr;
	char* current = nullptr;

	inline char advance();
	inline char setback();
	inline char advancestart();
	inline char peek();
	void skipSpace();
	void skipToLineEnd();

	Token nextToken();

	Token makeNumber();
	Token makeIdentifier();
	Token makeText(Token::Type text_type);
	Token makeString(char quote);
	Token makeToken(Token::Type type);

public:
	std::vector<Token> tokens;

	static void PrintToken(const Token& token);
	void PrintTokens();

	void Tokenize(char* str);
	void Tokenize(const std::string& str);
};
}