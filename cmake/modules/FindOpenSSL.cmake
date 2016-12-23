#
# Set hints for FindOpenSSL and then call the real FindOpenSSL module.
#
# Our way of placing OpenSSL installation outside of the sysroot does not play well
# with CMake's find_xxx() stuff, so cheat a little and pre-set certain internal
# variables of FindOpenSSL.cmake so it will skip its searching steps.
#

# Find the OpenSSL version
file( GLOB OPENSSL_ROOTS "${ANDROID_NDK}/sources/openssl/*" )
list( LENGTH OPENSSL_ROOTS OPENSSL_ROOTS_NR )

if( OPENSSL_ROOTS_NR GREATER 1 )
	message( FATAL_ERROR "Not implemented: more than one OpenSSL version bundled with NDK" )
endif()

list( GET OPENSSL_ROOTS 0 OPENSSL_ROOT )
# Set hard-coded pathes for the real FindOpenSSL.cmake (see above)
set( OPENSSL_INCLUDE_DIR	"${OPENSSL_ROOT}/include"                       CACHE STRING "Bundled OpenSSL include directory")
set( OPENSSL_ROOT_DIR		"${OPENSSL_ROOT}/libs/${ANDROID_NDK_ABI_NAME}"	CACHE STRING "Bundled OpenSSL root directory")

# Cleanup and chain to real FindOpenSSL.cmake
unset( OPENSSL_ROOT )
unset( OPENSSL_ROOTS )

set( ORIG_CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" )
list( REMOVE_ITEM CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}" )
find_package( OpenSSL )
set( CMAKE_MODULE_PATH "${ORIG_CMAKE_MODULE_PATH}" )
unset( ORIG_CMAKE_MODULE_PATH )
