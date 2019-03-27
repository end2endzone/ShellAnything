find_package(rapidassist REQUIRED)
find_package(tinyxml2 REQUIRED)
find_package(win32clipboard REQUIRED)

set(SHELLANYTHING_HEADER_FILES ""
  ${CMAKE_SOURCE_DIR}/include/shellanything/Action.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/ActionClipboard.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/ActionExecute.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/ActionOpen.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/ActionPrompt.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/ActionProperty.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/Configuration.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/ConfigManager.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/Context.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/Icon.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/Item.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/Node.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/ObjectFactory.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/Validator.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/Platform.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/PropertyManager.h
)

add_library(shellanything STATIC
  ${SHELLANYTHING_HEADER_FILES}
  ${SHELLANYTHING_EXPORT_HEADER}
  ${SHELLANYTHING_VERSION_HEADER}
  ${SHELLANYTHING_CONFIG_HEADER}
  Action.cpp
  ActionClipboard.cpp
  ActionExecute.cpp
  ActionOpen.cpp
  ActionPrompt.cpp
  ActionProperty.cpp
  Configuration.cpp
  ConfigManager.cpp
  Context.cpp
  Icon.cpp
  Item.cpp
  Node.cpp
  ObjectFactory.cpp
  Validator.cpp
  Platform.cpp
  PropertyManager.cpp
)

# Unit test projects requires to link with pthread if also linking with gtest
if(SHELLANYTHING_BUILD_GTESTHELP)
  if(NOT WIN32)
    set(PTHREAD_LIBRARIES -pthread)
  endif()
endif()

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(shellanything PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for exported code.
target_include_directories(shellanything
  PUBLIC
    $<INSTALL_INTERFACE:${SHELLANYTHING_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    ${GTEST_INCLUDE_DIR}
    rapidassist
    tinyxml2_static
    win32clipboard
)
target_link_libraries(shellanything PRIVATE ${PTHREAD_LIBRARIES} ${GTEST_LIBRARIES} rapidassist tinyxml2_static win32clipboard)

install(TARGETS shellanything
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
