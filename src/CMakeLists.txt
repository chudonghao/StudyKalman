
add_executable(
    ${PROJECT_NAME}
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui
    PointTableWidget.cpp
    PointTableWidget.h
)

target_link_libraries(
    ${PROJECT_NAME}
    Qt5::Widgets
)