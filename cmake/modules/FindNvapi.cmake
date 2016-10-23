# 
#  FindNvapi.cmake
# 
#  Try to find the Nvapi

#  Once done this will define
#
#  NVAPI_FOUND - system found Nvapi
#  NVAPI_INCLUDE_DIRS - the Nvapi include directory
#  NVAPI_LIBRARIES - Link this to use Nvapi
#
#  Distributed under the Apache License, Version 2.0.
#  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
# 

if (WIN32)
  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(NVAPI DEFAULT_MSG NVAPI_INCLUDE_DIRS NVAPI_LIBRARIES)
endif()

mark_as_advanced(NVAPI_INCLUDE_DIRS NVAPI_LIBRARIES NVAPI_SEARCH_DIRS)
