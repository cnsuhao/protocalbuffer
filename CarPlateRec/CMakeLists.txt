cmake_minimum_required(VERSION 2.6)

CMAKE_POLICY(SET CMP0043 NEW )
CMAKE_POLICY(SET CMP0020 NEW )

project(TCECarPlatePrj)

add_definitions(-D_WINDOWS)

SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on window")
SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows" )
SET(CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "add a postfix, usually empty on windows")
SET(CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "add a postfix, usually empty on windows")


#set(OPENCV_HPP_PATH D:/Workspace/TCE/platform/platform/common/3rdParty/opencv249/build/include)
#set(OPENCV_LIB_PATH D:/Workspace/TCE/platform/platform/common/3rdParty/opencv249/build/x86/vc12/lib)

IF(WIN32)
    #set(OpenCV_DIR D:/Workspace/TCE/platform/platform/common/3rdParty/opencv249/build)
    FIND_PACKAGE(OpenCV REQUIRED)
ELSE(WIN32)
    #set(OpenCV_DIR /home/tianzq/TCE/algorithm/3rdParty/linux/opencv-2.4.11-build)
ENDIF(WIN32)

#file(GLOB_RECURSE CURRENT_HEADER_EXTS *.h *.hpp)
#source_group　可以将文件分目录来显示在IDE中。
#所以，修改了一下base.cmake文件让所有的项目都能检索出头文件并显示在　include 文件夹中。
#source_group("Include" FILES ${CURRENT_HEADER_EXTS}) 

IF( CMAKE_SYSTEM MATCHES "Linux" )
    #add_definnitions(-g)
ENDIF( CMAKE_SYSTEM MATCHES "Linux" )

#设置输出文件路径
SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
MAKE_DIRECTORY(${OUTPUT_LIBDIR})

SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
MAKE_DIRECTORY(${OUTPUT_BINDIR})

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
IF(WIN32)
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
ELSE(WIN32)
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})
ENDIF(WIN32)

add_subdirectory(CarPlaleRec)
add_subdirectory(TCECarPlate)
add_subdirectory(testdll)
