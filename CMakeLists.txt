cmake_minimum_required(VERSION 3.14.0)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12 CACHE STRING "")

set(vst3sdk_SOURCE_DIR "C:/Users/pj/vst3/vst3sdk")
if(NOT vst3sdk_SOURCE_DIR)
    message(FATAL_ERROR "Path to VST3 SDK is empty!")
endif()


project(plugin1
# This is your plug-in version number. Change it here only.
# Version number symbols usable in C++ can be found in
# source/version.h and ${PROJECT_BINARY_DIR}/projectversion.h.
VERSION 1.0.0.0 
DESCRIPTION "plugin1 VST 3 Plug-in"
)


set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

smtg_add_vst3plugin(plugin1
source/version.h
source/myplugincids.h
source/mypluginprocessor.h
source/mypluginprocessor.cpp
source/myplugincontroller.h
source/myplugincontroller.cpp
source/mypluginentry.cpp
)


#- VSTGUI Wanted ----
if(SMTG_ADD_VSTGUI)
target_sources(plugin1
PRIVATE
resource/myplugineditor.uidesc
)
target_link_libraries(plugin1
PRIVATE
vstgui_support
)
smtg_target_add_plugin_resources(plugin1
RESOURCES
"resource/myplugineditor.uidesc"
)
endif(SMTG_ADD_VSTGUI)
# -------------------

smtg_target_add_plugin_snapshots (plugin1
RESOURCES
resource/7A379F59443E5A6E9EDADFABC31282BD_snapshot.png
resource/7A379F59443E5A6E9EDADFABC31282BD_snapshot_2.0x.png
)

target_link_libraries(plugin1
PRIVATE
sdk
)

target_include_directories(plugin1 "C:/Users/pj/fftw-3.3.10/api/")

smtg_target_configure_version_file(plugin1)

if(SMTG_MAC)
    smtg_target_set_bundle(plugin1
        BUNDLE_IDENTIFIER wtf
        COMPANY_NAME "pjmw"
    )
    smtg_target_set_debug_executable(plugin1
        "/Applications/VST3PluginTestHost.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(plugin1 PRIVATE 
        resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT plugin1)

        smtg_target_set_debug_executable(plugin1
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif(SMTG_MAC)
