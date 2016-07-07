ADD_DEFINITIONS(-DTEST_DLL)

set(PROJECT_NAME testdll)

include_directories(
        "${OpenCV_DIR}/include"
        "../CarPlaleRec"
        )

file(GLOB_RECURSE CURRENT_HEADER_EXTS *.h *.hpp)
aux_source_directory(./ DIR_SRCS)

IF(WIN32)
ELSE(WIN32)
    link_directories(${OpenCV_DIR}/lib})
ENDIF(WIN32)

link_directories(${PROJECT_BINARY_DIR}/lib)

#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_executable(${PROJECT_NAME} ${DIR_SRCS} ${CURRENT_HEADER_EXTS})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} CarPlaleRec)