#include "PCH.hpp"

#include "Axis/Util/Char.hpp"
#include "Axis/Util/Enum.hpp"
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

	std::string to_string(Severity severity)
	{
		std::string name(enum_name(severity));
		name[0] = to_lower(name[0]);
		return name;
	}

	Reporter::Reporter() : output(stdout)
	{}

	void Reporter::set_warnings_as_errors()
	{
		warnings_as_errors = true;
	}

	bool Reporter::has_message(Message msg) const
	{
		auto it = std::find_if(messages.begin(), messages.end(), [=](WrittenMessage const& written) {
			return written.msg == msg;
		});
		return it != messages.end();
	}

	errno_t Reporter::set_output(std::string_view path)
	{
		return fopen_s(std::out_ptr(output), path.data(), "w");
	}

	void Reporter::write(Message msg, SourceLocation loc, std::string text)
	{
		auto severity = get_severity(msg);
		if(warnings_as_errors && severity == Severity::Warning)
			severity = Severity::Error;

		auto severity_str = to_string(severity);
		std::fprintf(output.get(), "%s:%u:%u: %s: %s\n", loc.file.data(), loc.line, loc.column, severity_str.data(),
					 text.data());
	}
}
