
# Link with PCRE
include_directories(${PCRE_INCLUDE_DIR})

# On Windows, assume we are linking statically
if (WIN32)
  add_definitions(-DPCRE_STATIC)
endif()

add_executable(pplatex main.cpp regex.cpp outputinfo.cpp
	               outputfilter.cpp
	               latexoutputfilter.cpp)

target_link_libraries(pplatex ${PCRE_LIBRARIES})
