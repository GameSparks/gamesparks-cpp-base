
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel;Coverage" CACHE STRING "" FORCE)
endif()

if (CMAKE_BUILD_TYPE MATCHES Coverage)
	FIND_PROGRAM( GCOV_PATH gcov PATHS /usr/local/bin)
	FIND_PROGRAM( LCOV_PATH lcov PATHS /usr/local/bin)
	FIND_PROGRAM( GENHTML_PATH genhtml PATHS /usr/local/bin)
	FIND_PROGRAM( GCOVR_PATH gcovr PATHS /usr/local/bin)

	set(CMAKE_CXX_FLAGS_COVERAGE "-g -O0 -fprofile-arcs -ftest-coverage" CACHE STRING "" FORCE)
	set(CMAKE_C_FLAGS_COVERAGE   "-g -O0 -fprofile-arcs -ftest-coverage" CACHE STRING "" FORCE)
	mark_as_advanced(CMAKE_CXX_FLAGS_COVERAGE CMAKE_C_FLAGS_COVERAGE)

	function(target_setup_coverage executable_target)
		message(ARGS: ${ARGV1} ${ARGV2} ${ARGV3})

		# Setup target
		add_custom_target("${executable_target}Coverage"
			# lcov zerocounters
			${LCOV_PATH} --directory . --zerocounters

			# Run
			COMMAND $<TARGET_FILE:${executable_target}> ${ARGV1} ${ARGV2} ${ARGV3}

			# generate report
			COMMAND ${LCOV_PATH} --directory . --capture --output-file "${executable_target}Coverage.info"
			COMMAND ${LCOV_PATH} --remove "${executable_target}Coverage.info" 'build/*' 'tests/*' '/usr/*' --output-file "${executable_target}Coverage.info.cleaned"
			COMMAND ${GENHTML_PATH} -o "${executable_target}Coverage" "${executable_target}Coverage.info.cleaned"
			COMMAND ${CMAKE_COMMAND} -E remove "${executable_target}Coverage.info" "${executable_target}Coverage.info.cleaned"

			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			COMMENT "Resetting code coverage counters to zero.\nProcessing code coverage counters and generating report."

			DEPENDS ${executable_target}
		)

		# open report
		add_custom_command(TARGET "${executable_target}Coverage" POST_BUILD
			COMMAND open "./${executable_target}Coverage/index.html"
		)
	endfunction() # SETUP_TARGET_FOR_COVERAGE
else()
	function(target_setup_coverage executable_target)
		message(Coverage target setup disabled for non-Coverage configuration)
	endfunction()
endif()
