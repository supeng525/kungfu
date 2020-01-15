PROJECT(kungfu-extensions)

############################################################

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/cpp/yijinjing/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/cpp/wingchun/include)

ADD_SUBDIRECTORY(xtp)
## add ufxInfo
ADD_SUBDIRECTORY(ufx)
IF (NOT APPLE)
    ADD_SUBDIRECTORY(ctp)
ENDIF ()