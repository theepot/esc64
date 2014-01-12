get_filename_component(currentDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(Memory_INCLUDE_DIRS ${currentDir}/include)
set(Memory_LIBRARY ${currentDir}/lib/libMemory.a)
