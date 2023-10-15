#include "Parser.h"

#include <iostream>

using namespace MyML;

void Parser::Parse(char* str) {
	tokenizer.Tokenize(str);

	curr_index = 0;
	root = new Node(Node::Tag,nullptr);
	root->value = ROOT_TAG_TEXT;

	contexts.push(root);
	textContext = root;

	parseExpr();
}
void Parser::Parse(const std::string& str) {
	Parse(const_cast<char*>(str.data()));
}

void Parser::PrintNode(const Node*node) {
	switch(node->type) {
	case Node::Tag:
		std::cout<<"Tag: "<<std::get<std::string>(node->value)<<std::endl;
		break;
	case Node::Attribute:
		std::cout<<"Attribute: "<<std::get<std::string>(node->value)<<std::endl;
		break;
	case Node::String:
		std::cout<<"String: "<<std::get<std::string>(node->value)<<std::endl;
		break;
	case Node::Text:
		std::cout<<"Text: "<<std::get<std::string>(node->value)<<std::endl;
		break;
	case Node::Integer:
		std::cout<<"Integer: "<<std::get<long long>(node->value)<<std::endl;
		break;
	case Node::Float:
		std::cout<<"Float: "<<std::get<double>(node->value)<<std::endl;
		break;
	}
}
void Parser::printNodeWithChildren(const Node* node, const std::string& prefix, const bool isLeft) {
	std::cout << prefix << (isLeft ? "|-- " : "L-- ");

	PrintNode(node);

	std::size_t i = 0;
	for(auto ch_node: node->nodes) {
		if(i==node->nodes.size()-1) {
			printNodeWithChildren(ch_node, prefix + (isLeft ? "|   " : "    "), false);
		}else {
			printNodeWithChildren(ch_node, prefix + (isLeft ? "|   " : "    "), true);
		}
		i++;
	}
}
void Parser::PrintTree() {
	printNodeWithChildren(root,"",false);
}

void Parser::advance() {
	curr_token = tokenizer.tokens.at(curr_index++);
}
void Parser::setback() {
	curr_token = tokenizer.tokens.at(--curr_index);
}

bool Parser::topTagIsRoot() {
	return contexts.size() == 1 && contexts.top() == root;
}

Node* Parser::newNode(const Node::Type type) {
	return new Node(type, contexts.top());
}

std::string Parser::truncateTrailingSideWhitespaces(const std::string& str) {
	auto it = str.begin();
	auto rit = str.end()-1;
	for(;it<str.end() && isspace(*it);++it) {}
	for(;rit>=str.begin() && isspace(*rit);--rit) {}
	return std::string(it, rit+1);
}
char Parser::charToItsEscapeSequence(const char ch) {
	// f it idk how to adequately do it
	return ch;
}
std::string Parser::convertBackslashes(const std::string& str) {
	std::string res;
	res.reserve(str.capacity());
	for(std::size_t i=0;i<str.size();i++) {
		if(str.at(i) == '\\') {
			i++;
			if(i==str.size()) {
				continue;
			}
			res += charToItsEscapeSequence(str.at(i));
		}else {
			res += str.at(i);
		}
	}
	return res;
}

Node::Type Parser::parseNumber(void* number) {
	if(curr_token.data.empty()) {
		return Node::Integer;
	}
	bool is_float = false;
	std::string str = std::string(curr_token.data);
	char c;
	for(std::size_t i=0;i<str.size();i++) {
		c = str.at(i);
		if(!isdigit(c)) {
			if(c == '.') {
				is_float = true;
				i++;
				for(;i<str.size();i++) {
					c = str.at(i);
					if(!isdigit(c)) {
						return Node::Integer;
					}
				}
				break;
			}else {
				return Node::Integer;
			}
		}
	}
	if(is_float) {
		double* num = (double*)number;
		*num = std::stod(str);
		return Node::Float;
	}else {
		long long* num = (long long*)number;
		*num = std::stoll(str);
		return Node::Integer;
	}
}
Node* Parser::parseAttribute() {
	Node* curr_attribute_node = newNode(Node::Attribute);
	curr_attribute_node->value = std::string(curr_token.data);
	contexts.push(curr_attribute_node);

	Node* new_node;
	
	while(true){
		advance();
		switch(curr_token.type) {
		case Token::Dot:
			advance();
			while(curr_token.type == Token::Identifier) {
				new_node = newNode(Node::Attribute);
				new_node->value = std::string(curr_token.data);
				contexts.top()->nodes.push_back(new_node);
				contexts.push(new_node);
				advance();
				if(curr_token.type == Token::Dot) {
					advance();
				}else {
					break;
				}
			}
			setback();
			break;
		case Token::Equals:
			advance();
			switch(curr_token.type) {
			case Token::String:
				new_node = newNode(Node::String);
				new_node->value = convertBackslashes(std::string(curr_token.data));
				contexts.top()->nodes.push_back(new_node);
				break;
			case Token::Number: 
			{
				void* number = calloc(1,sizeof(long long));
				switch(parseNumber(number)) {
				case Node::Integer:
					new_node = newNode(Node::Integer);
					new_node->value = *((long long*)number);
					break;
				case Node::Float:
					new_node = newNode(Node::Float);
					new_node->value = *((double*)number);
					break;
				}
				contexts.top()->nodes.push_back(new_node);
				break;
			}
			default:
				setback();
				break;
			}
			break;
		default:
			setback();
			while(contexts.top() != curr_attribute_node) {
				contexts.pop();
			}
			contexts.pop();
			return curr_attribute_node;
		}
	}
}
Node* Parser::parseTag() {
	Node* new_node = newNode(Node::Tag);
	contexts.push(new_node);
	bool is_first_ident = true;
	while(true) {
		advance();
		switch(curr_token.type) {
		case Token::Identifier:
			if(is_first_ident) {
				new_node->value = std::string(curr_token.data);
				is_first_ident = false;
			}else {
				new_node->nodes.push_back(parseAttribute());
			}
			break;
		case Token::TagEnd:
			contexts.pop();
			return new_node;
		case Token::EndOfFile:
			setback();
			contexts.pop();
			return new_node; 
		default:
			std::cout<<"Parse Tag Error ";
			Tokenizer::PrintToken(curr_token);
			break;
		}
	}
}

void Parser::parseExpr() {
	while(true){
		advance();

		Node* new_node;

		switch(curr_token.type) {
		case Token::TagStartSame:
			new_node = parseTag();
			contexts.top()->nodes.push_back(new_node);
			textContext = new_node;
			break;
		case Token::TagStartNew:
			new_node = parseTag();
			contexts.top()->nodes.push_back(new_node);
			contexts.push(new_node);
			textContext = new_node;
			break;
		case Token::TagStartPrev:
			new_node = parseTag();
			if(topTagIsRoot()) {
				contexts.top()->nodes.push_back(new_node);
			} else {
				contexts.pop();
				contexts.top()->nodes.push_back(new_node);
				contexts.push(new_node);
			}
			textContext = new_node;
			break;
		case Token::TextIn:
			new_node = newNode(Node::Text);
			new_node->value = truncateTrailingSideWhitespaces(std::string(curr_token.data));
			textContext->nodes.push_back(new_node);
			break;
		case Token::TextAfter:
		{
			new_node = newNode(Node::Text);
			new_node->value = truncateTrailingSideWhitespaces(std::string(curr_token.data));
			if(textContext == root) {
				textContext->nodes.push_back(new_node);
			}else {
				textContext->context->nodes.push_back(new_node);
			}
			break;
		}
		case Token::TextBefore:
		{
			new_node = newNode(Node::Text);
			new_node->value = truncateTrailingSideWhitespaces(std::string(curr_token.data));
			if(textContext == root) {
				auto it = textContext->nodes.end();
				textContext->nodes.insert(textContext->nodes.empty()?it:--it, new_node);
			}else {
				auto it = textContext->context->nodes.end();
				textContext->context->nodes.insert(textContext->context->nodes.empty()?it:--it, new_node);
			}
			break;
		}
		case Token::Comment:
			break;
		case Token::EndOfFile:
			return; //End
		default:
			std::cout<<"Parse Error: ";
			Tokenizer::PrintToken(curr_token);
			break;
		}
	}
}
