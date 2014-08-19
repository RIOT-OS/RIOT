
#Devide specific library
#Requires macros defined in efm32.cmake to function

#Locate related directories
SET(DEVICE_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/SiliconLabs/${MCU_FAMILY_U}")

SET(DEVICE_INCLUDE_DIR "${DEVICE_BASE_DIR}/include")
SET(DEVICE_SYSTEM_NAME "${DEVICE_BASE_DIR}/Source/system_${MCU_FAMILY_L}.c")
SET(DEVICE_STARTUP_NAME "${DEVICE_BASE_DIR}/Source/GCC/startup_${MCU_FAMILY_L}.S")
SET(DEVICE_LINKER_NAME "${DEVICE_BASE_DIR}/Source/GCC/${MCU_FAMILY_L}.ld")

#Define library
include_directories(${DEVICE_INCLUDE_DIR})
add_library(device ${DEVICE_SYSTEM_NAME} ${DEVICE_STARTUP_NAME})

#Copy linker script
CONFIGURE_FILE(${DEVICE_LINKER_NAME} ${PROJECT_BINARY_DIR})
#file(COPY ${DEVICE_LINKER_NAME} DESTINATION ${PROJECT_BINARY_DIR})
