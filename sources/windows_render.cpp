#ifndef WINDOWS_RENDER_H
#define WINDOWS_RENDER_H


#include <Wingdi.h>
#include <Gdiplus.h> 


#include "util_image.cpp"



struct DrawContext{
    struct{
        GLuint program;
        GLuint vertexShader;
        GLuint fragmentShader;
        
        GLuint positionLocation;
        GLuint colorLocation;
    } gl;
    uint32 width;
    uint32 height;
};



DrawContext renderer;
Gdiplus::GdiplusStartupInput * gdiplusStartupInput; 
ULONG_PTR  gdiplusToken; 


static union ColorConvertor{
    uint32 complex;
    struct{
        uint8 b;
        uint8 g;
        uint8 r;
        uint8 a;
    } c;
};

static inline void loadImage(const WCHAR * file, Image * target){
    Gdiplus::Bitmap* image = Gdiplus::Bitmap::FromFile(file);
    
    target->width = ((Gdiplus::Image *) image)->GetWidth();
    target->height = ((Gdiplus::Image *) image)->GetHeight();
    
    target->pixeldata = &PPUSHA(uint32, target->width*target->height) 
    
    
        for(int w = 0; w < target->width; w++){
        for(int h = 0; h < target->height; h++){
            Gdiplus::Color c;
            image->GetPixel(w, h, &c);
            ColorConvertor col;
            col.c.r = c.GetRed();
            col.c.g = c.GetGreen();
            col.c.b = c.GetBlue();
            col.c.a = c.GetAlpha();
            int i = h*target->width  + w;
            target->pixeldata[i] = col.complex;
        }
    }
    
    delete image;
}

bool initGraphics(){
    //fecking constructors
    gdiplusStartupInput = &PPUSH(Gdiplus::GdiplusStartupInput);
    Gdiplus::GdiplusStartupInput tmp;
    *gdiplusStartupInput = tmp;
    return Gdiplus::GdiplusStartup(&gdiplusToken, gdiplusStartupInput, NULL) == 0; //Status.OK
}

void closeGraphics(){
    Gdiplus::GdiplusShutdown(gdiplusToken); 
}

enum RenderType{
    RenderType_Invalid,
    RenderType_Clear,
    RenderType_Image
};


struct RenderItem{
    RenderType type;
    RenderItem * next;
    union{
        struct {
            v2 offset;
            v2 size;
        }image;
    };
};

void render(RenderItem * queue){
    
    RenderItem * current = queue;
    while(current){
        //maybe switch differned render commands
        switch(current->type){
            case RenderType_Clear:{
                //glClear(GL_COLOR_BUFFER_BIT);
            }break;
            case RenderType_Image:{
                
                glVertexAttribPointer(renderer.gl.positionLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
                
                
                /*const float32 box[] = {
                    current->image.offset.x, current->image.offset.y,
                    current->image.offset.x + current->image.size.w, current->image.offset.y,
                    current->image.offset.x, current->image.offset.y + current->image.size.h
                };*/
                const float32 box[] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6};
                glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);
                glDrawArrays(GL_LINE_STRIP, 0, 6);
            }break;
            default: ASSERT(!"Invalid");
        }
        current = current->next;
    }    
}



static inline bool checkAndReloadShaders(){
    //GUI shader
    char vertexFilename[256] = "../sources/shaders/shader.vert";
    char fragmentFilename[256] = "../sources/shaders/shader.frag";
    
    static LocalTime vertexFiletime;
    static LocalTime fragmentFiletime;
    LocalTime oldVertexFiletime = vertexFiletime;
    LocalTime oldFragmentFiletime = fragmentFiletime;
    
    vertexFiletime = getFileChangeTime(vertexFilename);
    fragmentFiletime = getFileChangeTime(fragmentFilename);
    
    if(renderer.gl.program == 0 || oldVertexFiletime != vertexFiletime || oldFragmentFiletime != fragmentFiletime){
        
        FileContents vertexContents;
        FileContents fragmentContents;
        
        readFile(vertexFilename, &vertexContents);
        readFile(fragmentFilename, &fragmentContents);
        
        GLuint newVertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(newVertexShader, 1, &vertexContents.contents, (GLint *) &vertexContents.size);
        glCompileShader(newVertexShader);
        
        GLint status;
        glGetShaderiv(newVertexShader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(newVertexShader, GL_INFO_LOG_LENGTH, &maxLength);
            char error[1024];
            glGetShaderInfoLog(newVertexShader, maxLength, &maxLength, error);
            glDeleteShader(newVertexShader);
            INV;
            return false;
        }
        
        GLuint newFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(newFragmentShader, 1, &fragmentContents.contents, (GLint *)&fragmentContents.size);
        glCompileShader(newFragmentShader);
        
        
        glGetShaderiv(newFragmentShader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(newFragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
            char error[1024];
            glGetShaderInfoLog(newFragmentShader, maxLength, &maxLength, error);
            glDeleteShader(newFragmentShader);
            INV;
            return false;
        }
        
        GLuint newProgram = glCreateProgram();
        
        
        glAttachShader(newProgram, newVertexShader);
        glAttachShader(newProgram, newFragmentShader);
        
        glLinkProgram(newProgram);
        glGetProgramiv(newProgram, GL_LINK_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(newProgram, GL_INFO_LOG_LENGTH, &maxLength);
            char error[1024];
            glGetProgramInfoLog(newProgram, maxLength, &maxLength, error);
            glDeleteShader(newVertexShader);
            glDeleteShader(newFragmentShader);
            glDeleteProgram(newProgram);
            INV;
            return false;
        }else{
            renderer.gl.program = newProgram;
            renderer.gl.fragmentShader = newFragmentShader;
            renderer.gl.vertexShader = newVertexShader;
        }
        
        
    }
    
    renderer.gl.positionLocation = glGetAttribLocation(renderer.gl.program, "position");
    
    
    
    
    return true;
    
}

void destroyShaders(){
    glDeleteShader(renderer.gl.fragmentShader);
    glDeleteShader(renderer.gl.vertexShader);
    glDeleteProgram(renderer.gl.program);
}

#endif
