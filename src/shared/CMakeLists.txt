add_library(sa.shared STATIC
  BitmapCache.h	
  BitmapCache.cpp
  GlogUtils.h
  GlogUtils.cpp
  SaUtils.h
  SaUtils.cpp
  Win32Registry.h
  Win32Registry.cpp
  Win32Utils.h
  Win32Utils.cpp
)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(sa.shared       PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for the library.
target_include_directories(sa.shared
  PUBLIC
    $<INSTALL_INTERFACE:${SHELLANYTHING_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    rapidassist
    glog::glog
    ${CMAKE_SOURCE_DIR}/src/core
)

# Define linking dependencies.
target_link_libraries(sa.shared
  PRIVATE
    ${PTHREAD_LIBRARIES}
    ${GTEST_LIBRARIES}
    rapidassist
    glog::glog
)
