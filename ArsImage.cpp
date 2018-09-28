/*MIT License
*
*Copyright (c) 2018 Alysson Ribeiro da Silva
*
*Permission is hereby granted, free of charge, to any person obtaining a copy 
*of this software and associated documentation files (the "Software"), to deal 
*in the Software without restriction, including *without limitation the rights 
*to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
*copies of the Software, and to permit persons to whom the Software is furnished 
*to do so, subject *to the following conditions:
*
*The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
*EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. *IN NO EVENT SHALL THE AUTHORS 
*OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN 
*AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH 
*THE *SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//--------------------------LOCAL INCLUDES------------------------
#include "Ars.h"

//--------------------------GLOBAL INCLUDES-----------------------
#include <GL/glew.h>

//--------------------------MACROS--------------------------------

//--------------------------DIRECTIVES----------------------------

//--------------------------NAMESPACES----------------------------
using namespace Ars;

//----------------------------------------------------------------
ArsImage::ArsImage(const char* fileName){
	/* Generates an id to store the image into */
	ILuint    imageId = ilGenImage();

	/* Bind the generated id to the corrent state image */
	ilBindImage(imageId);

	/* Try to load the image from the native DevIL library */
    string filePath   = _contentPath + fileName;

	ILboolean success = ilLoadImage((const ILstring)filePath.c_str());

	//printf("%s \n", filePath.c_str());

	/* If image cannot be loaded send a signal to error handler method */
	if(!success){
		/* Set the working image to null to prevent future errors */
		ilBindImage(0x0);

		/* Delete the generated id */
		ilDeleteImages(1, &imageId);

		/* Check signal for errors */
		VerifySignal(CANNOT_LOAD_IMAGE);

		/* Set trash identifier id */
		id = -1;

	}else{
		/* Now load the image into openGL memory */
		width  = ilGetInteger(IL_IMAGE_WIDTH );
		height = ilGetInteger(IL_IMAGE_HEIGHT);
		format = ilGetInteger(IL_IMAGE_FORMAT);
		type   = ilGetInteger(IL_IMAGE_TYPE  );
		data   = ilGetData();

		GLuint texId;

		/* Generate an tex id to work on */
		glGenTextures(1, &texId);

		/* Set the current id to work */
		glBindTexture(GL_TEXTURE_2D, texId);

		/* Configure texture parameters */
		glPixelStorei(GL_UNPACK_SWAP_BYTES , GL_FALSE);
		glPixelStorei(GL_UNPACK_ROW_LENGTH , 0		 );
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0		 );
		glPixelStorei(GL_UNPACK_SKIP_ROWS  , 0		 );
		glPixelStorei(GL_UNPACK_ALIGNMENT  , 1		 );

		/* Generate an texture and put the data with the current configured parameters */
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);

		/* filters */
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S	 , GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T	 , GL_REPEAT );

		/* Configure current texture id into this object for later use */
		id = texId;

		/* Set no textures into working area to avoid future errors */
		glBindTexture(GL_TEXTURE_2D, 0x0);
	}
}
//----------------------------------------------------------------
ArsImage::~ArsImage(void){
	/* Just free the memory when necessary */
	if(id != -1){
		ilDeleteImage(id);

		GLuint glId = id;

		glDeleteTextures(1, &glId);

		if(data != NULL){
			free(data);
			data = NULL;
		}
	}
}
//----------------------------------------------------------------
int ArsImage::getId(){
	return id;
}
//----------------------------------------------------------------
int ArsImage::getWidth(){
	return width;
}
//----------------------------------------------------------------
int ArsImage::getHeight(){
	return height;
}
//----------------------------------------------------------------
int ArsImage::getFormat(){
	return format;
}
//----------------------------------------------------------------
void* ArsImage::getData(){
	return data;
}
