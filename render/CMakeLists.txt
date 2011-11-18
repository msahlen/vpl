######################################################################
# Configure nasm?
######################################################################

set(ASM_SOURCE "")

if(OPTION_SSE32 OR OPTION_SSE64)
  
  if(NASM_EXECUTABLE)
    set(USE_SSE_ 1)
  
    set(BASE_NAME vplBlendSSE)
    
    if(OPTION_SSE64)
       set(ASM_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/vplBlend64.asm)
    else(OPTION_SSE64)
       set(ASM_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/vplBlend32.asm)
    endif(OPTION_SSE64)   
  
  include(../base/src/asm/CMakeLists.txt)
  
  add_custom_command( OUTPUT ${ASM_OBJECT}
	              COMMAND ${NASM_EXECUTABLE}
	              ARGS ${NASM_OPTIONS} ${NASM_FORMAT} -o ${ASM_OBJECT} ${ASM_SOURCE}
	              DEPENDS ${ASM_SOURCE})
	              
  else(NASM_EXECUTABLE)
     message(STATUS "Not compiling SSE2 blending functions since we could'nt find nasm")  
  endif(NASM_EXECUTABLE)

endif(OPTION_SSE32 OR OPTION_SSE64)  

######################################################################
# Configure source
######################################################################

if(BUILD_RENDERER)

# Add include directories
include_directories(../base/include)
include_directories(include)
include_directories(src)

set(VPL_RENDER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/vplBlend.h
		       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplColorHelper.h
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplEdgeGenerator.h
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplEdgeGenerator.cpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplEvenOddMaskGenerator.h
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplEvenOddMaskGenerator.cpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplFill.h
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplFill.cpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplFixedPoint.h
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplGradient.cpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplHorizontalSpan.h
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplNonZeroMask.h
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplNonZeroMaskGenerator.h
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplNonZeroMaskGenerator.cpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplPixelBuffer.cpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplPixelCoverage.h
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplRender.cpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplRenderer.cpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplSampleMasks.h
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplScanLineList.h
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/vplScanLineList.cpp
                       ${ASM_OBJECT})


set(VPL_RENDER_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/vplColor.h
                       ${CMAKE_CURRENT_SOURCE_DIR}/include/vplGradient.h
                       ${CMAKE_CURRENT_SOURCE_DIR}/include/vplRender.h)

#########################################################################
# Build libraries
#########################################################################

# Shared libraries
if(BUILD_SHARED_LIBS)

  if(MSVC)
    add_definitions(-DVPL_BUILDING_DLL)
  endif(MSVC)

  add_library(vpl-render SHARED ${VPL_RENDER_SOURCES} ${VPL_RENDER_HEADERS})
  target_link_libraries(vpl-render vpl)
  install(TARGETS vpl-render RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib COMPONENT shared_library)

endif(BUILD_SHARED_LIBS)

# Static libraries
if(BUILD_STATIC_LIBS)

  # Add define for static libs on windows
  if(MSVC)
  add_definitions(-DVPL_BUILDING_STATIC_LIB)
  endif(MSVC)

  message(STATUS "Building static libraries")

  add_library(vpl-render-static STATIC ${VPL_RENDER_SOURCES} ${VPL_RENDER_HEADERS})
  target_link_libraries(vpl-render-static vpl-static)
  install(TARGETS vpl-render-static ARCHIVE DESTINATION lib COMPONENT static_library)
  
endif(BUILD_STATIC_LIBS)

#############################################################################
# Examples
############################################################################

add_subdirectory(examples)

endif(BUILD_RENDERER)
