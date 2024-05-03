set(SOURCE_FILES
    shader.hpp
    shader.cpp
    sphere.cpp
    sphere.hpp
    environment.hpp
    environment.cpp
    gui.hpp
    gui.cpp
    tree.hpp
    tree.cpp
    )
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
