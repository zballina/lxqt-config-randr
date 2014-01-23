set(EXE_NAME lxqt-config-randr)
find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
find_package(X11 REQUIRED)

include(CheckFunctionExists)
include(FindPkgConfig)

set(CMAKE_REQUIRED_INCLUDES ${X11_Xrandr_INCLUDE_PATH}/Xrandr.h)
set(CMAKE_REQUIRED_LIBRARIES ${X11_Xrandr_LIB})
check_function_exists(XRRGetScreenSizeRange HAS_RANDR_1_2)
check_function_exists(XRRGetScreenResourcesCurrent HAS_RANDR_1_3)
find_library(XRANDR_LIBRARY NAMES Xrandr)

configure_file(config-randr.h.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/config-randr.h)

include_directories( ${X11_Xrandr_INCLUDE_PATH})

set(SOURCES_FILES
    randr.cpp
    randrmode.cpp
    randrscreen.cpp
    randrgammainfo.cpp
    randrcrtc.cpp
    randroutput.cpp
    randrdisplay.cpp
    legacyrandrscreen.cpp
    qtimerconfirmdialog.cpp
    collapsiblewidget.cpp
    outputgraphicsitem.cpp
    outputconfig.cpp
    layoutmanager.cpp
    randrconfig.cpp
    razorrandrconfiguration.cpp
    loaderconfiglogin.cpp
    main.cpp
)

set(MOC_SOURCES_FILES
    randrscreen.h
    randrcrtc.h
    randroutput.h
    legacyrandrscreen.h
    qtimerconfirmdialog.h
    collapsiblewidget.h
    outputgraphicsitem.h
    outputconfig.h
    layoutmanager.h
    randrconfig.h
#    loaderconfiglogin.h
    razorrandrconfiguration.h
)

set(UI_SOURCES_FILES
    forms/outputconfigbase.ui
    forms/qtimerconfirmdialog.ui
    forms/randrconfigbase.ui
    forms/razorrandrconfiguration.ui
)

set( RESOURCES_SRC_FILES
    lxqtconfigrandr.qrc
)
QT4_WRAP_UI(UI_FILES ${UI_SOURCES_FILES} )
QT4_WRAP_CPP(MOC_FILES ${MOC_SOURCES_FILES} )
QT4_ADD_RESOURCES(RESOURCES_FILES ${RESOURCES_SRC_FILES} )
#QT4_ADD_TRANSLATION(RAZORRANDR_QM ${RAZORRANDR_TS} )

include(${QT_USE_FILE})

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDE_DIR}
    ${X11_Xrandr_INCLUDE_PATH}
)
add_executable(${EXE_NAME}
    ${SOURCES_FILES}
    ${UI_FILES}
    ${RESOURCES_FILES}
    ${MOC_FILES}
)

target_link_libraries(${EXE_NAME}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${X11_LIBRARIES}
    ${XRANDR_LIBRARY}
)

install(TARGETS ${EXE_NAME} RUNTIME DESTINATION bin)
