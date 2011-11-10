SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(SFML)

if(SFML_FOUND)

    message(STATUS "Found SFML, building render examples")
    
    include_directories(${SFML_INCLUDE_DIR})
    
    add_library(example-utils STATIC exampleUtils.cpp)

    if(BUILD_STATIC_LIBS)

      add_executable(clip_example clipExample.cpp )
      target_link_libraries(clip_example vpl-static vpl-render-static example-utils)

      add_executable(blend_example blendExample.cpp )
      target_link_libraries(blend_example vpl-static vpl-render-static example-utils)

      add_executable(svg_example svgExample.cpp SVGParser.cpp )
      target_link_libraries(svg_example vpl-static vpl-render-static example-utils)
    
      add_executable(gradient_example gradientExample.cpp )
      target_link_libraries(gradient_example vpl-static vpl-render-static example-utils)

    elseif(BUILD_SHARED_LIBS)

      add_executable(clip_example clipExample.cpp)
      target_link_libraries(clip_example vpl vpl-render example-utils)

      add_executable(blend_example blendExample.cpp)
      target_link_libraries(blend_example vpl vpl-render example-utils)

      add_executable(svg_example svgExample.cpp SVGParser.cpp)
      target_link_libraries(svg_example vpl vpl-render example-utils)

      add_executable(gradient_example gradientExample.cpp)
      target_link_libraries(gradient_example vpl vpl-render example-utils)

    endif(BUILD_STATIC_LIBS)

    target_link_libraries(blend_example ${SFML_SYSTEM_LIBRARY} ${SFML_WINDOW_LIBRARY} ${SFML_GRAPHICS_LIBRARY})
    target_link_libraries(clip_example ${SFML_SYSTEM_LIBRARY} ${SFML_WINDOW_LIBRARY} ${SFML_GRAPHICS_LIBRARY})
    target_link_libraries(gradient_example ${SFML_SYSTEM_LIBRARY} ${SFML_WINDOW_LIBRARY} ${SFML_GRAPHICS_LIBRARY})
    target_link_libraries(svg_example ${SFML_SYSTEM_LIBRARY} ${SFML_WINDOW_LIBRARY} ${SFML_GRAPHICS_LIBRARY})

else(SFML_FOUND)

     message(STATUS "SFML not found, not bilding rendering examples")

endif(SFML_FOUND)
