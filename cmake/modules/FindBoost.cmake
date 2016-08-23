#
# Set hints for FindBoost and then call the real FindBoost module.
#
# Our way of placing Boost installation outside of the sysroot does not play well
# with CMake's find_xxx() stuff, so cheat a little and pre-set certain internal
# variables of FindBoost.cmake so it will skip its searching steps.
#

# Find the Boost version
file( GLOB BOOST_ROOTS "${ANDROID_NDK}/sources/boost/*" )
list( LENGTH BOOST_ROOTS BOOST_ROOTS_NR )

if( BOOST_ROOTS_NR GREATER 1 )
	message( FATAL_ERROR "Not implemented: more than one Boost version bundled with NDK" )
endif()

list( GET BOOST_ROOTS 0 BOOST_ROOT )

# Pick the correct Boost for our runtime
if( ANDROID_STL MATCHES "gnustl_(static|shared)" )
	set( BOOST_RUNTIME "gnu-${ANDROID_COMPILER_VERSION}" )
elseif( ${ANDROID_STL} MATCHES "c++_(static|shared)" )
	set( BOOST_RUNTIME "llvm-${ANDROID_COMPILER_VERSION}" )
else()
	message( FATAL_ERROR "We do not support STL '${ANDROID_STL}' for Boost libraries!"
	                     "Please use one of 'gnustl_shared', 'gnustl_static', 'c++_shared' or 'c++_static'.")
endif()

# Set hard-coded pathes for the real FindBoost.cmake (see above)
set( Boost_INCLUDE_DIR         "${BOOST_ROOT}/include"                                       CACHE STRING "Bundled Boost include directory")
set( Boost_LIBRARY_DIR_RELEASE "${BOOST_ROOT}/libs/${ANDROID_NDK_ABI_NAME}/${BOOST_RUNTIME}" CACHE STRING "Bundled Boost library directory for Release builds")
set( Boost_LIBRARY_DIR_DEBUG   "${BOOST_ROOT}/libs/${ANDROID_NDK_ABI_NAME}/${BOOST_RUNTIME}" CACHE STRING "Bundled Boost include directory for Debug builds")
set( Boost_NO_SYSTEM_PATHS     TRUE                                                          CACHE BOOL   "Do not search default pathes for bundled Boost")

# Cleanup and chain to real FindBoost.cmake
unset( BOOST_RUNTIME )
unset( BOOST_ROOT )
unset( BOOST_ROOTS )

set( ORIG_CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" )
list( REMOVE_ITEM CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}" )
find_package( Boost )
set( CMAKE_MODULE_PATH "${ORIG_CMAKE_MODULE_PATH}" )
unset( ORIG_CMAKE_MODULE_PATH )
