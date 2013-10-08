include_directories( include )

OPTION(DEBUG_MODE
    "Build using debug code, which includes facilities for printing groups"
    OFF)

IF(DEBUG_MODE)
    MESSAGE("Adding DEBUG_MODE flag")
    ADD_DEFINITIONS(-DDEBUG_MODE)
ENDIF(DEBUG_MODE)

add_avr_executable(
    ranking
    main.c
)
