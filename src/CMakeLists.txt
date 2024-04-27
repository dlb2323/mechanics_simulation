set(SOURCE_FILES
    shader.hpp
    shader.cpp
    object.hpp
    object.cpp
    environment.hpp
    environment.cpp
    gui.hpp
    gui.cpp
    tree.hpp
    )
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
