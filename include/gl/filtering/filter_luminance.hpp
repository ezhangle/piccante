/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_GL_FILTERING_FILTER_LUMINANCE_HPP
#define PIC_GL_FILTERING_FILTER_LUMINANCE_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

class FilterGLLuminance: public FilterGL
{
protected:

    void InitShaders();

public:
    //Basic constructor
    FilterGLLuminance();

    //Processing
    ImageRAWGL *Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);

    static ImageRAWGL *Execute(ImageRAWGL *imgIn, ImageRAWGL *imgOut)
    {
        FilterGLLuminance filter;
        imgOut = filter.Process(SingleGL(imgIn), imgOut);
        return imgOut;
    }

    static ImageRAWGL *Execute(std::string nameIn, std::string nameOut)
    {
        ImageRAWGL imgIn(nameIn);
        imgIn.generateTextureGL(false);

        ImageRAWGL *imgOut = new ImageRAWGL(1, imgIn.width, imgIn.height, 1, IMG_CPU);

        imgOut = Execute(&imgIn, imgOut);


        imgOut->loadToMemory();
        imgOut->Write(nameOut);
        return imgOut;
    }
};

FilterGLLuminance::FilterGLLuminance(): FilterGL()
{
    InitShaders();
}

void FilterGLLuminance::InitShaders()
{
    fragment_source = GLW_STRINGFY
                      (
    uniform sampler2D u_tex; \n
    out     vec4      f_color; \n
    const vec3 LUM_XYZ =   vec3(0.213, 0.715,  0.072); \n
    \n
    void main(void) {
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy);\n
        \n
        vec3  color = texelFetch(u_tex, coords, 0).xyz;\n
        \n
        float L = dot(color, LUM_XYZ);\n
        f_color = vec4(L, L, L, 1.0);\n
        \n
    }
                      );

    std::string prefix;
    prefix += glw::version("150");
    prefix += glw::ext_require("GL_EXT_gpu_shader4");

    filteringProgram.setup(prefix, vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();
    filteringProgram.uniform("u_tex",      0);
    glw::bind_program(0);
}

//Processing
ImageRAWGL *FilterGLLuminance::Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return imgOut;
    }

    if(imgIn[0]->channels != 3) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;

    if(imgOut == NULL) {
        imgOut = new ImageRAWGL(1, w, h, 1, IMG_GPU);
    }

    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(w, h, 1, false, imgOut->getTexture());

    //Rendering
    fbo->bind();
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    //Shaders
    glw::bind_program(filteringProgram);

    //Textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, imgIn[0]->getTexture());

    //Rendering aligned quad
    quad->Render();

    //Fbo
    fbo->unbind();

    //Shaders
    glw::bind_program(0);

    //Textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_LUMINANCE_HPP */
