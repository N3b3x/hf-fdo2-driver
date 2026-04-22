# hf_fdo2 — ESP-IDF component wrapper (header-only driver)
set(HF_FDO2_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." CACHE PATH
    "Path to hf-fdo2-driver root")

get_filename_component(HF_FDO2_ROOT "${HF_FDO2_ROOT}" ABSOLUTE)
if(NOT EXISTS "${HF_FDO2_ROOT}/cmake/hf_fdo2_build_settings.cmake")
    message(FATAL_ERROR
        "Cannot find HF-FDO2 driver root at ${HF_FDO2_ROOT}")
endif()

include("${HF_FDO2_ROOT}/cmake/hf_fdo2_build_settings.cmake")

idf_component_register(
    INCLUDE_DIRS ${HF_FDO2_PUBLIC_INCLUDE_DIRS}
    REQUIRES ${HF_FDO2_IDF_REQUIRES}
)

target_compile_features(${COMPONENT_LIB} INTERFACE cxx_std_17)
