set(EXE_NAME razorxrandr)
find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
find_package(X11 REQUIRED)

include(CheckFunctionExists)
include(FindPkgConfig)

set(CMAKE_REQUIRED_INCLUDES ${X11_Xrandr_INCLUDE_PATH}/Xrandr.h)
set(CMAKE_REQUIRED_LIBRARIES ${X11_Xrandr_LIB})
check_function_exists(XRRGetScreenSizeRange HAS_RANDR_1_2)
check_function_exists(XRRGetScreenResourcesCurrent HAS_RANDR_1_3)

configure_file(config-randr.h.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/config-randr.h)

include_directories( ${X11_Xrandr_INCLUDE_PATH})

set(RAZORRANDR_SOURCES
    randr.cpp
    randrmode.cpp
    randrscreen.cpp
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
    main.cpp
)

set(RAZORRANDR_MOC_SOURCES
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
    razorrandrconfiguration.h
)

set(RAZORRANDR_UI_SOURCES
    forms/outputconfigbase.ui
    forms/qtimerconfirmdialog.ui
    forms/randrconfigbase.ui
    forms/razorrandrconfiguration.ui
)

QT4_WRAP_UI(RAZORRANDR_UI ${RAZORRANDR_UI_SOURCES} )
QT4_WRAP_CPP(RAZORRANDR_MOC ${RAZORRANDR_MOC_SOURCES} )
#QT4_ADD_RESOURCES(RAZORRANDR_RCC ${RAZORRANDR_RCC_SRC} )
#QT4_ADD_TRANSLATION(RAZORRANDR_QM ${RAZORRANDR_TS} )

include(${QT_USE_FILE})

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDE_DIR}
    ${X11_Xrandr_INCLUDE_PATH}
)
add_executable(${EXE_NAME}
    ${RAZORRANDR_SOURCES}
    ${RAZORRANDR_UI}
    ${RAZORRANDR_MOC}
)

target_link_libraries(${EXE_NAME}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${X11_LIBRARIES}
)

install(TARGETS ${EXE_NAME} RUNTIME DESTINATION bin)
