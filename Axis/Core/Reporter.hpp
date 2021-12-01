#pragma once

#include "Axis/Core/Message.hpp"

#include <format>
#include <string_view>
#include <vector>

namespace ax
{
	class Reporter
	{
	public:
		template<typename... Ts> void report(std::type_identity_t<MessageCodeCheck<Ts...>> code, Ts&&... ts)
		{
			report(code, std::format(get_format(code), std::forward<Ts>(ts)...));
		}

		void set_output(std::string_view file_path);
		bool has_message(MessageCode code) const;

	private:
		std::vector<Message> messages;

		void report(MessageCode code, std::string text);
	};
}
