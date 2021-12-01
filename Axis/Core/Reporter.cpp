#include "PCH.hpp"

#include "Reporter.hpp"

namespace ax
{
	void Reporter::set_output(std::string_view)
	{}

	bool Reporter::has_message(MessageCode code) const
	{
		auto it = std::find_if(messages.begin(), messages.end(), [=](Message const& msg) {
			return msg.code == code;
		});
		return it != messages.end();
	}

	void Reporter::report(MessageCode, std::string)
	{}
}
