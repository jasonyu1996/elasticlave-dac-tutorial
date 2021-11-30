cmake_minimum_required(VERSION 3.5)

set(tutname "tut2")

add_executable(${tutname}-client enclaves/client.c)
add_dependencies(${tutname}-client sdk)

add_executable(${tutname}-server enclaves/server.c)
add_dependencies(${tutname}-server sdk)

target_link_options(${tutname}-client BEFORE PRIVATE "-static" "-Wl,--undefined=exit" "-Wl,--allow-multiple-definition")
target_link_options(${tutname}-server BEFORE PRIVATE "-static" "-Wl,--undefined=exit" "-Wl,--allow-multiple-definition")

add_executable(${tutname}-host host/host.cpp)
add_dependencies(${tutname}-host sdk)

add_custom_target(${tutname} DEPENDS ${tutname}-client ${tutname}-server ${tutname}-host)

target_include_directories(${tutname}-client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/enclaves
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/app/include
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/edge/include
    $ENV{LIBSODIUM_DIR}/include)

target_link_libraries(${tutname}-client PRIVATE
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/libkeystone-eapp.a
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/libkeystone-edge.a
    $ENV{LIBSODIUM_DIR}/.libs/libsodium.a)

target_include_directories(${tutname}-server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/enclaves
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/app/include
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/edge/include
    $ENV{LIBSODIUM_DIR}/include)

target_link_libraries(${tutname}-server PRIVATE
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/libkeystone-eapp.a
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/libkeystone-edge.a
    $ENV{LIBSODIUM_DIR}/.libs/libsodium.a)

target_include_directories(${tutname}-host PRIVATE
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/host/include
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/edge/include
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/verifier)

target_link_libraries(${tutname}-host PRIVATE
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/libkeystone-host.a
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/libkeystone-edge.a
    ${ELASTICLAVE_SDK_SOURCE_DIR}/lib/libkeystone-verifier.a
    pthread)

set(tut_to_install "${tutname}-client" "${tutname}-server" "${tutname}-host" PARENT_SCOPE)
set(tuthost "${tutname}-host" PARENT_SCOPE)

