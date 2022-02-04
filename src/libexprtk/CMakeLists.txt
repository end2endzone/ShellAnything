add_library(libexprtk SHARED
  ${EXPRTK_HEADER_PATH}
  libexprtk.h
  libexprtk.cpp
  libexprtk.def
)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(libexprtk       PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# exprtk.hpp(15607): fatal error C1128: number of sections exceeded object file format limit : compile with /bigobj
target_compile_options(libexprtk PRIVATE "/bigobj")

# Define include directories for the library.
target_include_directories(libexprtk
  PUBLIC
    $<INSTALL_INTERFACE:${SHELLANYTHING_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    ${PACKAGE_DOWNLOAD_DIR}/exprtk-master
)

# Define files that will be part of the installation package.
install(FILES libexprtk.h
              DESTINATION ${SHELLANYTHING_INSTALL_INCLUDE_DIR}/shellanything
)
install(TARGETS libexprtk
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
