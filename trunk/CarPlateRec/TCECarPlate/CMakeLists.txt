ADD_DEFINITIONS(-DTCE_CARPLATE_LIBRARY)

set(LIB_NAME TCECarPlate)

add_definitions(-DTCE_CARPLATE_EXPORT)

include_directories(
        "${OpenCV_DIR}/include"
        )

file(GLOB_RECURSE CURRENT_HEADER_EXTS *.h *.hpp)
aux_source_directory(./ DIR_SRCS)
aux_source_directory(./Framework FRAMEWORK_DIR)
aux_source_directory(./Interface INTERFACE_DIR)

IF(WIN32)
ELSE(WIN32)
    link_directories(${OpenCV_DIR}/lib})
ENDIF(WIN32)

#SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_library(${LIB_NAME} SHARED ${DIR_SRCS} ${FRAMEWORK_DIR} ${INTERFACE_DIR} ${CURRENT_HEADER_EXTS})

target_link_libraries(${LIB_NAME} ${OpenCV_LIBS})