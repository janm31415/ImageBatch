
set(HDRS
../stb/stb_image.h
../stb/stb_image_write.h
CmdList.h
raw_to_rgb.h
    )
	
set(SRCS
CmdList.cpp
main.cpp
raw_to_rgb.cpp
)


if (WIN32)
set(CMAKE_C_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_CXX_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_C_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi")
set(CMAKE_CXX_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi")
endif(WIN32)

include ("../jtk/jtk/jtk.cmake")

# general build definitions
add_definitions(-DNOMINMAX)
add_definitions(-D_UNICODE)
add_definitions(-DUNICODE)
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(ImageBatch ${HDRS} ${SRCS} )

source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})

 target_include_directories(ImageBatch
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../jtk
        )	
	
target_link_libraries(ImageBatch
        )	