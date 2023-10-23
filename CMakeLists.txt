cmake_minimum_required(VERSION 3.1)

project(
    share-a-keyb 
    VERSION 0.1
    DESCRIPTION "Share your keyboard between two PCs."
    LANGUAGES C
)

include_directories(include)

if(WIN32)
  link_libraries(wsock32 ws2_32)
endif()

file(GLOB_RECURSE src_shared
	"include/*.h"
	"src/shared/*.h"
	"src/shared/*.c"
)
file(GLOB_RECURSE src_server
	"include/*.h"
	"src/server/*.h"
	"src/server/*.c"
)
file(GLOB_RECURSE src_client
	"include/*.h"
	"src/client/*.h"
	"src/client/*.c"
)

# Generate a random port and store it in a variable
execute_process(COMMAND ${CMAKE_COMMAND} -E env python ./cmake/port-gen.py OUTPUT_VARIABLE port OUTPUT_STRIP_TRAILING_WHITESPACE)
add_compile_definitions(DESIRED_PORT=${port})
message("Randomized port: [${port}]\n")

add_executable(server ${src_shared} ${src_server})

add_executable(client ${src_shared} ${src_client})