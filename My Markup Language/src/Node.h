#pragma once

#include <list>
#include <string>
#include <variant>

namespace MyML {
struct Node {
	enum Type {
		Tag,
		Attribute,
		String,
		Text,
		Integer,
		Float
	} type;

	//union {
	//	long long intnum;
	//	double floatnum;
	//	const char* text;
	//};

	std::variant<long long, double, std::string> value;

	Node* context = nullptr;

	std::list<Node*> nodes = std::list<Node*>();

	Node():type(Tag){}

	Node(const Type type_, Node* context_):type(type_),context(context_){}
};
}