cmake_minimum_required( VERSION 3.24 )
project( vmfdown
	DESCRIPTION "A tool used to verify a game's install."
	VERSION 1.0.0
)

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

# RPath for Linux
set( CMAKE_SKIP_BUILD_RPATH FALSE )
set( CMAKE_BUILD_RPATH_USE_ORIGIN TRUE )
set( CMAKE_INSTALL_RPATH $ORIGIN )

# Enable PIE
set( CMAKE_POSITION_INDEPENDENT_CODE ON )

# Enable IPO/LTO if supported
include( CheckIPOSupported )
check_ipo_supported( RESULT VMFDOWN_IPO_SUPPORTED OUTPUT VMFDOWN_IPO_ERROR )
if( VMFDOWN_IPO_SUPPORTED )
	set( CMAKE_INTERPROCEDURAL_OPTIMIZATION ON )
else()
	message( WARNING "IPO/LTO not supported! (${VMFDOWN_IPO_ERROR})" )
endif()


# Add sources for executable
list( APPEND ${PROJECT_NAME}_SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/src/main.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/processor.cpp"
)

# Create executable
add_executable( ${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES} )
target_compile_definitions( ${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG> "VMFDOWN_VERSION=\"${PROJECT_VERSION}\"" )

# Link to dependencies
include( "${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/CMakeLists.txt" )
target_link_libraries( ${PROJECT_NAME} PRIVATE Argumentum::argumentum fmt::fmt keyvalues)
