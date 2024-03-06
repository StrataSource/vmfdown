//
// Created by ENDERZOMBI102 on 05/03/2024.
//
#include "processor.hpp"
#include "fmt/format.h"
#include "KeyValue.h"
#include <fstream>
#include <algorithm>

void entityEscPass( KeyValueRoot& root, bool postcompiler ) {
	auto& firstEnt{ root["entity"] };
	for ( auto* entity{&firstEnt}; entity->Next(); entity = entity->Next() ) {
		auto& conns{ entity->Get( "connections" ) };
		for ( auto connN{0}; connN < conns.ChildCount(); connN += 1 ) {
			auto value{ conns.At( connN ).Value().string };
			auto length{ conns.At( connN ).Value().length };
			for ( int j{0}; j < length; j += 1 ) {
				if ( value[j] == '\x1b' )
					value[j] = ',';
			}
		}
	}
}

void entityQuotingPass( KeyValueRoot& root, bool postcompiler ) {
	auto& firstEnt{ root["entity"] };
	for ( auto* entity{&firstEnt}; entity->Next(); entity = entity->Next() ) {
		auto& conns{ entity->Get( "connections" ) };
		for ( auto connN{0}; connN < conns.ChildCount(); connN += 1 ) {
			auto value{ conns.At( connN ).Value().string };
			auto runScript{ strstr( value, "RunScriptCode" ) };
			auto length{ conns.At( connN ).Value().length };

			for ( int j{0}; j < length; j += 1 ) {
				if ( value[j] == '"' )
					value[j] = (runScript && postcompiler) ? '`' : '\'';
			}
		}
	}
}

using PassFn = void(*)(KeyValueRoot&, bool);
static PassFn passes[2] { entityEscPass, entityQuotingPass };

auto process( const std::string& inputFile, bool postcompiler ) -> int {
	// read input file
	std::ifstream reader{ inputFile, std::ios_base::in };
	if (! reader.good() ) {
		return 1;
	}

	std::string contents{};
	{
		char buffer[1024] { 0 };
		while ( reader.getline( buffer, 1024 ) )
			contents.append( buffer ).append("\n");
	}
	reader.close();
	std::erase( contents, '\r' );

	// parse into KeyValue structure
	KeyValueRoot kv{};
	kv.Parse( contents.c_str() );

	// do correction passes
	for ( const auto pass : passes ) {
		pass( kv, postcompiler );
	}

	// open output file
	const auto outputFile{ inputFile.substr( 0, inputFile.size() - 4 ).append( "_out.vmf" ) };
	std::ofstream writer{ outputFile, std::ios_base::out | std::ios_base::trunc };
	if (! writer.good() ) {
		return 2;
	}

	// serialize and write to output file
	auto output{ kv.ToString() };
	auto size{ std::strlen( output ) };
	long amount{};
	for ( long written{ 0 }; written < size; written += amount ) {
		amount = std::min( static_cast<long>( size - written ), 1024l );
		writer.write( output + written, amount );
	}

	writer.close();

	return 0;
}
