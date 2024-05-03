set(SOURCE_FILES
    window.cpp
    window.hpp
    shader.hpp
    shader.cpp
    sphere.cpp
    sphere.hpp
    )
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
