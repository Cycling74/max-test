project(Oscar)

cmake_minimum_required(VERSION 3.19)

string(REGEX REPLACE "(.*)/" "" THIS_PACKAGE_NAME "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_OSX_ARCHITECTURES x86_64;arm64)


if (${CMAKE_GENERATOR} MATCHES "Xcode")
         if (${XCODE_VERSION} VERSION_LESS 10)
              message(STATUS "Xcode 10 or higher is required. Please install from the Mac App Store.")
            return ()
        endif ()
endif ()


# Fetch the correct version of the min-api
message(STATUS "Updating Git Submodules")
execute_process(
	COMMAND				git submodule update --init --recursive
	WORKING_DIRECTORY	"${CMAKE_CURRENT_SOURCE_DIR}"
)


# Misc setup and subroutines
include(${CMAKE_CURRENT_SOURCE_DIR}/source/max-sdk-base/script/max-package.cmake)


# Generate a project for every folder in the "source/projects" folder
SUBDIRLIST(PROJECT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/source/projects)
foreach (project_dir ${PROJECT_DIRS})
	if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/projects/${project_dir}/CMakeLists.txt")
		message("Generating: ${project_dir}")
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/projects/${project_dir})
	endif ()
endforeach ()

# Comment the line below if you want automatic cmake regneration enabled
set(CMAKE_SUPPRESS_REGENERATION true)