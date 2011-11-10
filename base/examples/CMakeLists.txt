

if(BUILD_STATIC_LIBS)

      add_executable(transform_example transformExample.cpp)
      target_link_libraries(transform_example vpl-static)

      add_executable(log_example logExample.cpp)
      target_link_libraries(log_example vpl-static)
      
    elseif(BUILD_SHARED_LIBS)

      add_executable(transform_example transformExample.cpp)
      target_link_libraries(transform_example vpl)
      
      add_executable(log_example logExample.cpp)
      target_link_libraries(log_example vpl)

endif(BUILD_STATIC_LIBS)