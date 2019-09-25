/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, SDL OpenGL, standard IO, and, strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#define STB_IMAGE_IMPLEMENTATION
#include "includes/stb_image.h"

#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

GLfloat rotationX = 0.0f;
GLfloat rotationY = 0.0f;

//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes matrices and clear color
bool initGL();

//Input handler
void handleKeys( unsigned char key, int x, int y );

//Per frame update
void update();

//Renders quad to the screen
void render();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

//Render flag
bool gRenderQuad = true;

void DrawCube( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength )
{
    GLfloat halfSideLength = edgeLength * 0.5f;
    
    GLfloat vertices[] =
    {
        // front face
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom right
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left
        
        // back face
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top left
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom left
        
        // left face
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left
        
        // right face
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left
        
        // top face
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // bottom left
        
        // top face
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength  // bottom left
    };
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    //glColor3f( colour[0], colour[1], colour[2] );
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertices );

    glDrawArrays( GL_QUADS, 0, 24 );
    
    glDisableClientState( GL_VERTEX_ARRAY );
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Use OpenGL 2.1
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create context
			gContext = SDL_GL_CreateContext( gWindow );
			if( gContext == NULL )
			{
				printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Use Vsync
				if( SDL_GL_SetSwapInterval( 1 ) < 0 )
				{
					printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
				}

				//Initialize OpenGL
				if( !initGL() )
				{
					printf( "Unable to initialize OpenGL!\n" );
					success = false;
				}
			}
		}
	}

	return success;
}

bool initGL()
{
	bool success = true;
	GLenum error = GL_NO_ERROR;

	glViewport( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT ); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels

	//Initialize Projection Matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1000 ); // essentially set coordinate system
	
	//Check for error
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
		success = false;
	}

	//Initialize Modelview Matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	//Check for error
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
		success = false;
	}
	
	//Initialize clear color
	glClearColor( 0.f, 0.f, 0.f, 1.f );
	
	//Check for error
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
		success = false;
	}
	
	return success;
}

void handleKeys( unsigned char key, int x, int y )
{
	//Toggle quad
	if( key == 'q' )
	{
		gRenderQuad = !gRenderQuad;
	}

	if( key == 'w' )
    {
        rotationY -= 10.f;
    }
    else if( key == 's' )
    {
        rotationY += 10.f;
    }
    else if( key == 'a' )
    {
        rotationX -= 10.f;
    }
    else if( key == 'd' )
    {
        rotationX += 10.f;
    }
}

void update()
{
	//No per frame update needed
}

void render()
{
	//Clear color buffer
	glClear( GL_COLOR_BUFFER_BIT );
	
	//Render quad
	if( gRenderQuad )
	{
		glPushMatrix( );
        glTranslatef( SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, -500 );
        glRotatef( rotationX, 1, 0, 0 );
        glRotatef( rotationY, 0, 1, 0 );
        glTranslatef( -(SCREEN_WIDTH / 2), -(SCREEN_HEIGHT / 2), 500 );
        
        DrawCube( SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, -500, 200 );
        
        glPopMatrix();

		// White side - BACK
		// glBegin(GL_POLYGON);
		// glColor3f(   1.0,  0.0, 1.0 );
		// glVertex3f(  0.5, -0.5, 0.5 );
		// glVertex3f(  0.5,  0.5, 0.5 );
		// glVertex3f( -0.5,  0.5, 0.5 );
		// glVertex3f( -0.5, -0.5, 0.5 );
		// glEnd();

		// // Purple side - RIGHT
		// glBegin(GL_POLYGON);
		// glColor3f(  1.0,  0.0,  1.0 );
		// glVertex3f( 0.5, -0.5, -0.5 );
		// glVertex3f( 0.5,  0.5, -0.5 );
		// glVertex3f( 0.5,  0.5,  0.5 );
		// glVertex3f( 0.5, -0.5,  0.5 );
		// glEnd();

		// // Green side - LEFT
		// glBegin(GL_POLYGON);
		// glColor3f(   0.0,  1.0,  0.0 );
		// glVertex3f( -0.5, -0.5,  0.5 );
		// glVertex3f( -0.5,  0.5,  0.5 );
		// glVertex3f( -0.5,  0.5, -0.5 );
		// glVertex3f( -0.5, -0.5, -0.5 );
		// glEnd();

		// // Blue side - TOP
		// glBegin(GL_POLYGON);
		// glColor3f(   0.0,  0.0,  1.0 );
		// glVertex3f(  0.5,  0.5,  0.5 );
		// glVertex3f(  0.5,  0.5, -0.5 );
		// glVertex3f( -0.5,  0.5, -0.5 );
		// glVertex3f( -0.5,  0.5,  0.5 );
		// glEnd();

		// // Red side - BOTTOM
		// glBegin(GL_POLYGON);
		// glColor3f(   1.0,  0.0,  0.0 );
		// glVertex3f(  0.5, -0.5, -0.5 );
		// glVertex3f(  0.5, -0.5,  0.5 );
		// glVertex3f( -0.5, -0.5,  0.5 );
		// glVertex3f( -0.5, -0.5, -0.5 );
		// glEnd();
	}
}

void close()
{
	//Destroy window	
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;
		
		//Enable text input
		SDL_StartTextInput();

		//While application is running
		while( !quit )
		{
			//Handle events on queue
			while( SDL_PollEvent( &e ) != 0 )
			{
				//User requests quit
				if( e.type == SDL_QUIT )
				{
					quit = true;
				}
				//Handle keypress with current mouse position
				else if( e.type == SDL_TEXTINPUT )
				{
					int x = 0, y = 0;
					SDL_GetMouseState( &x, &y );
					handleKeys( e.text.text[ 0 ], x, y );
				}
			}

			//Render quad
			render();
			
			//Update screen
			SDL_GL_SwapWindow( gWindow );
		}
		
		//Disable text input
		SDL_StopTextInput();
	}

	//Free resources and close SDL
	close();

	return 0;
}

