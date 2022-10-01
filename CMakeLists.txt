cmake_minimum_required(VERSION 3.4)
set(CMAKE_SYSTEM_NAME Generic)
set(PROJECT_NAME STM32g070_Template)
project(${PROJECT_NAME} C ASM)
set(CMAKE_C_STANDARD 17)


set(BUILD_DIR ${CMAKE_SOURCE_DIR}/bin)            #binary output directory
set(MAIN_SRC_DIR "${CMAKE_SOURCE_DIR}/src")       #source directory


#CPU specific options. Please do not modify unless you've readed 1000 pages reference manual.
set(LIBS "-lc -lm")
set(MCU_FLAGS "-mcpu=cortex-m0plus")
set(MCU_INCLUDE ${CMAKE_SOURCE_DIR}/CMSIS)
set(MCU_SOURCES  ${CMAKE_SOURCE_DIR}/startup_stm32g070kbtx.s ${CMAKE_SOURCE_DIR}/CMSIS/system_stm32g0xx.c)
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/stm32_flash.ld")
#CPU specific options end

set(TARGET "main")
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR})
set(LIBRARY_OUTPUT_PATH ${BUILD_DIR})
set(TARGET_ELF "${TARGET}.elf")
set(TARGET_HEX "${TARGET}.hex")
set(TARGET_BIN "${TARGET}.bin")


set(C_FLAGS "${MCU_FLAGS} -std=gnu11 -g3 -DSTM32G070xx -ffunction-sections -fdata-sections -Wall -fstack-usage -mfloat-abi=soft -mthumb")
set(AS_FLAGS "${MCU_FLAGS} -g3 -c -x assembler-with-cpp --specs=nano.specs -mfloat-abi=soft -mthumb")
set(LD_FLAGS "${MCU_FLAGS}  --specs=nosys.specs  -T${LINKER_SCRIPT} ${LIBS} -mfloat-abi=soft -mthumb -Wl,--print-memory-usage -u _printf_float")

macro(main_sources var) 
    set(${var} ${ARGN})
    list(TRANSFORM ${var} PREPEND "${MAIN_SRC_DIR}/")
endmacro()

add_subdirectory("${MAIN_SRC_DIR}")
main_sources(src "${MAIN_SRC}")

add_executable(${TARGET_ELF}
    "${MCU_SOURCES}"
    "${src}")

target_include_directories(${TARGET_ELF} PUBLIC Src ${MCU_INCLUDE} ${CMAKE_SOURCE_DIR})


set(CMAKE_C_FLAGS "${C_FLAGS}")
set(CMAKE_ASM_FLAGS "${AS_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${LD_FLAGS}") 


set(OBJCOPY arm-none-eabi-objcopy)

add_custom_target(${TARGET_BIN} ALL COMMAND ${OBJCOPY} -O binary -S ${BUILD_DIR}/${TARGET_ELF}  ${BUILD_DIR}/${TARGET_BIN})
add_custom_target(${TARGET_HEX} ALL COMMAND ${OBJCOPY} -O ihex -S ${BUILD_DIR}/${TARGET_ELF}  ${BUILD_DIR}/${TARGET_HEX})

add_dependencies(${TARGET_BIN} ${TARGET_ELF} ${TARGET_HEX})
