if("$ENV{VTUNE_AMPLIFIER_PATH}" STREQUAL "")
    message(FATAL_ERROR "VTune Amplifier path wasn't specified")
endif()

if(WIN32)
    set(Ittnotity_LIBRARIES $ENV{VTUNE_AMPLIFIER_PATH}/lib64/libittnotify.lib)
    set(Ittnotity_INCLUDE_DIRS $ENV{VTUNE_AMPLIFIER_PATH}/include)
else()
    set(Ittnotity_LIBRARIES $ENV{VTUNE_AMPLIFIER_PATH}/lib64/libittnotify.a)
    set(Ittnotity_INCLUDE_DIRS $ENV{VTUNE_AMPLIFIER_PATH}/include)
endif()

if(EXISTS ${Ittnotity_LIBRARIES})
    set(Ittnotity_FOUND ON)
    message(STATUS "Ittnotify library found: ${Ittnotity_LIBRARIES}")
else()
    message(FATAL_ERROR "VTune Amplifier path doesn't contain libittnotify")
endif()