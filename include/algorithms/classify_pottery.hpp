/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#ifndef PIC_ALGORITHMS_CLASSIFY_POTTERY_HPP
#define PIC_ALGORITHMS_CLASSIFY_POTTERY_HPP

#include "util/math.hpp"

#include "image.hpp"
#include "algorithms/white_balance.hpp"
#include "filtering/filter_radial_basis_function.hpp"
#include "tone_mapping/lischinski_minimization.hpp"
#include "util/mask.hpp"

namespace pic {

#ifndef DATA_POTTERY_HPP
#define DATA_POTTERY_HPP

float data_pottery_mu_c[] = {0.7323f, 0.5415f, 0.3707f};

float data_pottery_var_distance = 0.0067f;

float data_pottery_colors[] = {
     0.905880f, 0.551079f, 0.324502f,
     0.763653f, 0.595515f, 0.439585f,
     0.622630f, 0.364378f, 0.257991f,
     0.590748f, 0.313685f, 0.185474f,
     0.824251f, 0.601703f, 0.450050f,
     0.794906f, 0.619364f, 0.436753f,
     0.821792f, 0.675837f, 0.498416f,
     0.707608f, 0.449535f, 0.341534f,
     0.855996f, 0.623774f, 0.422128f,
     0.870602f, 0.637404f, 0.384643f,
     0.644414f, 0.568198f, 0.417752f,
     0.622106f, 0.533768f, 0.396989f,
     0.583980f, 0.510922f, 0.375564f,
     0.814296f, 0.679109f, 0.491677f,
     0.482260f, 0.345150f, 0.228758f,
     0.629664f, 0.518320f, 0.394790f,
     0.596400f, 0.447404f, 0.256758f,
     0.560073f, 0.463912f, 0.306148f,
     0.529985f, 0.430025f, 0.264095f,
     0.755730f, 0.662055f, 0.535680f,
     0.616141f, 0.445908f, 0.297837f,
     0.659471f, 0.481760f, 0.317175f,
     0.833966f, 0.753722f, 0.520149f,
     0.631097f, 0.489354f, 0.341120f,
     0.526288f, 0.455893f, 0.355562f,
     0.629198f, 0.517534f, 0.383627f,
     0.713235f, 0.547767f, 0.383766f,
     0.665671f, 0.559135f, 0.420191f,
     0.836172f, 0.632880f, 0.456998f,
     0.873670f, 0.769964f, 0.608476f,
     0.906808f, 0.784524f, 0.657299f,
     0.766953f, 0.639195f, 0.511362f,
     0.755353f, 0.667762f, 0.556958f,
     0.873414f, 0.740157f, 0.553530f,
     0.687956f, 0.616581f, 0.497425f,
     0.727101f, 0.576612f, 0.476035f,
     0.781689f, 0.635009f, 0.545383f,
     0.665089f, 0.505783f, 0.392420f,
     0.733842f, 0.552824f, 0.404788f,
     0.766402f, 0.583544f, 0.449300f,
     0.779055f, 0.603955f, 0.475617f,
     0.515715f, 0.392771f, 0.270234f,
     0.562040f, 0.455849f, 0.334098f,
     0.432367f, 0.362063f, 0.275770f,
     0.820558f, 0.501371f, 0.279049f,
     0.703572f, 0.466650f, 0.279772f,
     0.756817f, 0.508563f, 0.307656f,
     0.690162f, 0.457518f, 0.293599f,
     0.780417f, 0.518715f, 0.334374f,
     0.573641f, 0.404990f, 0.282097f,
     0.655341f, 0.419693f, 0.261571f,
     0.654784f, 0.433438f, 0.287802f,
     0.706187f, 0.462480f, 0.316167f,
     0.695337f, 0.478736f, 0.327183f,
     0.728882f, 0.487880f, 0.306745f,
     0.726695f, 0.495772f, 0.330402f,
     0.712120f, 0.493872f, 0.318577f,
     0.596444f, 0.354997f, 0.221728f,
     0.674294f, 0.465968f, 0.375128f,
     0.767637f, 0.599584f, 0.428771f,
     0.760421f, 0.524806f, 0.384579f,
     0.769023f, 0.533266f, 0.366673f,
     0.652649f, 0.402764f, 0.239002f,
     0.669948f, 0.497725f, 0.297398f,
     0.864794f, 0.697683f, 0.442756f,
     0.692026f, 0.505735f, 0.343322f,
     0.717008f, 0.512693f, 0.311160f,
     0.719711f, 0.541340f, 0.376563f,
     0.749569f, 0.515989f, 0.325290f,
     0.753319f, 0.491545f, 0.314442f,
     0.783242f, 0.536050f, 0.333573f,
     0.676031f, 0.460068f, 0.303192f,
     0.592480f, 0.451014f, 0.281046f,
     0.692530f, 0.569825f, 0.365165f,
     0.963594f, 0.624278f, 0.230820f,
     0.888652f, 0.546586f, 0.257259f,
     0.890909f, 0.534012f, 0.273106f,
     0.834008f, 0.538457f, 0.306348f,
     0.659635f, 0.506418f, 0.374847f,
     0.667339f, 0.518752f, 0.338996f,
     0.678887f, 0.491503f, 0.290518f,
     0.686869f, 0.528855f, 0.314364f,
     0.725494f, 0.469987f, 0.296856f,
     0.764626f, 0.531834f, 0.403452f,
     0.711875f, 0.514270f, 0.383985f,
     0.633652f, 0.372839f, 0.212571f,
     0.859792f, 0.499931f, 0.321176f,
     0.798168f, 0.522912f, 0.389133f,
     0.862585f, 0.506567f, 0.311798f,
     0.688454f, 0.526415f, 0.366411f,
     0.780315f, 0.554559f, 0.361056f,
     0.755042f, 0.600079f, 0.441551f,
     0.712283f, 0.583090f, 0.445270f,
     0.743296f, 0.597892f, 0.444955f,
     0.836638f, 0.567583f, 0.316427f,
     0.756260f, 0.471527f, 0.308210f,
     0.708007f, 0.563116f, 0.375907f,
     0.760332f, 0.491914f, 0.305653f,
     0.684718f, 0.423006f, 0.281000f,
     0.824254f, 0.571622f, 0.437942f,
     0.812088f, 0.627018f, 0.488890f,
     0.767457f, 0.631865f, 0.412610f,
     0.702364f, 0.566919f, 0.394636f,
     0.835935f, 0.808427f, 0.569948f,
     0.840687f, 0.647521f, 0.451695f,
     0.757705f, 0.645234f, 0.438983f,
     0.753995f, 0.536740f, 0.422004f,
     0.793717f, 0.694344f, 0.479166f,
     0.691007f, 0.600893f, 0.575436f,
     0.796568f, 0.608328f, 0.376709f,
     0.790251f, 0.593221f, 0.395209f,
     0.659157f, 0.478309f, 0.320786f,
     0.700998f, 0.610203f, 0.430669f,
     0.833097f, 0.591283f, 0.405894f,
     0.823696f, 0.589574f, 0.364761f,
     0.724031f, 0.524344f, 0.360316f,
     0.688033f, 0.469742f, 0.318249f,
     0.707864f, 0.500870f, 0.332724f,
     0.722441f, 0.480724f, 0.321314f,
     0.663607f, 0.521701f, 0.418351f,
     0.721152f, 0.585068f, 0.417684f,
     0.718179f, 0.483824f, 0.270235f,
     0.600823f, 0.464321f, 0.325956f,
     0.730693f, 0.541168f, 0.397530f,
     0.598295f, 0.396023f, 0.293850f,
     0.745034f, 0.488612f, 0.335355f,
     0.705249f, 0.481905f, 0.330481f,
     0.670851f, 0.625477f, 0.488647f,
     0.808199f, 0.553863f, 0.367883f,
     0.841836f, 0.698494f, 0.412466f,
     0.575360f, 0.414156f, 0.306605f,
     0.687219f, 0.538317f, 0.350663f,
     0.718699f, 0.438491f, 0.294296f,
     0.673970f, 0.418290f, 0.310875f,
     0.704020f, 0.429756f, 0.318432f,
     0.788929f, 0.579650f, 0.373385f,
     0.704772f, 0.504252f, 0.323225f,
     0.718221f, 0.453715f, 0.294117f,
     0.751163f, 0.500053f, 0.317387f,
     0.759800f, 0.488985f, 0.345410f,
     0.725358f, 0.584621f, 0.430615f,
     0.705827f, 0.567039f, 0.419231f,
     0.570291f, 0.337304f, 0.232464f,
     0.750234f, 0.447538f, 0.275323f,
     0.565289f, 0.360465f, 0.255439f,
     0.656323f, 0.430128f, 0.294309f,
     0.673637f, 0.440270f, 0.305129f,
     0.688219f, 0.606699f, 0.442338f,
     0.769524f, 0.524712f, 0.370449f,
     0.820324f, 0.543587f, 0.365516f,
     0.678666f, 0.521523f, 0.379170f,
     0.606721f, 0.386053f, 0.271043f,
     0.784671f, 0.721454f, 0.585207f,
     0.726181f, 0.476962f, 0.292561f,
     0.623600f, 0.566590f, 0.460150f,
     0.710883f, 0.568604f, 0.429637f,
     0.764222f, 0.533366f, 0.354165f,
     0.724621f, 0.567116f, 0.390913f,
     0.625083f, 0.451748f, 0.316139f,
     0.621494f, 0.395761f, 0.277835f,
     0.759817f, 0.605538f, 0.430626f,
     0.807997f, 0.748994f, 0.570144f,
     0.820408f, 0.693392f, 0.449494f,
     0.807488f, 0.659545f, 0.344929f,
     0.817591f, 0.674640f, 0.373004f,
     0.910433f, 0.801914f, 0.431528f,
     0.851552f, 0.661569f, 0.337149f,
     0.912385f, 0.710592f, 0.480525f,
     0.889140f, 0.656206f, 0.468313f,
     0.867347f, 0.668786f, 0.477773f,
     0.902716f, 0.702486f, 0.495801f,
     0.903408f, 0.707121f, 0.467009f,
     0.758545f, 0.524321f, 0.312592f,
     0.851895f, 0.580750f, 0.331892f,
     0.737622f, 0.527599f, 0.372707f,
     0.932369f, 0.614163f, 0.285938f,
     0.886393f, 0.648476f, 0.365666f};
#endif //DATA_POTTERY_HPP

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief classifyPottery
 * @param img
 * @param white_pixel
 * @return
 */
bool *classifyPottery(Image *img, float *white_pixel)
{
    RadialBasisFunction rbf;
    rbf.update(data_pottery_colors, 177, 3, data_pottery_var_distance);

    FilterRadialBasisFunction flt_rbf;
    flt_rbf.update(&rbf);

    FilterWhiteBalance flt_wb;
    flt_wb.update(white_pixel, img->channels, true);

    Image *img_wb = NULL;

    bool bFlag = true;
    if(white_pixel != NULL) {
       img_wb = applyWhiteBalance(img, white_pixel);
    } else {
        img_wb = img;
        bFlag = false;
    }

    #ifdef PIC_DEBUG
        img_wb->Write("../data/output/s_input_wb.bmp");
    #endif

    Image *img_wb_rbf = flt_rbf.ProcessP(Single(img_wb), NULL);

    img_wb_rbf->clamp(0.0f, 1.0f);

    Image *img_L = FilterLuminance::Execute(img, NULL);

    Image *opt = LischinskiMinimization(img_L, img_wb_rbf);

    float value = 1.0f;
    bool *mask = opt->convertToMask(&value, 0.25f, false, NULL);

    int width = opt->width;
    int height = opt->height;
    bool *tmp;
    tmp = MaskDilate(mask, NULL, width, height, 3);
    MaskDilate(tmp, mask, width, height, 3);
    MaskDilate(mask, tmp, width, height, 3);
    MaskDilate(tmp, mask, width, height, 3);

    MaskRemoveIsolatedPixels(mask, tmp, width, height);

    MaskErode(tmp, mask, width, height, 3);
    MaskErode(mask, tmp, width, height, 3);
    MaskErode(tmp, mask, width, height, 3);


    #ifdef PIC_DEBUG
        opt->convertFromMask(mask, width, height);
        opt->Write("../data/output/opt.bmp");
    #endif

    if(tmp != mask) {
        delete[] tmp;
    }

    delete opt;
    delete img_L;
    delete img_wb_rbf;

    if(bFlag) {
        delete img_wb;
    }

    return mask;
}

#endif

} // end namespace pic

#endif /* PIC_ALGORITHMS_CLASSIFY_POTTERY_HPP */
