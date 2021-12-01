#pragma once

#include <string>

namespace ax
{
	enum class MessageCode
	{
	};

	struct Message
	{
		MessageCode code;
		std::string text;
	};

	enum class Severity
	{
		Note,
		Warning,
		Error
	};

	Severity	get_severity(MessageCode code);
	char const* get_format(MessageCode code);

	template<typename... Ts> struct MessageCodeCheck
	{
		consteval MessageCodeCheck(MessageCode code)
		{}
	};
}
