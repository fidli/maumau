#ifndef WINDOWS_RENDER_H
#define WINDOWS_RENDER_H


#include <Wingdi.h>
#include <Gdiplus.h> 


#include "util_image.cpp"



struct DrawContext{
    uint32 * drawbuffer;
    BITMAPINFO drawinfo;
    HDC  backbufferDC;
    HBITMAP DIBSection;
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
    RenderType_Image
};


struct RenderItem{
    RenderType type;
    RenderItem * next;
    union{
        
    };
};

void render(RenderItem * queue){
    
    RenderItem * current = queue;
    while(current){
        //maybe switch differned render commands
        switch(current->type){
            default: ASSERT(!"Invalid");
        }
        current = current->next;
    }    
}

#endif
