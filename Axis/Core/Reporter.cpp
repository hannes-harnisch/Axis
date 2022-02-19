#include "PCH.hpp"

#include "Reporter.hpp"

namespace ax
{
	enum class Severity
	{
		Error,
		Warning,
		Note,
	};

	Severity get_severity(Message)
	{
		using enum Message;
		// add switch for warnings/notes here eventually
		return Severity::Error;
	}

	void Reporter::set_output(std::string_view)
	{}

	void Reporter::set_warnings_as_errors()
	{
		warnings_as_errors = true;
	}

	bool Reporter::has_message(Message msg) const
	{
		auto it = std::find_if(messages.begin(), messages.end(), [=](MessageText const& txt) {
			return txt.msg == msg;
		});
		return it != messages.end();
	}

	void Reporter::write(Message msg, SourceLocation loc, std::string text)
	{
		if(warnings_as_errors)
			;
	}
}
