cmake_minimum_required(VERSION 3.6)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

#I want to link statically to the runtime library. This will help in sharing the game with others
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug$<$<CONFIG:Debug>:Debug>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")


#declare projects
set(PROJECT_NAME samuraiEngine)
set(GAMEPLAY_NAME samuraiGameplay)
project (${PROJECT_NAME}
		LANGUAGES CXX
		VERSION 0.10)

# Get the DAMMMMNNNNN sdl2
if (NOT TARGET SDL2)
    if (MSVC)  # Windows, use clang or MSVC
        set(SDL2_ROOT "" CACHE PATH "SDL2 root directory")
# You will find SDL when you install vulkan and also clicked the box that says install SDL2
        set(SDL2_INCLUDE_DIR "$ENV{VULKAN_SDK}/Include/SDL2")
        set(SDL2_LIB_DIR "$ENV{VULKAN_SDK}/Lib")
        set(SDL2_BIN_DIR "$ENV{VULKAN_SDK}/Bin")
        add_library(SDL2::SDL2 SHARED IMPORTED GLOBAL)
        set_target_properties(SDL2::SDL2 PROPERTIES
                IMPORTED_LOCATION "${SDL2_BIN_DIR}/SDL2.dll"
                IMPORTED_IMPLIB "${SDL2_LIB_DIR}/SDL2.lib"
                INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIR}"
        )
        add_library(SDL2::SDL2main SHARED IMPORTED GLOBAL)
        set_target_properties(SDL2::SDL2main PROPERTIES
            IMPORTED_LOCATION "${SDL2_BIN_DIR}/SDL2.dll"
            IMPORTED_IMPLIB "${SDL2_LIB_DIR}/SDL2main.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIR}"
        )
        add_library(SDL2 INTERFACE IMPORTED GLOBAL)
        target_link_libraries(SDL2 INTERFACE SDL2::SDL2 SDL2::SDL2main)
        message(STATUS "SDL2_INCLUDE_DIR: ${SDL2_INCLUDE_DIR}")
    else()  # Linux, MacOSX
        find_package(SDL2 QUIET)
        if (SDL2_FOUND)
            add_library(SDL2 ALIAS SDL2::SDL2)
        else()
            find_package(PkgConfig REQUIRED)
            pkg_check_modules(SDL2 sdl2 REQUIRED IMPORTED_TARGET)
            add_library(SDL2 ALIAS PkgConfig::SDL2)
        endif()
    endif()
endif()

#set glfw to NOT use dynamic runtime and not build unnecessary stuff
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

#add third party libraries
add_subdirectory(thirdparty/glfw-3.3.2)
add_subdirectory(thirdparty/glad)
add_subdirectory(thirdparty/gl2d)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/stb_image)
add_subdirectory(thirdparty/stb_truetype)
add_subdirectory(thirdparty/imgui-docking)
add_subdirectory(thirdparty/safeSafe)

#Define some macros for the project sources
file(GLOB_RECURSE SAMURAI_SOURCES_CORE_CONFIG			CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/core/coreConfig/*.cpp")
file(GLOB_RECURSE SAMURAI_SOURCES_CORE_EDITOR			CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/core/editor/*.cpp")
file(GLOB_RECURSE SAMURAI_SOURCES_CORE_RUNTIME			CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/core/runtime/*.cpp")
file(GLOB_RECURSE SAMURAI_SOURCES_CORE_STD				CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/core/utility/*.cpp")
file(GLOB_RECURSE SAMURAI_SOURCES_GAMEPLAY				CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/gameplay/*.cpp")
file(GLOB_RECURSE SAMURAI_SOURCES_CORE_SHARED_RUNTIME	CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shared/*.cpp")

file(GLOB_RECURSE SAMURAI_HEADER_EDITOR CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/core/editor/*.h")
file(GLOB_RECURSE SAMURAI_HEADER_RUNTIME CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/core/runtime/*.h")
file(GLOB_RECURSE SAMURAI_HEADER_STD CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/core/utility/*.h")
file(GLOB_RECURSE SAMURAI_HEADER_SHARED_RUNTIME CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shared/*.h")
file(GLOB_RECURSE SAMURAI_HEADER_GAMEPLAY CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/gameplay/*.h")


add_compile_definitions(SAMURAI_RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/") 
add_compile_definitions(SAMURAI_ENGINE_RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/engineResources/") 





#core
add_executable(core)

target_compile_definitions(core PUBLIC SAMURAI_DEVELOPMENT)
target_compile_definitions(core PUBLIC SAMURAI_SDL)
set_property(TARGET core PROPERTY CXX_STANDARD 17)
target_sources(core PRIVATE 
	"${SAMURAI_SOURCES_CORE_CONFIG}" "${SAMURAI_SOURCES_CORE_EDITOR}" "${SAMURAI_SOURCES_CORE_RUNTIME}"
	"${SAMURAI_SOURCES_CORE_STD}" "${SAMURAI_SOURCES_CORE_SHARED_RUNTIME}"
    "${SAMURAI_HEADER_EDITOR}" "${SAMURAI_HEADER_RUNTIME}" "${SAMURAI_HEADER_STD}" "${SAMURAI_HEADER_SHARED_RUNTIME}")
target_include_directories(core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/core/coreConfig/")
target_include_directories(core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/core/editor/")
target_include_directories(core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/core/runtime/")
target_include_directories(core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/core/utility/")
target_include_directories(core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/shared/")
target_link_libraries(core PRIVATE SDL2 SDL2::SDL2main glad glfw glm stb_image stb_truetype imgui safeSave)


#gameplay 
add_library(gameplay SHARED)

target_compile_definitions(gameplay PUBLIC SAMURAI_DEVELOPMENT)
target_compile_definitions(gameplay PUBLIC SAMURAI_SDL)
set_property(TARGET gameplay PROPERTY CXX_STANDARD 17)
target_sources(gameplay PRIVATE "${SAMURAI_SOURCES_CORE_CONFIG}" "${SAMURAI_SOURCES_CORE_STD}" "${SAMURAI_SOURCES_GAMEPLAY}" "${SAMURAI_SOURCES_CORE_SHARED_RUNTIME}"
"${SAMURAI_HEADER_GAMEPLAY}" "${SAMURAI_HEADER_RUNTIME}" "${SAMURAI_HEADER_STD}" "${SAMURAI_HEADER_SHARED_RUNTIME}"
)
target_include_directories(gameplay PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/core/utility/")
target_include_directories(gameplay PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/core/coreConfig/")
target_include_directories(gameplay PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/gameplay/")
target_include_directories(gameplay PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/shared/")
target_link_libraries(gameplay PRIVATE SDL2::SDL2 SDL2::SDL2main glad glfw gl2d glm stb_image stb_truetype imgui safeSave)






#set exe type to be windowed
if(MSVC)
	 set_target_properties(core PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

endif()
