include(ExternalProject)
include(SelectLibraryConfigurations)

set(EXTERNAL_NAME nvapi)

string(TOUPPER ${EXTERNAL_NAME} EXTERNAL_NAME_UPPER)

if (WIN32)

    ExternalProject_Add(
      ${EXTERNAL_NAME}
      URL http://hifi-public.s3.amazonaws.com/dependencies/NVAPI-r370-developer.zip
      URL_MD5 d5597640e3191319f011ee48247fa9cd
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
      LOG_DOWNLOAD 1
    )

  # Hide this external target (for ide users)
  set_target_properties(${EXTERNAL_NAME} PROPERTIES FOLDER "hidden/externals")

  ExternalProject_Get_Property(${EXTERNAL_NAME} SOURCE_DIR)

  set(${EXTERNAL_NAME_UPPER}_INCLUDE_DIRS ${SOURCE_DIR}/ CACHE TYPE INTERNAL)

  set(${EXTERNAL_NAME_UPPER}_LIBRARIES ${SOURCE_DIR}/amd64/nvapi64.lib CACHE TYPE INTERNAL)

endif()
