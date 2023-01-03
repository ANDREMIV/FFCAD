/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library


static void
paint (GtkWidget *widget, GdkEventExpose *eev, void* surface)
{
  gint width, height;
  gint i;
  cairo_t *cr;cairo_surface_t *Surface=surface;
	//int *a=(surface);
	//printf("%d",*a);
  width  = widget->allocation.width;
  height = widget->allocation.height;
  
  cr = gdk_cairo_create (widget->window);
  
    /* clear background */
    //cairo_set_source_rgb (cr, 0.5,0.5,0.5);
cairo_set_source_surface (cr, (Surface), 0, 0);
    cairo_paint (cr);


    cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                                        CAIRO_FONT_WEIGHT_BOLD);

    /* enclosing in a save/restore pair since we alter the
     * font size
     */
    cairo_save (cr);
      cairo_set_font_size (cr, 40);
      cairo_move_to (cr, 40, 60);
      cairo_set_source_rgb (cr, 0,0,0);
      cairo_show_text (cr, "Hello World");
    cairo_restore (cr);

    cairo_set_source_rgb (cr, 1,0,0);
    cairo_set_font_size (cr, 20);
    cairo_move_to (cr, 50, 100);
    cairo_show_text (cr, "greetings from gtk and cairo");

    cairo_set_source_rgb (cr, 0,0,1);

    cairo_move_to (cr, 0, 150);
    for (i=0; i< width/10; i++)
      {
        cairo_rel_line_to (cr, 5,  10);
        cairo_rel_line_to (cr, 5, -10);
      }
    cairo_stroke (cr);

  cairo_destroy (cr);
}




void quit ( GtkWidget* self,  void* data)
{
	unsigned char*  Data=data;
	free((Data));
  glfwTerminate();
	//int *a=(data);
	//printf("%d",*a);
  exit (0);
}

int main( int   argc, 
          char *argv[] )
{

  gtk_init (&argc, &argv);
//gtk_gl_init (&argc, &argv);

static GLubyte *pixels = NULL;
static GLuint fbo;
static GLuint rbo_color;
static GLuint rbo_depth;int width=208;
int height=208;
int stride;
unsigned char *data;
cairo_surface_t *surface;
cairo_format_t format =CAIRO_FORMAT_ARGB32;
stride = cairo_format_stride_for_width (format, width);
data = malloc (stride * height);
{int i,j;for(j=0;j<height;j++)for(i=0;i<width;i++){*(unsigned int*)(&(data[i*4+j*stride]))=0xFF880000;}}

surface = cairo_image_surface_create_for_data (data, format, width, height,stride);

if (!glfwInit()) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return 1;
  } 

  // uncomment these lines if on Apple OS X
  /*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/
glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  GLFWwindow* Window = glfwCreateWindow(width, height, "Hello Triangle", NULL, NULL);
  if (!Window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(Window);
                                  
  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit();
  const GLubyte* renderer;
  const GLubyte* version;
  GLuint vao;
  GLuint vbo;

  /* geometry to use. these are 3 xyz points (9 floats total) to make a triangle */
  GLfloat points[] = { 0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, 0.0f };

  /* these are the strings of code for the shaders
  the vertex shader positions each vertex point */
  const char* vertex_shader =
    "#version 410\n"
    "in vec3 vp;"
    "void main () {"
    "  gl_Position = vec4(vp, 1.0);"
    "}";

  /* the fragment shader colours each fragment (pixel-sized area of the
  triangle) */
  const char* fragment_shader =
    "#version 410\n"
    "out vec4 frag_colour;"
    "void main () {"
    "  frag_colour = vec4(0.5, 0.0, 0.0, 1.0);"
    "}";

  /* GL shader objects for vertex and fragment shader [components] */
  GLuint vert_shader, frag_shader;
  /* GL shader programme object [combined, to link] */
GLuint shader_programme, vs, fs;



  /* get version info */
  renderer = glGetString( GL_RENDERER ); /* get renderer string */
  version  = glGetString( GL_VERSION );  /* version as a string */
  printf( "Renderer: %s\n", renderer );
  printf( "OpenGL version supported %s\n", version );

        /*  Framebuffer */
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        /* Color renderbuffer. */
        glGenRenderbuffers(1, &rbo_color);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_color);
        /* Storage must be one of: */
        /* GL_RGBA4, GL_RGB565, GL_RGB5_A1, GL_DEPTH_COMPONENT16, GL_STENCIL_INDEX8. */
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB565, width, height);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo_color);

        /* Depth renderbuffer. */
        glGenRenderbuffers(1, &rbo_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

        glReadBuffer(GL_COLOR_ATTACHMENT0);


glEnable( GL_DEPTH_TEST ); // enable depth-testing
  glDepthFunc( GL_LESS );    // depth-testing interprets a smaller value as "closer"

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( GLfloat ), points, GL_STATIC_DRAW );

  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );
  glEnableVertexAttribArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );

  vs = glCreateShader( GL_VERTEX_SHADER );
  glShaderSource( vs, 1, &vertex_shader, NULL );
  glCompileShader( vs );
  fs = glCreateShader( GL_FRAGMENT_SHADER );
  glShaderSource( fs, 1, &fragment_shader, NULL );
  glCompileShader( fs );
  shader_programme = glCreateProgram();
  glAttachShader( shader_programme, fs );
  glAttachShader( shader_programme, vs );
  glLinkProgram( shader_programme );


glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram( shader_programme );
    glBindVertexArray( vao );
    // draw points 0-3 from the currently bound VAO with current in-use shader
    glDrawArrays( GL_TRIANGLES, 0, 3 );
glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, data);

  GtkWidget *window;
  GtkWidget *drawing_area;
  GtkWidget *vbox;

  GtkWidget *button;


  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_name (window, "Test Input");

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show (vbox);

  g_signal_connect_after (window, "destroy", G_CALLBACK (quit), data);

  /* Create the drawing area */

  drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (GTK_WIDGET (drawing_area), width, height);
  gtk_box_pack_start (GTK_BOX (vbox), drawing_area, TRUE, TRUE, 0);

  gtk_widget_show (drawing_area);

  /* Signals used to handle backing pixmap */

  g_signal_connect (drawing_area, "expose_event",
		    G_CALLBACK (paint), surface);
  /*g_signal_connect (drawing_area, "configure_event",
		    G_CALLBACK (configure_event), NULL);*/

  /* Event signals */

 /* g_signal_connect (drawing_area, "motion_notify_event",
		    G_CALLBACK (motion_notify_event), NULL);
  g_signal_connect (drawing_area, "button_press_event",
		    G_CALLBACK (button_press_event), NULL);*/

  gtk_widget_set_events (drawing_area, GDK_EXPOSURE_MASK
			 | GDK_LEAVE_NOTIFY_MASK/*
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_POINTER_MOTION_MASK
			 | GDK_POINTER_MOTION_HINT_MASK*/);

  /* .. And a quit button */
  button = gtk_button_new_with_label ("Quit");
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  g_signal_connect_swapped (button, "clicked",
			    G_CALLBACK (gtk_widget_destroy),
			    window);
  gtk_widget_show (button);

  gtk_widget_show (window);

  gtk_main ();

  return 0;
}
