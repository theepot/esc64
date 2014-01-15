get_filename_component(VirtualIO_currentDir ${CMAKE_CURRENT_LIST_FILE} PATH)

find_package(Memory REQUIRED PATHS ${VirtualIO_currentDir}/../Memory NO_DEFAULT_PATH)

set(VirtualIO_INCLUDE_DIRS "${VirtualIO_currentDir}/include" "${Memory_INCLUDE_DIRS}")
set(VirtualIO_LIBRARY "${VirtualIO_currentDir}/lib/libVirtualIO.a" "${Memory_LIBRARY}")
