# ==========================================================================
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
# ==========================================================================

cmake_minimum_required(VERSION 3.12.0 FATAL_ERROR)

# Add all the layers required for the library
#   Core Layer (software optimizations and accelerator-related logic)
add_subdirectory(isal)
add_subdirectory(core-sw)
add_subdirectory(core-iaa)

#   Business logic layer
add_subdirectory(middle-layer)

#   User API
add_subdirectory(c_api)

# Build and install the resulting library
get_property(LIB_DEPS GLOBAL PROPERTY QPL_LIB_DEPS)

add_library(qpl STATIC ${LIB_DEPS})

target_include_directories(qpl
        PUBLIC $<BUILD_INTERFACE:${QPL_PROJECT_DIR}/include/> $<INSTALL_INTERFACE:include>)

if (DYNAMIC_LOADING_LIBACCEL_CONFIG)
    target_link_libraries(qpl PRIVATE ${CMAKE_DL_LIBS})
else()
    target_link_libraries(qpl PRIVATE "$<$<PLATFORM_ID:Linux>:accel-config>")
endif()

install(TARGETS qpl
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY ${QPL_PROJECT_DIR}/include/qpl/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qpl
        FILES_MATCHING PATTERN "*.h")
