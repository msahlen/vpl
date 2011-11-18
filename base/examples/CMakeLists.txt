

if(BUILD_STATIC_LIBS)

      add_executable(path_example pathExample.cpp)
      target_link_libraries(path_example vpl-static)

      add_executable(log_example logExample.cpp)
      target_link_libraries(log_example vpl-static)
      
    elseif(BUILD_SHARED_LIBS)

      add_executable(path_example pathExample.cpp)
      target_link_libraries(path_example vpl)
      
      add_executable(log_example logExample.cpp)
      target_link_libraries(log_example vpl)

endif(BUILD_STATIC_LIBS)
