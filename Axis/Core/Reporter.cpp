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

	Reporter::Reporter(std::FILE* file) : output(file)
	{}

	void Reporter::set_warnings_as_errors()
	{
		warnings_as_errors = true;
	}

	bool Reporter::pop_message(Message msg, SourceLocation loc, FormatArgs args)
	{
		WrittenMessage target(msg, loc, std::vformat(get_format(msg), args));

		auto it = std::find(messages.begin(), messages.end(), target);
		if(it == messages.end())
			return false;

		messages.erase(it);
		return true;
	}

	bool Reporter::has_no_messages()
	{
		return messages.empty();
	}

	errno_t Reporter::set_output(std::string_view path)
	{
		return fopen_s(std::out_ptr(output), path.data(), "w");
	}

	void Reporter::write(Message msg, SourceLocation loc, std::string_view format, FormatArgs args)
	{
		auto severity = get_severity(msg);
		if(warnings_as_errors && severity == Severity::Warning)
			severity = Severity::Error;

		auto text = std::vformat(format, args);
		auto sev  = to_string(severity);
		std::fprintf(output.get(), "%s:%u:%u: %s: %s\n", loc.file.data(), loc.line, loc.column, sev.data(), text.data());

		messages.emplace_back(msg, loc, std::move(text));
	}
}
