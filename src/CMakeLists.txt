
# Link with PCRE
include_directories(${PCRE_INCLUDE_DIR})

add_executable(pplatex main.cpp regex.cpp outputinfo.cpp
	               outputfilter.cpp
	               latexoutputfilter.cpp)

target_link_libraries(pplatex ${PCRE_LIBRARIES})
