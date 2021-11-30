cmake_minimum_required(VERSION 3.5)
project(dac-tutorial)


if(NOT DEFINED ENV{RISCV})
	message(FATAL_ERROR "set RISCV environment variable. \n Try `cd ${CMAKE_CURRENT_SOURCE_DIR}; source source.sh`")
endif()
set(ELASTICLAVE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/elasticlave)
set(ELASTICLAVE_SDK_SOURCE_DIR ${ELASTICLAVE_SOURCE_DIR}/sdk)

set(ENV{LIBSODIUM_DIR} ${ELASTICLAVE_SOURCE_DIR}/libsodium/src/libsodium)

set(ENV{PATH} $ENV{PATH}:$ENV{RISCV}/bin)

add_subdirectory(${ELASTICLAVE_SOURCE_DIR} EXCLUDE_FROM_ALL)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_C_COMPILER $ENV{RISCV}/bin/riscv64-unknown-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER $ENV{RISCV}/bin/riscv64-unknown-linux-gnu-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)


list(APPEND ELASTICLAVE_ENCL_DEPENDS linux bootrom buildroot sm driver)

add_custom_target(sdk WORKING_DIRECTORY ${ELASTICLAVE_SDK_SOURCE_DIR}
    COMMAND $(MAKE)
    DEPENDS sodium)

add_custom_target(sodium WORKING_DIRECTORY ${ELASTICLAVE_SOURCE_DIR}
    COMMAND env PATH=$ENV{PATH}:$ENV{RISCV}/bin ./make-sodium.sh)

#add_custom_target(musl WORKING_DIRECTORY ${ELASTICLAVE_SOURCE_DIR}
    #COMMAND ./make-musl.sh)

set(ELASTICLAVE_OVERLAY_DIR ${CMAKE_CURRENT_BINARY_DIR}/elasticlave/overlay/root)
set(ELASTICLAVE_BUILDROOT_SRC_DIR ${ELASTICLAVE_SOURCE_DIR}/buildroot)
set(ELASTICLAVE_BUILDROOT_WRK_DIR ${CMAKE_CURRENT_BINARY_DIR}/elasticlave/buildroot.build)
set(ELASTICLAVE_MONITOR_WRK_DIR ${CMAKE_CURRENT_BINARY_DIR}/elasticlave/riscv-pk.build)

add_custom_target(run-script ALL
    DEPENDS ${ELASTICLAVE_ENCL_DEPENDS}
    COMMAND cp ${ELASTICLAVE_SOURCE_DIR}/sdk/rts/eyrie/eyrie-rt ${ELASTICLAVE_OVERLAY_DIR}
    COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/elasticlave/linux-keystone-driver.build/keystone-driver.ko
        ${ELASTICLAVE_OVERLAY_DIR}
    COMMAND $(MAKE) -s -C ${ELASTICLAVE_BUILDROOT_SRC_DIR} RISCV=$ENV{RISCV} PATH=$ENV{PATH} O=${ELASTICLAVE_BUILDROOT_WRK_DIR}
    COMMAND $ENV{RISCV}/bin/riscv64-unknown-linux-gnu-objcopy -S -O binary --change-addresses -0x80000000 ${ELASTICLAVE_MONITOR_WRK_DIR}/bbl ${CMAKE_CURRENT_BINARY_DIR}/elasticlave/bbl.bin)

macro(add_tut tutname)
    set(tut_to_install "")
    set(tuthost "")
    add_subdirectory(${tutname} EXCLUDE_FROM_ALL)
    #set(tut_to_install "tut1-host" "tut1-enclave")
    list(TRANSFORM tut_to_install PREPEND ${CMAKE_CURRENT_BINARY_DIR}/${tutname}/)
    #string(JOIN " " tut_to_install ${tut_to_install})
    message("What the heck ${tut_to_install}")
    add_custom_target(${tutname}-executable
        DEPENDS ${tutname}
        COMMAND mkdir -p ${ELASTICLAVE_OVERLAY_DIR}/${tutname}
        COMMAND cp ${tut_to_install} ${ELASTICLAVE_OVERLAY_DIR}/${tutname}/)
    add_dependencies(run-script ${tutname}-executable)
    set(run_cmd "cd" "${tutname}" "&&" "./${tuthost}")
    add_custom_target(run-${tutname}
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/qemu-run ${ELASTICLAVE_SOURCE_DIR}/qemu/riscv64-softmmu/qemu-system-riscv64 ${CMAKE_CURRENT_BINARY_DIR}/elasticlave/bootrom.build/bootrom.bin ${CMAKE_CURRENT_BINARY_DIR}/elasticlave/riscv-pk.build/bbl ${CMAKE_CURRENT_BINARY_DIR}/elasticlave/buildroot.build/images/rootfs.ext2 '${run_cmd}')
endmacro()

add_custom_target(run
    COMMAND echo "**** Running QEMU ****"
    COMMAND ${ELASTICLAVE_SOURCE_DIR}/qemu/riscv64-softmmu/qemu-system-riscv64 -m 2G -nographic -machine virt -bios ${CMAKE_CURRENT_BINARY_DIR}/elasticlave/bootrom.build/bootrom.bin -kernel ${CMAKE_CURRENT_BINARY_DIR}/elasticlave/riscv-pk.build/bbl -append "console=ttyS0 ro root=/dev/vda" -drive file=${CMAKE_CURRENT_BINARY_DIR}/elasticlave/buildroot.build/images/rootfs.ext2,format=raw,id=hd0 -device virtio-blk-device,drive=hd0 -smp 4 -netdev user,id=net0,net=192.168.100.1/24,dhcpstart=192.168.100.128 -device virtio-net-device,netdev=net0)

add_tut(tut1)
add_tut(tut2)
add_tut(tut3)
add_tut(tut4)

