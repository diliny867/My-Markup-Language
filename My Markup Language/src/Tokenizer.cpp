#include "Tokenizer.h"

#include <iostream>
#include <string>

using namespace MyML;

void Tokenizer::Tokenize(char* str) {
	current = str;
	start = current;

	tokens.clear();
	for(Token token = nextToken(); token.type != Token::Type::EndOfFile; token = nextToken()) {
#if EXCLUDES | EXCLUDE_COMMENT_BIT
		if(token.type == Token::Comment) {
			continue;
		}
#endif
#if EXCLUDES | EXCLUDE_NUL_BIT
		if(token.type == Token::Nul) {
			continue;
		}
#endif
#if EXCLUDES | EXCLUDE_ERROR_BIT
		if(token.type == Token::Error) {
			continue;
		}
#endif
		tokens.push_back(token);
	}
	tokens.push_back(Token(Token::EndOfFile,""));
}
void Tokenizer::Tokenize(const std::string& str) {
	if(str.empty()) {
		return;
	}
	Tokenize(const_cast<char*>(str.data()));
}

void Tokenizer::PrintToken(const Token&token) {
	switch(token.type) {
	case Token::Error:
		std::cout<<"Token::Error ";
		break;
	case Token::EndOfFile:
		std::cout<<"Token::EndOfFile ";
		break;
	case Token::Nul:
		std::cout<<"Token::Nul ";
		break;
	case Token::Identifier:
		std::cout<<"Token::Identifier ";
		break;
	case Token::String:
		std::cout<<"Token::String ";
		break;
	case Token::Number:
		std::cout<<"Token::Number ";
		break;
	case Token::Equals:
		std::cout<<"Token::Equals ";
		break;
	case Token::Dot:
		std::cout<<"Token::Dot ";
		break;
	//case Token::DoubleQuotation:
	//	std::cout<<"Token::DoubleQuotation ";
	//	break;
	//case Token::SingleQuotation:
	//	std::cout<<"Token::SingleQuotation ";
	//	break;
	case Token::TagStartNew:
		std::cout<<"Token::TagStartNew ";
		break;
	case Token::TagStartSame:
		std::cout<<"Token::TagStartSame ";
		break;
	case Token::TagStartPrev:
		std::cout<<"Token::TagStartPrev ";
		break;
	case Token::TagEnd:
		std::cout<<"Token::TagEnd ";
		break;
	case Token::TextIn:
		std::cout<<"Token::TextIn ";
		break;
	case Token::TextAfter:
		std::cout<<"Token::TextAfter ";
		break;
	case Token::TextBefore:
		std::cout<<"Token::TextBefore ";
		break;
	case Token::Comment:
		std::cout<<"Token::Comment ";
		break;
	}
	std::cout<<std::string(token.data)<<std::endl;
}
void Tokenizer::PrintTokens() {
	for(std::size_t i=0; i<tokens.size();i++) {
		std::cout<<i<<" ";
		MyML::Tokenizer::PrintToken(tokens.at(i));
	}
}

char Tokenizer::advance() {
	return *(current++);
}
char Tokenizer::setback() {
	return *(--current);
}
char Tokenizer::advancestart() {
	return *(++start);
}
char Tokenizer::peek() {
	return *current;
}

Token Tokenizer::makeToken(const Token::Type type) {
	return Token(type,std::string_view(start,current-start));
}

void Tokenizer::skipSpace() {
	while(isspace(peek())) {
		advance();
	}
}
void Tokenizer::skipToLineEnd() {
	while(peek() != '\n' && peek() != '\0') {
		advance();
	}
}

Token Tokenizer::makeNumber() {
	while(isdigit(peek())) {
		advance();
	}
	if(peek() == '.') {
		advance();
		while(isdigit(peek())) {
			advance();
		}
	}
	return makeToken(Token::Type::Number);
}
Token Tokenizer::makeIdentifier() {
	while(isdigit(peek()) || isalpha(peek()) || peek() == '_') {
		advance();
	}
	return makeToken(Token::Type::Identifier);
}
Token Tokenizer::makeText(const Token::Type text_type) {
	while(peek() != '[' && peek() != '\0') {
		advance();
	}
	return makeToken(text_type);
}
Token Tokenizer::makeString(const char quote) {
	advancestart();
	while(peek() != quote && peek() != '\0') {
		if(peek() == '\\') {
			advance();
		}
		advance();
	}
	Token tok = makeToken(Token::Type::String);
	advance();
	return tok;
}


Token Tokenizer::nextToken() {
	skipSpace();
	start = current;
	if(peek() == '\0') {
		return makeToken(Token::Type::EndOfFile);
	}

	const char c = advance();
	switch(c) {
	case '[':
		in_tag = true;
		switch(advance()) {
		case '+':
			return makeToken(Token::Type::TagStartSame);
		case '>':
			return makeToken(Token::Type::TagStartNew);
		case '<':
			return makeToken(Token::Type::TagStartPrev);
		}
		setback();
		return makeToken(Token::Type::TagStartSame);
	case ']':
		in_tag = false;
		return makeToken(Token::Type::TagEnd);
	case '+':
		advancestart();
		return makeText(Token::Type::TextIn);
	case '>':
		advancestart();
		return makeText(Token::Type::TextAfter);
	case '<':
		advancestart();
		return makeText(Token::Type::TextBefore);
	case '\"': case '\'':
		return makeString(c);
	case '=':
		return makeToken(Token::Type::Equals);
	case '.':
		return makeToken(Token::Type::Dot);
	case '/':
		if(peek() == '/') {
			skipToLineEnd();
		}
		return makeToken(Token::Type::Comment);
	case '_':
		return makeIdentifier();
	case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
		return makeNumber();
	default:
		if(in_tag) {
			if(isalpha(c)) {
				return makeIdentifier();
			}
		}else {
			return makeText(Token::Type::TextIn);
		}
		break;
	}
	return makeToken(Token::Type::Nul);
}

