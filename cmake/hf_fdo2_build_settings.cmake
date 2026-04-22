#===============================================================================
# HF-FDO2 Driver - Build settings
#===============================================================================

include_guard(GLOBAL)

set(HF_FDO2_TARGET_NAME "hf_fdo2")

set(HF_FDO2_VERSION_MAJOR 1)
set(HF_FDO2_VERSION_MINOR 0)
set(HF_FDO2_VERSION_PATCH 0)
set(HF_FDO2_VERSION
    "${HF_FDO2_VERSION_MAJOR}.${HF_FDO2_VERSION_MINOR}.${HF_FDO2_VERSION_PATCH}")
set(HF_FDO2_VERSION_STRING "${HF_FDO2_VERSION}")

set(HF_FDO2_VERSION_TEMPLATE
    "${CMAKE_CURRENT_LIST_DIR}/../inc/fdo2_version.h.in")
set(HF_FDO2_VERSION_HEADER_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/hf_fdo2_generated")
set(HF_FDO2_VERSION_HEADER
    "${HF_FDO2_VERSION_HEADER_DIR}/fdo2_version.h")

file(MAKE_DIRECTORY "${HF_FDO2_VERSION_HEADER_DIR}")

if(EXISTS "${HF_FDO2_VERSION_TEMPLATE}")
    configure_file(
        "${HF_FDO2_VERSION_TEMPLATE}"
        "${HF_FDO2_VERSION_HEADER}"
        @ONLY
    )
    message(STATUS
        "HF-FDO2 driver v${HF_FDO2_VERSION} — generated fdo2_version.h in ${HF_FDO2_VERSION_HEADER_DIR}")
else()
    message(WARNING "fdo2_version.h.in not found at ${HF_FDO2_VERSION_TEMPLATE}")
endif()

set(HF_FDO2_PUBLIC_INCLUDE_DIRS
    "${CMAKE_CURRENT_LIST_DIR}/../inc"
    "${HF_FDO2_VERSION_HEADER_DIR}"
)

set(HF_FDO2_SOURCE_FILES "")

set(HF_FDO2_IDF_REQUIRES driver)
