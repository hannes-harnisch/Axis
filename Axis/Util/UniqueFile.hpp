#pragma once

#include <cstdio>
#include <memory>

namespace ax
{
	struct FileCloser
	{
		void operator()(FILE* file) const
		{
			if(file != stdout && file != stderr && file != stdin)
				std::fclose(file);
		}
	};

	using UniqueFile = std::unique_ptr<std::FILE, FileCloser>;
}
