######################################################################
# Configure nasm?
######################################################################

if(OPTION_SSE32 OR OPTION_SSE64)

  if(NASM_EXECUTABLE)
    set(USE_SSE_ 1)
    set(BASE_NAME vplAsm)

    if(OPTION_SSE64)
       set(SSE_64_ 1)
    endif(OPTION_SSE64)
    
  set(ASM_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/asm/vplAsm.asm)
  
  include(src/asm/CMakeLists.txt)

  else(NASM_EXECUTABLE)
     message(STATUS "Not compiling SSE2 optimizations since we could'nt find nasm")
  endif(NASM_EXECUTABLE)

  add_custom_command( OUTPUT ${ASM_OBJECT}
                      COMMAND ${NASM_EXECUTABLE}
                      ARGS ${NASM_OPTIONS} ${NASM_FORMAT} -o ${ASM_OBJECT} ${ASM_SOURCE}
                      DEPENDS ${ASM_SOURCE} )

endif(OPTION_SSE32 OR OPTION_SSE64)

######################################################################
# Configure source
######################################################################

# Generate configuration header
configure_file(${CMAKE_SOURCE_DIR}/config.h.in
               ${CMAKE_CURRENT_SOURCE_DIR}/include/vplConfig.h)

# Add include directories
include_directories(include)
include_directories(src)

# Set sources
set(VPL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/vplAffineMatrix.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/vplLog.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/vplPath.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/vplStroke.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/vplVector.cpp
                ${ASM_OBJECT})

set(VPL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/vpl.h
                ${CMAKE_CURRENT_SOURCE_DIR}/include/vplAffineMatrix.h
                ${CMAKE_CURRENT_SOURCE_DIR}/include/vplArray.h
                ${CMAKE_CURRENT_SOURCE_DIR}/include/vplConfig.h
                ${CMAKE_CURRENT_SOURCE_DIR}/include/vplLog.h
                ${CMAKE_CURRENT_SOURCE_DIR}/include/vplList.h
                ${CMAKE_CURRENT_SOURCE_DIR}/include/vplMap.h
                ${CMAKE_CURRENT_SOURCE_DIR}/include/vplMemory.h
                ${CMAKE_CURRENT_SOURCE_DIR}/include/vplPath.h
                ${CMAKE_CURRENT_SOURCE_DIR}/include/vplRect.h
                ${CMAKE_CURRENT_SOURCE_DIR}/include/vplRedBlackTree.h
                ${CMAKE_CURRENT_SOURCE_DIR}/include/vplString.h
		${CMAKE_CURRENT_SOURCE_DIR}/include/vplStroke.h
                ${CMAKE_CURRENT_SOURCE_DIR}/include/vplVector.h)

#########################################################################
# Build libraries
#########################################################################

# Shared libraries
if(BUILD_SHARED_LIBS)

  if(MSVC)
    add_definitions(-DVPL_BUILDING_DLL)
  endif(MSVC)

  message(STATUS "Building shared libraries")

  add_library(vpl SHARED ${VPL_SOURCES} ${VPL_HEADERS})
  install(TARGETS vpl RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib COMPONENT shared_library)

endif(BUILD_SHARED_LIBS)

# Static libraries
if(BUILD_STATIC_LIBS)

  # Add define for static libs on windows
  if(MSVC)
  add_definitions(-DVPL_BUILDING_STATIC_LIB)
  endif(MSVC)

  message(STATUS "Building static libraries")

  add_library(vpl-static STATIC ${VPL_SOURCES} ${VPL_HEADERS})
  install(TARGETS vpl-static ARCHIVE DESTINATION lib COMPONENT static_library)

endif(BUILD_STATIC_LIBS)

#############################################################################
# Examples
############################################################################
add_subdirectory(examples)


