  /* create shader program */
const unsigned int shader_program = glCreateProgram();
{
    /* create and link vertex and fragment shaders */
    const unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    load_shader("vertex_shader.glsl", &vertex_shader);
    const unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    load_shader("fragment_shader.glsl", &fragment_shader);
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    /* handle linking errors */
    int success;
    char info_log[512];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader_program, 512, NULL, info_log);
      fprintf(stderr, "[ERROR] LINKING SHADER PROGRAM FAILED\n%s", info_log);

      load_shader("vertex_shader.glsl", &vertex_shader);
      glAttachShader(shader_program, vertex_shader);
      glAttachShader(shader_program, fragment_shader);
      glLinkProgram(shader_program);
      glDeleteProgram(shader_program);
    }
    /* delete used shader objects */
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}
const unsigned int mvp_location = glGetUniformLocation(shader_program, "u_mvp");
