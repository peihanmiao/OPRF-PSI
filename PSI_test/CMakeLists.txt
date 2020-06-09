#project(PSI_test) 

#############################################
#               Build PSI_test              #
#############################################

file(GLOB_RECURSE SRCS_TEST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(PSI_test ${SRCS_TEST})

#############################################
#            Link our libraries             #
#############################################
target_link_libraries(PSI_test PSI)
