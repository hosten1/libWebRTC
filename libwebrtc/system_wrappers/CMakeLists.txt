set(WEBRTC_SYSTEM_WRAPPERS_SOURCES
    ${WEBRTC_SRC_DIR}/system_wrappers/source/field_trial.cc
    ${WEBRTC_SRC_DIR}/system_wrappers/source/rtp_to_ntp_estimator.cc
    ${WEBRTC_SRC_DIR}/system_wrappers/source/clock.cc
    ${WEBRTC_SRC_DIR}/system_wrappers/source/cpu_features.cc
    ${WEBRTC_SRC_DIR}/system_wrappers/source/metrics.cc
    ${WEBRTC_SRC_DIR}/system_wrappers/source/sleep.cc
)

add_library(system_wrappers OBJECT
    ${WEBRTC_SYSTEM_WRAPPERS_SOURCES}
)

target_include_directories(
    system_wrappers
    PRIVATE
        ${WEBRTC_SRC_DIR}
        ${DEPS_DIR}/abseil-cpp/abseil-cpp
)

list(LENGTH WEBRTC_SYSTEM_WRAPPERS_SOURCES _total)
message(STATUS "[system_wrappers] Total source files: ${_total}")
message(STATUS "[system_wrappers] Sources: field_trial, rtp_to_ntp_estimator, clock, cpu_features, metrics, sleep")
message(STATUS "[system_wrappers] ====== End of system_wrappers ======")