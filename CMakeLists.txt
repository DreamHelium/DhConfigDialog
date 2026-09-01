project(DhConfigDialog)

option(DHCD_BUILD_EXAMPLE OFF)

add_subdirectory(src)

if(DHCD_BUILD_EXAMPLE)
  add_subdirectory(example)
endif()
