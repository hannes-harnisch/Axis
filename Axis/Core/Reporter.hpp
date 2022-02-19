#pragma once

#include "Axis/Core/Message.hpp"

#include <format>
#include <string_view>
#include <vector>

namespace ax
{
	struct SourceLocation
	{
		unsigned line	= 0;
		unsigned column = 0;
	};

	class Reporter
	{
	public:
		template<Message MSG, typename... Ts> void report(SourceLocation loc, Ts&&... ts)
		{
			static_assert(check_message<Ts...>(MSG), "Number of arguments does not match required arguments for this message.");
			write(MSG, loc, std::format(get_format(MSG), std::forward<Ts>(ts)...));
		}

		void set_output(std::string_view file_path);
		void set_warnings_as_errors();
		bool has_message(Message msg) const;

	private:
		struct MessageText
		{
			Message		msg;
			std::string text;
		};

		std::vector<MessageText> messages;
		bool					 warnings_as_errors = false;

		void write(Message msg, SourceLocation loc, std::string text);
	};
}
