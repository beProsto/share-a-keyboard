cmake_minimum_required(VERSION 3.1)

project(
    share-a-keyb 
    VERSION 0.1
    DESCRIPTION "Share your keyboard between two PCs."
    LANGUAGES CXX
)

include_directories(include)

file(GLOB_RECURSE src_shared
	"include/*.hpp"
	"src/shared/*.hpp"
	"src/shared/*.cpp"
)
file(GLOB_RECURSE src_server
	"include/*.hpp"
	"src/server/*.hpp"
	"src/server/*.cpp"
)
file(GLOB_RECURSE src_client
	"include/*.hpp"
	"src/client/*.hpp"
	"src/client/*.cpp"
)
# string(RANDOM "UNIQUERANDOM" random_string 8 "0123456789ABCDEF")
# message("Random String: ${random_string}")

# Generate a random port and store it in a variable
execute_process(COMMAND ${CMAKE_COMMAND} -E env python ./cmake/port-gen.py OUTPUT_VARIABLE port OUTPUT_STRIP_TRAILING_WHITESPACE)
add_compile_definitions(DESIRED_PORT=${port})
message("Randomized port: [${port}]\n")

add_executable(server ${src_shared} ${src_server})

add_executable(client ${src_shared} ${src_client})