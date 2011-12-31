#include <arm_neon.h>

namespace math {
    namespace internal {
#define _IOS_SHUFFLE_1032(vec) vrev64q_f32(vec)
#define _IOS_SHUFFLE_2301(vec) vcombine_f32(vget_high_f32(vec), vget_low_f32(vec))
        inline float32x4_t dot4VecResult(const float32x4_t& vec1, const float32x4_t& vec2) {
            float32x4_t result = vmulq_f32(vec1, vec2);
            result = vaddq_f32(result, _IOS_SHUFFLE_1032(result));
            result = vaddq_f32(result, _IOS_SHUFFLE_2301(result));
            return result;
        }

        inline float32x4_t fastRSqrt(const float32x4_t& vec) {
            float32x4_t result;
            result = vrsqrteq_f32(vec);
            result = vmulq_f32(vrsqrtsq_f32(vmulq_f32(result, result), vec), result);
            return result;
        }

        union qword {
            int32x4_t   ivec;
            uint32x4_t  uvec;
            float32x4_t fvec;
            uint8x16_t  bytes;
        };

        typedef qword HwVector;

        typedef union {
            float32_t array[16];
            struct {
                HwVector row0;
                HwVector row1;
                HwVector row2;
                HwVector row3;
            };
        } HwMatrix;
    }

    struct Vector3 {
        union {
            struct {
                float x;
                float y;
                float z;
                float pad;
            };

            float                   array[3];
            internal::HwVector      hwVector;
        };
    };

    struct Matrix43 {
        union {
            struct {
                float   row_0[4];
                float   row_1[4];
                float   row_2[4];
                float   row_3[4];
            };

            struct {
                float   _00, _10, _20, _pad0;
                float   _01, _11, _21, _pad1;
                float   _02, _12, _22, _pad2;
                float   _03, _13, _23, _pad3;
            };

            float                 array[16];
            internal::HwMatrix    hwMatrix;
        };

        inline Matrix43() {}
        Matrix43(
            const float m00, const float m10, const float m20,
            const float m01, const float m11, const float m21,
            const float m02, const float m12, const float m22,
            const float m03, const float m13, const float m23);
    };

    inline Vector3 operator-(const Vector3& srcVector1, const Vector3& srcVector2) {
        Vector3 result;
        result.hwVector.fvec = vsubq_f32(srcVector1.hwVector.fvec, srcVector2.hwVector.fvec);
        return result;
    }

    inline Vector3 normalize(const Vector3& v1) {
        float32x4_t dot;
        dot = vsetq_lane_f32(0.0f, v1.hwVector.fvec, 3);
        dot = internal::dot4VecResult(dot, dot);

        if (vgetq_lane_f32(dot, 0) == 0.0f) {
            return v1;
        } else {
            Vector3 result;
            result.hwVector.fvec = vmulq_f32(v1.hwVector.fvec, internal::fastRSqrt(dot));
            return result;
        }
    }

    inline Vector3 cross(const Vector3& v1, const Vector3& v2) {
        float32x4x2_t v_1203 = vzipq_f32(vcombine_f32(vrev64_f32(vget_low_f32(v1.hwVector.fvec)), vrev64_f32(vget_low_f32(v2.hwVector.fvec))), vcombine_f32(vget_high_f32(v1.hwVector.fvec), vget_high_f32(v2.hwVector.fvec)));
        float32x4x2_t v_2013 = vzipq_f32(vcombine_f32(vrev64_f32(vget_low_f32(v_1203.val[0])), vrev64_f32(vget_low_f32(v_1203.val[1]))), vcombine_f32(vget_high_f32(v_1203.val[0]), vget_high_f32(v_1203.val[1])));

        Vector3 result;
        result.hwVector.fvec = vmlsq_f32(vmulq_f32(v_1203.val[0], v_2013.val[1]), v_1203.val[1], v_2013.val[0]);
        return result;
    }

    inline float dot(const Vector3& v1, const Vector3& v2) {
        Vector3 vTmp;
        vTmp.hwVector.fvec = vmulq_f32(v1.hwVector.fvec, v2.hwVector.fvec);
        return (vTmp.x + vTmp.y + vTmp.z);
    }

    inline Matrix43 transpose3x3(const Matrix43& matrix) {
        float32x4x4_t data;
        data.val[0] = matrix.hwMatrix.row0.fvec;
        data.val[1] = matrix.hwMatrix.row1.fvec;
        data.val[2] = matrix.hwMatrix.row2.fvec;

        Matrix43 result;
        vst4q_f32(result.hwMatrix.array, data);
        result.hwMatrix.row3.fvec = matrix.hwMatrix.row3.fvec;
        return result;
    }
}

namespace cam {
    math::Matrix43 createMatrixLookAt(const math::Vector3& pEye, const math::Vector3& pAt, const math::Vector3& pUp) {
        math::Vector3 zaxis = normalize(pEye - pAt);
        math::Vector3 xaxis = normalize(cross(pUp, zaxis));
        math::Vector3 yaxis = cross(zaxis, xaxis);
        math::Matrix43 result = math::Matrix43(
            xaxis.x,		  yaxis.x,			zaxis.x,
            xaxis.y,		  yaxis.y,			zaxis.y,
            xaxis.z, 		  yaxis.z,			zaxis.z,
            -dot(xaxis, pEye), -dot(yaxis, pEye), -dot(zaxis, pEye));
        return result;
    }

    void recalculateView(const math::Vector3& pos, const math::Vector3& dir, const math::Vector3& up) {
        math::Matrix43 _view = createMatrixLookAt(pos, dir, up);
        math::Matrix43 _viewInverse = transpose3x3(_view);
    }
}

int main()
{
    return 0;
}
