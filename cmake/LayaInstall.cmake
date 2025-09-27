# Installation configuration

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Install library
install(TARGETS laya
    EXPORT LayaTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install export targets
install(EXPORT LayaTargets
    FILE LayaTargets.cmake
    NAMESPACE laya::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/laya
)

# Generate config file
configure_package_config_file(
    cmake/LayaConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/LayaConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/laya
)

# Generate version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/LayaConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install config files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/LayaConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/LayaConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/laya
)
