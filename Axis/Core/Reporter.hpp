#pragma once

#include "Axis/Core/Message.hpp"
#include "Axis/Util/UniqueFile.hpp"

#include <format>
#include <string_view>
#include <vector>

namespace ax
{
	struct SourceLocation
	{
		std::string_view file;
		unsigned line	= 0;
		unsigned column = 0;

		bool operator==(SourceLocation const&) const = default;
	};

	struct FormatArgs : std::format_args
	{
		template<typename... Ts>
		FormatArgs(Ts&&... ts) : std::format_args(std::make_format_args(std::forward<Ts>(ts)...))
		{}
	};

	class Reporter
	{
	public:
		explicit Reporter(std::FILE* file = stdout); // takes ownership of the file pointer

		template<Message MSG, typename... Ts>
		void report(SourceLocation loc, Ts&&... ts)
		{
			static_assert(check_message<Ts...>(MSG),
						  "The given number of arguments does not match required number of arguments for this message.");
			write(MSG, loc, get_format(MSG), {std::forward<Ts>(ts)...});
		}

		void set_warnings_as_errors();
		bool has_message(Message msg, SourceLocation loc, FormatArgs args) const;
		errno_t set_output(std::string_view file_path);

	private:
		struct WrittenMessage
		{
			Message msg;
			SourceLocation loc;
			std::string text;

			bool operator==(WrittenMessage const&) const = default;
		};

		UniqueFile output;
		std::vector<WrittenMessage> messages;
		bool warnings_as_errors = false;

		void write(Message msg, SourceLocation loc, std::string_view format, FormatArgs args);
	};
}
