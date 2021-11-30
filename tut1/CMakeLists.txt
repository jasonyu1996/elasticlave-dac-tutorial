cmake_minimum_required(VERSION 3.5)


set(tutname "tut1")

add_executable(${tutname}-enclave encl/hello.c)
add_dependencies(${tutname}-enclave sdk)

target_link_options(${tutname}-enclave BEFORE PRIVATE "-static" "-Wl,--undefined=exit" "-Wl,--allow-multiple-definition")

add_executable(${tutname}-host host/host.cpp)
add_dependencies(${tutname}-host sdk)

add_custom_target(${tutname} DEPENDS ${tutname}-enclave ${tutname}-host)

target_include_directories(${tutname}-enclave PRIVATE
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/app/include
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/edge/include)

target_link_libraries(${tutname}-enclave PRIVATE
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/libkeystone-eapp.a
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/libkeystone-edge.a)

target_include_directories(${tutname}-host PRIVATE
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/host/include
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/edge/include
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/verifier)

target_link_libraries(${tutname}-host PRIVATE
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/libkeystone-host.a
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/libkeystone-edge.a
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/libkeystone-verifier.a)

set(tut_to_install "${tutname}-enclave;${tutname}-host" PARENT_SCOPE)
set(tuthost "${tutname}-host" PARENT_SCOPE)

