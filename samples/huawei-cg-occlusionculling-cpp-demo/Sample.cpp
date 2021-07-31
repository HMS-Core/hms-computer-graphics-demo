/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: A sample program for occlusion culling.
 */
#include <iostream>
#include <cstring>
#include <unistd.h>
#include "HiCulling.h"

#define CHECK_RESULT(func) { \
    HcErrorCode ret = func; \
    if (ret != HC_OK) { \
        return HC_EXIT_FAIL; \
    }; \
}

const int HC_EXIT_SUCCESS = 0;
const int HC_EXIT_FAIL = -1;

/* The number of occludees */
const int OCCLUDEE_BOX_NUM = 4;

/* Dimensions of the matrix */
const int MATRIX_DIMENSIONS = 4;

/* The index for occludees */
const int B0 = 0;
const int B1 = 1;
const int B2 = 2;
const int B3 = 3;

static void MatrixCopy(float dst[4][4], const float src[4][4], const int mDimension)
{
    for (int i = 0; i < mDimension; i++) {
        for (int j = 0; j < mDimension; j++) {
            dst[i][j] = src[i][j];
        }
    }
}

int main()
{
    /* Create software culling object */
    HiCulling *obj = HcCreate(true);
    if (obj == nullptr) {
        return HC_EXIT_FAIL;
    }
    unsigned int width = 0;
    unsigned int height = 0;
    CHECK_RESULT(HcGetResolution(obj, width, height));
    printf("Default depth buffer resulution: width=%d height=%d \n\n", width, height);

    /*
     * The parameter used in HcSetResolution must be a multiple of 2.
     * If not, the setting is invalid and an error is returned.
     */
    const unsigned int alignedWidth = 400;
    const unsigned int alignedHeight = 200;
    CHECK_RESULT(HcSetResolution(obj, alignedWidth, alignedHeight));
    CHECK_RESULT(HcGetResolution(obj, width, height));
    printf("Set new depth buffer resulution: width=%d height=%d \n\n", width, height);

    /*
     * Assuming in right-handed coordinate system, the frustum is configured as zn = 0.1,
     * w = 4 and h = 4, and corresponding projection matrix map the interval between near
     * and far plane [Zn,Zf] to [1,0].
     */
    float projection[][4] = { {0.05f, 0.0f,  0.0f,  0.0f},
                              {0.0f,  0.05f, 0.0f,  0.0f},
                              {0.0f,  0.0f,  0.0f, -1.0f},
                              {0.0f,  0.0f,  0.1f,  0.0f} };
    /*
     * aabb test case:
     * case 0: the aabb is closer to the camera than the occluder
     * case 1: the aabb is occluded by occluders0
     * case 2: the aabb is out of view frustum
     * case 3: the aabb is occluded by occluders1
     */
    float aabb[] = {-1.8f,  0.5f, -3.0f,  -1.7f, 0.6f, -1.9f,
                    -1.8f,  0.5f, -3.0f,  -1.7f, 0.6f, -2.1f,
                    -20.0f, 0.5f, -3.0f, -19.0f, 0.6f, -2.1f,
                    -1.0f, -0.5f, -2.1f,   0.0f, 0.0f, -2.1f};
    HcOccludeeBoxType occludees = { .numBox = OCCLUDEE_BOX_NUM, .boxMinMax = aabb };
    /* Let the view matrix equal the identity matrix */
    MatrixCopy(occludees.worldToClip, projection, MATRIX_DIMENSIONS);
    /* visibility of aabbs */
    bool visibility[OCCLUDEE_BOX_NUM] = { true };
    bool *result[OCCLUDEE_BOX_NUM] = { nullptr };
    for (int i = 0; i < OCCLUDEE_BOX_NUM; i++) {
        result[i] = &visibility[i];
    }

    /* Occluder Case0 */
    unsigned short indices[] = { 0, 1, 2 };
    float vertices[] = { -2.0f,  1.0f, -2.0f,
                         -2.0f, -1.0f, -2.0f,
                         1.0f,  1.0f, -2.0f };
    unsigned int case0VerticesNum = 3;
    unsigned int case0IndicesNum = 3;
    HcMeshType mesh = { .vertices = vertices, .nVertices = case0VerticesNum,
                        .indices = indices, .nIndices = case0IndicesNum };
    /* Let the world matrix and the view matrix equal the identity matrix. */
    MatrixCopy(mesh.localToClip, projection, MATRIX_DIMENSIONS);
    unsigned int case0VtxStride = 3;
    HcOccluderMeshType occluders = { .numMesh = 1, .meshes = &mesh, .bWinding = HC_CW, .idxType = HC_UNSIGNED_SHORT,
                                     .vtxStride = case0VtxStride, .nearClipDistance = 0.1f };
    CHECK_RESULT(HcAddOccluderMeshes(obj, occluders));
    CHECK_RESULT(HcRasterizeOccluder(obj));
    CHECK_RESULT(HcTestOccludeesAABB(obj, occludees, result));
    printf("Occludees visibility under Occluder case0 :\n");
    printf("aabb0: %d, aabb1: %d, aabb2: %d, aabb3: %d\n\n",
           visibility[B0], visibility[B1], visibility[B2], visibility[B3]);

    /* Occluder Case1 */
    unsigned int indices1[] = { 0, 1, 2, 3, 2, 1 };
    float vertices1[] = { -2.0f,  1.0f, -2.0f, 1.0f,
                          -2.0f, -1.0f, -2.0f, 1.0f,
                          1.0f,  1.0f, -2.0f, 1.0f,
                          1.0f, -1.0f, -2.0f, 1.0f };
    unsigned int case1VerticesNum = 4;
    unsigned int case1IndicesNum = 6;
    HcMeshType mesh1 = { .vertices = vertices1, .nVertices = case1VerticesNum,
                         .indices = indices1, .nIndices = case1IndicesNum };
    MatrixCopy(mesh1.localToClip, projection, MATRIX_DIMENSIONS);
    /* The vertex buffer with w component, and vtxStride should be configured to 4 */
    unsigned int case1VtxStride = 4;
    /* The index buffer supports different data types, it can be configured by the idxType parameter. */
    HcOccluderMeshType occluders1 = { .numMesh = 1, .meshes = &mesh1, .bWinding = HC_CCW, .idxType = HC_UNSIGNED_INT,
                                      .vtxStride = case1VtxStride, .nearClipDistance = 0.1f };
    CHECK_RESULT(HcClearBuffer(obj));
    CHECK_RESULT(HcAddOccluderMeshes(obj, occluders1));
    CHECK_RESULT(HcRasterizeOccluder(obj));
    CHECK_RESULT(HcTestOccludeesAABB(obj, occludees, result));
    printf("Occludees visibility under Occluder case1 :\n");
    printf("aabb0: %d, aabb1: %d, aabb2: %d, aabb3: %d\n\n",
           visibility[B0], visibility[B1], visibility[B2], visibility[B3]);

    /*
     * Occluder Case2:
     * Set bWinding to HC_CW, and triangles in clockwise order will not be rasterized.
     */
    HcOccluderMeshType occluders2 = { .numMesh = 1, .meshes = &mesh1, .bWinding = HC_CW, .idxType = HC_UNSIGNED_INT,
                                      .vtxStride = case1VtxStride, .nearClipDistance = 0.1f };
    CHECK_RESULT(HcClearBuffer(obj));
    CHECK_RESULT(HcAddOccluderMeshes(obj, occluders2));
    CHECK_RESULT(HcRasterizeOccluder(obj));
    CHECK_RESULT(HcTestOccludeesAABB(obj, occludees, result));
    printf("Occludees visibility under Occluder case2 :\n");
    printf("aabb0: %d, aabb1: %d, aabb2: %d, aabb3: %d\n\n",
           visibility[B0], visibility[B1], visibility[B2], visibility[B3]);

    /* Clear the depth buffer */
    CHECK_RESULT(HcClearBuffer(obj));
    HcDestroy(obj);
    return HC_EXIT_SUCCESS;
}