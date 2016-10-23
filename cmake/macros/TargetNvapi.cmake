#
#  Copyright 2016 High Fidelity, Inc.
#  Created by Sam Gateau on 2016/10/22
#
#  Distributed under the Apache License, Version 2.0.
#  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
#
macro(TARGET_NVAPI)
    if (WIN32)
        add_dependency_external_projects(nvapi)
        find_package(nvapi REQUIRED)
        
        target_include_directories(${TARGET_NAME} SYSTEM PRIVATE ${NVAPI_INCLUDE_DIRS})
        target_link_libraries(${TARGET_NAME} ${NVAPI_LIBRARIES})
        add_definitions(-DHAVE_NVAPI)
    endif()
endmacro()
