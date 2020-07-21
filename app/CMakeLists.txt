file(GLOB NODE_SOURCES "Node.cpp" "Webserver.cpp")
add_executable(node ${NODE_SOURCES})
target_link_libraries (node LINK_PUBLIC ${EXTERNAL_LIBS})
