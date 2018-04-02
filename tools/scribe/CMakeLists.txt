set(TARGET_NAME scribe)


# don't use the setup_hifi_project macro as we don't want Qt or GLM dependencies
file(GLOB TARGET_SRCS src/*)

set (BUILDTGT_DIR "build")
#set (BUILDTGT_DIR "build")
message( STATUS "Source_src_dir=${CMAKE_SOURCE_DIR}" )
message( STATUS "Project_src_dir=${PROJECT_SOURCE_DIR}" )

message( STATUS "GLSLANG_SPIRV_INCLUDE_DIR=${PROJECT_SOURCE_DIR}" )

find_path(GLSLANG_SPIRV_INCLUDE_DIR SPIRV/spirv.hpp HINTS "${CMAKE_SOURCE_DIR}/../glslang" DOC "Path to SPIRV/spirv.hpp")
#find_path(SPIRV_TOOLS_INCLUDE_DIR spirv-tools/libspirv.h HINTS "${CMAKE_SOURCE_DIR}/../spirv-tools/include"
 #                                                              "${CMAKE_SOURCE_DIR}/../source/spirv-tools/include"
  #                                                             "${CMAKE_SOURCE_DIR}/../spirv-tools/external/include"
  #                                                             "${CMAKE_SOURCE_DIR}/../source/spirv-tools/external/include"
   #                                                      DOC "Path to spirv-tools/libspirv.h")

if (WIN32)
    set (GLSLANG_SEARCH_PATH "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/glslang/Release"
                             "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/glslang/OSDependent/Windows/Release"
                             "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/OGLCompilersDLL/Release"
                             "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/SPIRV/Release" )
    message( STATUS "GLSLANG_SEARCH_PATH=${GLSLANG_SEARCH_PATH}" )
  #  set (SPIRV_TOOLS_SEARCH_PATH "${CMAKE_SOURCE_DIR}/../spirv-tools/${BUILDTGT_DIR}/Release")
else()
    set (GLSLANG_SEARCH_PATH "${CMAKE_SOURCE_DIR}/../glslang/build/install/lib" "${CMAKE_SOURCE_DIR}/../x86_64/lib/glslang" )
   # set (SPIRV_TOOLS_SEARCH_PATH "${CMAKE_SOURCE_DIR}/../spirv-tools/build" "${CMAKE_SOURCE_DIR}/../x86_64/lib/spirv-tools" )
endif()

find_library(GLSLANG_LIB NAMES glslang
             HINTS ${GLSLANG_SEARCH_PATH} )

find_library(OGLCompiler_LIB NAMES OGLCompiler
             HINTS ${GLSLANG_SEARCH_PATH} )

find_library(OSDependent_LIB NAMES OSDependent
             HINTS ${GLSLANG_SEARCH_PATH} )

find_library(SPIRV_LIB NAMES SPIRV
             HINTS ${GLSLANG_SEARCH_PATH} )

#find_library(SPIRV_TOOLS_LIB NAMES SPIRV-Tools
 #            HINTS ${SPIRV_TOOLS_SEARCH_PATH} )

# On Windows, we must pair Debug and Release appropriately
if (WIN32)
    set (GLSLANG_DEBUG_SEARCH_PATH "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/glslang/Debug"
                                   "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/glslang/OSDependent/Windows/Debug"
                                   "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/OGLCompilersDLL/Debug"
                                   "${CMAKE_SOURCE_DIR}/../glslang/${BUILDTGT_DIR}/SPIRV/Debug")
   # set (SPIRV_TOOLS_DEBUG_SEARCH_PATH "${CMAKE_SOURCE_DIR}/../spirv-tools/${BUILDTGT_DIR}/Debug")

    add_library(glslang     STATIC IMPORTED)
    add_library(OGLCompiler STATIC IMPORTED)
    add_library(OSDependent STATIC IMPORTED)
    add_library(SPIRV       STATIC IMPORTED)
  #  add_library(Loader      STATIC IMPORTED)
 #   add_library(SPIRV-Tools STATIC IMPORTED)

    find_library(GLSLANG_DLIB NAMES glslang
                 HINTS ${GLSLANG_DEBUG_SEARCH_PATH} )
    find_library(OGLCompiler_DLIB NAMES OGLCompiler
                 HINTS ${GLSLANG_DEBUG_SEARCH_PATH} )
    find_library(OSDependent_DLIB NAMES OSDependent
                 HINTS ${GLSLANG_DEBUG_SEARCH_PATH} )
    find_library(SPIRV_DLIB NAMES SPIRV
                 HINTS ${GLSLANG_DEBUG_SEARCH_PATH} )
 #   find_library(SPIRV_TOOLS_DLIB NAMES SPIRV-Tools
  #               HINTS ${SPIRV_TOOLS_DEBUG_SEARCH_PATH} )

    set_target_properties(glslang PROPERTIES
                         IMPORTED_LOCATION       "${GLSLANG_LIB}"
                         IMPORTED_LOCATION_DEBUG "${GLSLANG_DLIB}")
    set_target_properties(OGLCompiler PROPERTIES
                         IMPORTED_LOCATION       "${OGLCompiler_LIB}"
                         IMPORTED_LOCATION_DEBUG "${OGLCompiler_DLIB}")
    set_target_properties(OSDependent PROPERTIES
                         IMPORTED_LOCATION       "${OSDependent_LIB}"
                         IMPORTED_LOCATION_DEBUG "${OSDependent_DLIB}")
    set_target_properties(SPIRV PROPERTIES
                         IMPORTED_LOCATION       "${SPIRV_LIB}"
                         IMPORTED_LOCATION_DEBUG "${SPIRV_DLIB}")
 #   set_target_properties(SPIRV-Tools PROPERTIES
  #                       IMPORTED_LOCATION       "${SPIRV_TOOLS_LIB}"
  #                       IMPORTED_LOCATION_DEBUG "${SPIRV_TOOLS_DLIB}")

    set (GLSLANG_LIBRARIES glslang OGLCompiler OSDependent SPIRV)
  #  set (SPIRV_TOOLS_LIBRARIES SPIRV-Tools)
else ()
    set (GLSLANG_LIBRARIES ${GLSLANG_LIB} ${OGLCompiler_LIB} ${OSDependent_LIB} ${SPIRV_LIB})
 #   set (SPIRV_TOOLS_LIBRARIES ${SPIRV_TOOLS_LIB})
endif()



add_executable(${TARGET_NAME} ${TARGET_SRCS})
if (WIN32)
 set_property(TARGET ${TARGET_NAME} APPEND_STRING PROPERTY LINK_FLAGS_DEBUG "/OPT:NOREF /OPT:NOICF")
endif()

#adding glslang to the mix
#glslang_set_link_args(glslangValidator)
   # For MinGW compiles, statically link against the GCC and C++ runtimes.
    # This avoids the need to ship those runtimes as DLLs.
    if(WIN32 AND ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
        set_target_properties(${TARGET} PROPERTIES
                              LINK_FLAGS "-static -static-libgcc -static-libstdc++")
    endif()

set(LIBRARIES
    glslang
    SPIRV
 #   SPVRemapper
 #   glslang-default-resource-limits
    )

if(WIN32)
 #   set(LIBRARIES ${LIBRARIES} psapi)
elseif(UNIX)
    if(NOT ANDROID)
        set(LIBRARIES ${LIBRARIES} pthread)
    endif()
endif(WIN32)

target_link_libraries(${TARGET_NAME} ${LIBRARIES})

