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

#ifndef PIC_COMPUTER_VISION_FIND_CHECKER_BOARD_HPP
#define PIC_COMPUTER_VISION_FIND_CHECKER_BOARD_HPP

#include "filtering/filter_luminance.hpp"

#include "computer_vision/iterative_closest_point_2D.hpp"
#include "computer_vision/nelder_mead_opt_ICP_2D.hpp"

#include "features_matching/orb_descriptor.hpp"

#ifndef PIC_DISABLE_EIGEN
#include "externals/Eigen/Dense"
#include "externals/Eigen/SVD"
#include "externals/Eigen/Geometry"
#include "util/eigen_util.hpp"
#endif

namespace pic {

/**
 * @brief getMinDistance
 * @param points
 * @return
 */
float getMinDistance(std::vector< Eigen::Vector2f > &points)
{
    float ret = FLT_MAX;
    for(auto i = 0; i < points.size(); i++) {

        auto p_i = points[i];

        for(auto j = 0; j < points.size(); j++) {
            if(j == i) {
                continue;
            }

            auto delta_ij = p_i - points[j];
            float dist = delta_ij.norm();

            if(dist < ret) {
                ret = dist;
            }
        }
    }

    return ret;
}

/**
 * @brief estimateCheckerBoardSize
 * @param points
 * @return
 */
float estimateCheckerBoardSize(std::vector< Eigen::Vector2f > &points)
{
    if(points.size() < 2) {
        return -1.0f;
    }

    float ret = 0.0f;

    int n = int(points.size());

    std::vector<float> m_d;
    for(int i = 0; i < n; i++) {
        auto p_i = points[i];

        float closest = FLT_MAX;

        for(int j = 0; j < n; j++) {
            if(j == i) {
                continue;
            }

            auto delta_ij = p_i - points[j];

            float dist = delta_ij.norm();

            if(dist < closest) {
                closest = dist;
            }
        }

        if(closest < FLT_MAX) {
            m_d.push_back(closest);
        }
    }

    if(!m_d.empty()) {
        std::sort(m_d.begin(), m_d.end());

        ret = m_d[m_d.size() / 2];
    }

    return ret;
}

/**
 * @brief getCheckerBoardModel
 * @param chekers_x
 * @param checkers_y
 * @param checkers_size
 * @param out
 * @return
 */
Image *getCheckerBoardModel(int checkers_x, int checkers_y, int checkers_size, std::vector< Eigen::Vector2f > &out)
{
    Image *ret = new Image(1, (checkers_x + 1) * checkers_size, (checkers_y + 1) * checkers_size, 1);
    *ret = 1.0f;

    for(int i = 1; i <= checkers_y; i++) {
        Eigen::Vector2f point;

        int y = i * checkers_size;
        point[1] = float(y);

        for(int j = 1; j <= checkers_x; j++) {

            int x = j * checkers_size;
            point[0] = float(x);

            bool bDraw = false;
            if(j < checkers_x) {
                if(((j % 2) == 0) && ((i % 2) == 0)) {
                    bDraw = true;
                }
            }

            if(i < checkers_y) {
                if(((j % 2) == 1) && ((i % 2) == 1)) {
                    bDraw = true;
                }
            }

            if(bDraw) {
                for(int yy = y; yy < (y + checkers_size); yy++) {
                    for(int xx = x; xx < (x + checkers_size); xx++) {
                        float *pixel_value = (*ret)(xx, yy);
                        pixel_value[0] = 0.0f;
                    }
                }
            }

            out.push_back(point);
        }
    }

    return ret;
}

/**
 * @brief findCheckerBoard
 * @param img
 */
void findCheckerBoard(Image *img)
{
    //compute the luminance images
    Image *L = FilterLuminance::Execute(img, NULL, LT_CIE_LUMINANCE);

    //get corners
    printf("Extracting corners...\n");
    HarrisCornerDetector hcd(2.5f, 5);
    std::vector< Eigen::Vector2f > corners_from_img;
    hcd.execute(L, &corners_from_img);

    float *col_mu = img->getMeanVal(NULL, NULL);
    float *scaling = FilterWhiteBalance::getScalingFactors(col_mu, img->channels);
    FilterWhiteBalance fwb(scaling, img->channels, true);

    Image *img_wb = fwb.Process(Single(img), NULL);

    float red[] = {1.0f, 0.0f, 0.0f};
    float green[] = {0.0f, 1.0f, 0.0f};
    float blue[] = {1.0f, 0.0f, 1.0f};
    float yellow[] = {1.0f, 1.0f, 0.0f};

    (*img_wb) *= 0.125f;

    std::vector< Eigen::Vector2f > cfi_out;
    GeneralCornerDetector::removeClosestCorners(&corners_from_img, &cfi_out, 16.0f, 100);

    //compute checkerboard size
    float checker_size = estimateCheckerBoardSize(corners_from_img);

    drawPoints(img_wb, cfi_out, blue);

    std::vector< Eigen::Vector2f > cfi_valid;
    auto n =  cfi_out.size();
    for(auto i = 0; i < n; i++) {
        auto p_i = cfi_out[i];

        bool bFlag = true;
        for(auto j = 0; j < n; j++) {
            if(j != i) {
                auto delta_ij = p_i - cfi_out[j];
                float dist = delta_ij.norm();

                if(dist < checker_size) {
                    bFlag = false;
                    break;
                }
            }
        }

        if(bFlag) {
            cfi_valid.push_back(p_i);
        }
    }

    #ifdef PIC_DEBUG
        printf("Size: %f\n", checker_size);
    #endif

    checker_size = estimateCheckerBoardSize(cfi_valid);

    drawPoints(img_wb, cfi_valid, green);

    //pattern image
    std::vector< Eigen::Vector2f > corners_model;

    int checkers_size = 32;
    Image *img_pattern = getCheckerBoardModel(4, 6, checkers_size, corners_model);
    pic::ORBDescriptor b_desc((checkers_size >> 1) + 1, checkers_size);

    std::vector< unsigned int *> descs_model, descs_cfi_valid;
    b_desc.getAll(img_pattern, corners_model, descs_model);
    b_desc.getAll(L, cfi_valid, descs_cfi_valid);

    //scale the model using the checker size
    float min_dist = getMinDistance(corners_model);
    float scaling_factor = checker_size / min_dist;

    ICP2DTransform t_init;
    t_init.scale = scaling_factor;
    t_init.applyC(corners_model);

    //run 2D ICP
    iterativeClosestPoints2D(corners_model, cfi_valid, descs_model, descs_cfi_valid, b_desc.getDescriptorSize(), 1000);

    drawPoints(img_wb, corners_model, red);


    //At this point, the rotation may be wrong so
    //this brute-force trick does the job.
    NelderMeadOptICP2D opt(corners_model, cfi_valid);

    float prev_err = FLT_MAX;
    float *x = new float[3];
    int nSample = 36;

    float *tmp = new float[4];
    for(float i = 0; i < nSample; i++) {
        float angle = float(i) * C_PI_2 / float(nSample);
        float start[] = {0.0f, 0.0f, angle};
        opt.run(start, 3, 1e-10f, 1000, tmp);

        if(opt.output_error < prev_err) {
            memcpy(x, tmp, sizeof(float) * 3);
            prev_err = opt.output_error;
        }
    }

    #ifdef PIC_DEBUG
    for(int i = 0; i < 4; i++) {
        printf("%f\n", x[i]);
    }
    #endif

    float start[] = {x[0], x[1], x[2], 1.0f};
    opt.run(start, 4, 1e-10f, 1000, tmp);
    ICP2DTransform t2(tmp[0], tmp[1], tmp[2], tmp[3]);

    #ifdef PIC_DEBUG
    for(int i = 0; i < 4; i++) {
        printf("%f\n", tmp[i]);
    }
    #endif

    t2.applyC(corners_model);
    drawPoints(img_wb, corners_model, yellow);

    #ifdef PIC_DEBUG
        img_wb->Write("../data/output/img_wb.bmp");
    #endif
}

} // end namespace pic

#endif // PIC_COMPUTER_VISION_FIND_CHECKER_BOARD_HPP