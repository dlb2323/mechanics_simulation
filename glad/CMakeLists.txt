add_library(glad
    "include/KHR/khrplatform.h"
    "include/glad/glad.h"
    "src/glad.c"
    )
target_include_directories(glad PRIVATE "include")
