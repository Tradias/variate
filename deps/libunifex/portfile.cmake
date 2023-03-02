vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH
    SOURCE_PATH
    REPO
    facebookexperimental/libunifex
    REF
    6b4368568f83fe02ef7a8e87b5c5698e4ff0cecb
    SHA512
    12b08773f10e971d8a19cee354adec75982fb3a736953ccba6a6fde38eb7b6b0bfddca5169aeb7e118dca91582f4b862a8934c7b9e941378c99e82b2376272bb
    HEAD_REF
    broken-stdlib
    PATCHES
    do-not-link-std-coroutines.patch)

vcpkg_check_features(
    OUT_FEATURE_OPTIONS
    FEATURE_OPTIONS
    FEATURES
    test
    BUILD_TESTING
    test
    UNIFEX_BUILD_EXAMPLES)

vcpkg_cmake_configure(SOURCE_PATH ${SOURCE_PATH} OPTIONS ${FEATURE_OPTIONS} -DCMAKE_CXX_STANDARD:STRING=20)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME unifex CONFIG_PATH lib/cmake/unifex)
vcpkg_copy_pdbs()
vcpkg_fixup_pkgconfig()

file(
    INSTALL "${SOURCE_PATH}/LICENSE.txt"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
    RENAME copyright)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include" "${CURRENT_PACKAGES_DIR}/debug/share"
     "${CURRENT_PACKAGES_DIR}/include/unifex/config.hpp.in")
if(VCPKG_TARGET_IS_WINDOWS)
    file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/include/unifex/linux")
elseif(VCPKG_TARGET_IS_LINUX)
    file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/include/unifex/win32")
endif()
