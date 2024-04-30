set(SOURCE_FILES
    environment.cpp
    environment.hpp
    gui.cpp
    gui.hpp
    object.cpp
    object.hpp
    shader.cpp
    shader.hpp
    simulation.cpp
    simulation.hpp
    tree.hpp
    utils.h
    )
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
