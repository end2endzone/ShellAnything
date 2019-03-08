find_package(GTest)
find_package(rapidassist)

add_executable(shellanything_unittest
  ${SHELLANYTHING_EXPORT_HEADER}
  ${SHELLANYTHING_VERSION_HEADER}
  ${SHELLANYTHING_CONFIG_HEADER}
  main.cpp
  TestPropertyManager.cpp
  TestPropertyManager.h
)

# Unit test projects requires to link with pthread if also linking with gtest
if(NOT WIN32)
  set(PTHREAD_LIBRARIES -pthread)
endif()

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(shellanything_unittest PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

target_include_directories(shellanything_unittest PRIVATE ${GTEST_INCLUDE_DIR} rapidassist)
add_dependencies(shellanything_unittest shellanything)
target_link_libraries(shellanything_unittest PUBLIC shellanything PRIVATE ${PTHREAD_LIBRARIES} ${GTEST_LIBRARIES} rapidassist )

install(TARGETS shellanything_unittest
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
