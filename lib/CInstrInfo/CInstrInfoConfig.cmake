get_filename_component(currentDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(INFO_GEN_BASENAME instr_info)
set(CInstrInfo_INCLUDE_DIRS ${currentDir}/src)
set(CInstrInfo_SOURCE_FILES ${currentDir}/src/${INFO_GEN_BASENAME}.h ${currentDir}/src/${INFO_GEN_BASENAME}.c)

