
#project(PSI)


#############################################
#                 Link Boost                #
#############################################



set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     ON)

find_package(Boost  COMPONENTS system thread)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIR}) 
  message( "Found Boost at ${Boost_LIBRARIES}")
else()
   message(FATAL_ERROR "Failed to find boost at > " ${BOOST_ROOT} " < Need system thread")
endif()

 
#############################################
#                 Link Miracl               #
#############################################
find_library(
  MIRACL_LIB 
  NAMES miracl  
  HINTS "${Miracl_Dirs}/miracl/source/")
  
# if we cant fint it, throw an error
if(NOT MIRACL_LIB)
  Message(${MIRACL_LIB})
  message(FATAL_ERROR "Failed to find miracl at " ${Miracl_Dirs})
else()
  message("Miracl at  ${MIRACL_LIB}")
endif()



#############################################
#                 Link cryptoTools          #
#############################################

find_library(
    cryptoTools_LIB
    NAMES cryptoTools
    HINTS "${libOTe_Dirs}/lib/")
    


#############################################
#                 Link libOTe               #
#############################################

find_library(
    libOTe_LIB
    NAMES libOTe
    HINTS "${libOTe_Dirs}/lib/")
    


#############################################
#              Build PSI                    #
#############################################

file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
 

add_library(PSI STATIC ${SRCS})
target_include_directories(PSI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include  ${libOTe_Dirs}/cryptoTools ${libOTe_Dirs} ${libOTe_Dirs}/libOTe ${Miracl_Dirs} ${Boost_INCLUDE_DIR})

target_link_libraries(PSI ${libOTe_LIB} ${cryptoTools_LIB} ${MIRACL_LIB} ${NTL_LIB} ${Boost_LIBRARIES})

