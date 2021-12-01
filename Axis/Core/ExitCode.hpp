#pragma once

namespace ax
{
	enum class ExitCode
	{
		Ok			= 0,
		Usage		= 64,
		DataError	= 65,
		NoInput		= 66,
		NoUser		= 67,
		NoHost		= 68,
		Unavailable = 69,
		Software	= 70,
		OsError		= 71,
		OsFile		= 72,
		CantCreate	= 73,
		IoError		= 74,
		TempFail	= 75,
		Protocol	= 76,
		NoPerm		= 77,
		Config		= 78,
	};
}
