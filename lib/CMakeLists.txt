aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)
# add the library that runs
add_library(tools ${SRCS})
target_include_directories(tools PUBLIC "${CMAKE_SOURCE_DIR}/include")

# setup the version numbering
set_property(TARGET tools PROPERTY VERSION "1.0.0")
set_property(TARGET tools PROPERTY SOVERSION "1")

# install rules
install(
    TARGETS tools
    DESTINATION lib
)