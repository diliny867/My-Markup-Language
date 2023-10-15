#pragma once

#include <string_view>

namespace MyML {
struct Token {
	enum Type {
		Error = -2,
		EndOfFile = -1,
		Nul = 0,
		Identifier,
		String,
		Number,
		Equals,
		Dot,
		TagStartNew,
		TagStartSame,
		TagStartPrev,
		TagEnd,
		TextIn,
		TextAfter,
		TextBefore,
		Comment
	} type;
	std::string_view data;
	Token(const Type type,const std::string_view data) {
		this->type = type;
		this->data = data;
	}
};
}
