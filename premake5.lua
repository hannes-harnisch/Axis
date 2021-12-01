output_dir = '%{cfg.buildcfg}_%{cfg.architecture}_%{cfg.system}'

workspace 'Axis'
	startproject		'Tests'
	architecture		'x64'
	configurations		{ 'Debug', 'Development', 'Release' }
	flags				'MultiProcessorCompile'
	language			'C++'
	cppdialect			'C++latest'
	cdialect			'C11'
	conformancemode		'On'
	warnings			'Extra'
	disablewarnings		'4201' -- anonymous structs
	floatingpoint		'Fast'
	staticruntime		'On'
	inheritdependencies	'Off'
	files				{
							'%{prj.name}/**.cpp',
							'%{prj.name}/**.hpp',
						}
	debugdir			('.bin/'	 .. output_dir .. '/%{prj.name}')
	targetdir			('.bin/'	 .. output_dir .. '/%{prj.name}')
	objdir				('.bin_int/' .. output_dir .. '/%{prj.name}')

	filter 'Debug'
		symbols			'On'
		runtime			'Debug'
		defines			'AX_DEBUG'

	filter 'Development'
		symbols			'On'
		optimize		'Speed'
		runtime			'Debug'
		defines			'AX_DEBUG'
		flags			'LinkTimeOptimization'

	filter 'Release'
		optimize		'Speed'
		runtime			'Release'
		flags			'LinkTimeOptimization'

project 'Axis'
	location			'%{prj.name}'
	kind				'StaticLib'
	includedirs			{ '', '%{prj.name}', 'Dependencies' }
	pchheader			'PCH.hpp'
	pchsource			'%{prj.name}/PCH.cpp'

project 'Tests'
	location			'%{prj.name}'
	kind				'ConsoleApp'
	includedirs			{ '', '%{prj.name}', 'Dependencies' }
	links				'Axis'