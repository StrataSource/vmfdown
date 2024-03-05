//
// Created by ENDERZOMBI102 on 05/03/2024.
//
#include <string>
#include <filesystem>
#include "fmt/base.h"
#include "fmt/std.h"
#include "fmt/ranges.h"
#include "argumentum/argparse.h"

#include "processor.hpp"



auto main( int argc, char* argv[] ) -> int {
	argumentum::argument_parser parser{};
	parser.config()
		.program( std::filesystem::path( argv[ 0 ] ).filename().string() )
		.description( "A tool used to downgrade `.vmf`s to something accepted by the standard valve compilers. v" VMFDOWN_VERSION );

	std::vector<std::string> inputFiles{ 10 };  // 10 should be enough to avoid reallocation
	bool postcompiler{};

	auto params{ parser.params() };
	params.add_parameter( inputFiles, "file" )
		.help( "The files to convert." )
		.minargs( 1 );
	params.add_parameter( postcompiler, "--postcompiler" )
		.help( "Enable compatability with srctools' postcompiler, using backticks (`) to replace \"." )
		.setLongName( "--postcompiler" )
		.default_value( false );

	auto result{ parser.parse_args( argc, argv ) };
	if (! result )
		return 1;

	// `vmfdown started at $time` message
	auto start{ std::time( nullptr ) };
	{
		tm local{};
		#if defined( _WIN32 )
			localtime_s( &local, &now );
		#else
			localtime_r( &start, &local );
		#endif
		fmt::println( "vmfdown started at {:02d}:{:02d}:{:02d}", local.tm_hour, local.tm_min, local.tm_sec );
	}

	for ( const auto& inputFile : inputFiles ) {
		fmt::print( "Processing file `{}`... ", inputFile );
		auto status{ process( inputFile, postcompiler ) };
		if ( status == 0 )
			fmt::println( "done" );
		else if ( status == 1 )
			fmt::println( "failed to load map" );
		else if ( status == 2 )
			fmt::println( "failed to write output map" );
		else
			fmt::println( "how did you get here?" );
	}

	auto took{ std::time( nullptr ) - start };
	fmt::println( "Finished processing {} files in {:02d}s", inputFiles.size(), took );

	return 0;
}
