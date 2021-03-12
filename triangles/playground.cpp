#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <chrono>
#include <thread>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

#include <common/shader.hpp>
#include <common/objloader.hpp>

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
    //glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Playground", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
    glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    
    GLuint first_programID = LoadShaders("SimpleVertexShader.glsl", "FragmentShaderFirst.glsl");
    GLuint second_programID = LoadShaders("SimpleVertexShader.glsl", "FragmentShaderSecond.glsl");

    
    GLuint MatrixID_first = glGetUniformLocation(first_programID, "MVP");
    GLuint MatrixID_second = glGetUniformLocation(second_programID, "MVP");

    mat4 Projection = perspective(radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
    mat4 View = lookAt(vec3(1, 5, 1), vec3(0, -1, 1), vec3(0, 1, 0));
    mat4 Model = mat4(1.0f);
    
    mat4 MVP = Projection * View * Model;
    
    static const GLfloat g_vertex_buffer_data_first[] = {
        .0f, .0f, .0f,
        1.0f, .0f, 1.0f,
        .5f, 1.0f, .5f,
    };
    
    static const GLfloat g_vertex_buffer_data_second[] = {
        .5f, .0f, .0f,
        .0f, .0f, .5f,
        1.0f, 1.0f, 1.0f,
    };
    
    
    //glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    
    
    GLuint vertexbuffer_first;
    glGenBuffers(1, &vertexbuffer_first);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_first);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_first), g_vertex_buffer_data_first, GL_STATIC_DRAW);
    
    GLuint vertexbuffer_second;
    glGenBuffers(1, &vertexbuffer_second);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_second);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_second), g_vertex_buffer_data_second, GL_STATIC_DRAW);
    
    GLuint VERTEX_COORD_ATTR_ID = 0;
    
    vec3 start(1, 5, 1), end(1.5, 5, -1);
    uint N_STEPS = 24 * 2;
    
    uint cur_step = 0;
    vec3 myRotationAxis(0, 0.5, 1);
    //rotate(angle, myRotationAxis);
    
	do{
        float alpha = float(cur_step % (2 * N_STEPS)) / N_STEPS;
        if (alpha > 1) {
            alpha = 2 - alpha;
        }
        
        float angle = float(cur_step) / N_STEPS;
        vec3 camera_pos;
        camera_pos[0] = end[0]*alpha + start[0]*(1 - alpha);
        camera_pos[1] = end[1]*alpha + start[1]*(1 - alpha);
        camera_pos[2] = end[2]*alpha + start[2]*(1 - alpha);
        
        View = lookAt(camera_pos, vec3(0, 0, 0), vec3(0, 1, 0));
        View = rotate(View, angle, myRotationAxis);
        MVP = Projection * View * Model;
        
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(first_programID);
        glUniformMatrix4fv(MatrixID_first, 1, GL_FALSE, &MVP[0][0]);

		// Draw nothing, see you in tutorial 2 !
        glEnableVertexAttribArray(VERTEX_COORD_ATTR_ID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_first);
        glVertexAttribPointer(VERTEX_COORD_ATTR_ID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glDrawArrays(GL_TRIANGLES, 0, sizeof(g_vertex_buffer_data_first));
        
        glDisableVertexAttribArray(VERTEX_COORD_ATTR_ID);
        
        
        glUseProgram(second_programID);
        glUniformMatrix4fv(MatrixID_second, 1, GL_FALSE, &MVP[0][0]);
        
        // Draw nothing, see you in tutorial 2 !
        glEnableVertexAttribArray(VERTEX_COORD_ATTR_ID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_second);
        glVertexAttribPointer(VERTEX_COORD_ATTR_ID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glDrawArrays(GL_TRIANGLES, 0, sizeof(g_vertex_buffer_data_second));
        
        glDisableVertexAttribArray(VERTEX_COORD_ATTR_ID);


        
        cur_step += 1;
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
        std::chrono::milliseconds timespan(42);

        std::this_thread::sleep_for(timespan);

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

