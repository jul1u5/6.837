#include "curve.h"

#include "Matrix4f.h"
#include "Vector3f.h"
#include "extra.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <optional>

using namespace std;

namespace {
// Approximately equal to.  We don't want to use == because of
// precision issues with floating point.
inline bool approx(const Vector3f &lhs, const Vector3f &rhs) {
    const float eps = 1e-8f;
    return (lhs - rhs).absSquared() < eps;
}

const Matrix4f bezierBasis{
    1, -3, 3,  -1, //
    0, 3,  -6, 3,  //
    0, 0,  3,  -3, //
    0, 0,  0,  1   //
};

const Matrix4f bsplineBasis = Matrix4f{
    1, -3, 3,  -1, //
    4, 0,  -6, 3,  //
    1, 3,  3,  -3, //
    0, 0,  0,  1   //
} /= 6;

} // namespace

Vector3f orthogonal(const Vector3f &v) {
    const array<Vector3f, 3> &candidates{Vector3f(0, v.z(), -v.y()),
                                         Vector3f(-v.z(), 0, v.x()),
                                         Vector3f(-v.y(), v.x(), 0)};

    return *max_element(candidates.begin(), candidates.end(),
                        [](const Vector3f &x, const Vector3f &y) {
                            return x.absSquared() < y.absSquared();
                        });
}

Matrix4f points2Matrix(const vector<Vector3f> &points) {
    return {
        {points[0], 0},
        {points[1], 0},
        {points[2], 0},
        {points[3], 0},
    };
}

vector<Vector3f> matrix2Points(const Matrix4f &matrix) {
    return {
        matrix.getCol(0).xyz(),
        matrix.getCol(1).xyz(),
        matrix.getCol(2).xyz(),
        matrix.getCol(3).xyz(),
    };
}

Curve evalBezier(const vector<Vector3f> &P, unsigned steps,
                 const optional<Vector3f> &binormal) {
    // Check
    if (P.size() < 4 || P.size() % 3 != 1) {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit(0);
    }

    // TODO:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.

    cerr << "\t>>> evalBezier has been called with the following input:"
         << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): " << endl;
    for (unsigned i = 0; i < P.size(); ++i) {
        cerr << "\t>>> " << P[i] << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;

    Curve curve;
    curve.reserve((P.size() - 1) / 3 * (steps + 1));

    for (unsigned i = 0; i < P.size() - 1; i += 3) {
        auto controlPoints =
            vector<Vector3f>(P.cbegin() + i, P.cbegin() + i + 4);
        auto gb = points2Matrix(controlPoints) * bezierBasis;

        for (unsigned step = 0; step <= steps; step++) {
            auto t = static_cast<float>(step) / steps;

            Vector4f powerBasis{1, t, (float)pow(t, 2), (float)pow(t, 3)};
            Vector4f dPowerBasis{0, 1, 2 * t, 3 * (float)pow(t, 2)};

            CurvePoint p;

            p.V = (gb * powerBasis).xyz();
            p.T = (gb * dPowerBasis).xyz().normalized();

            auto prev_B = curve.empty() ? binormal.value_or(orthogonal(p.T))
                                        : curve.back().B;

            p.N = Vector3f::cross(prev_B, p.T).normalized();
            p.B = Vector3f::cross(p.T, p.N).normalized();

            curve.push_back(p);
        }
    }

    return curve;
}

Curve evalBspline(const vector<Vector3f> &P, unsigned steps) {
    // Check
    if (P.size() < 4) {
        cerr << "evalBspline must be called with 4 or more control points."
             << endl;
        exit(0);
    }

    // TODO:
    // It is suggested that you implement this function by changing
    // basis from B-spline to Bezier.  That way, you can just call
    // your evalBezier function.

    cerr << "\t>>> evalBspline has been called with the following input:"
         << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): " << endl;
    for (unsigned i = 0; i < P.size(); ++i) {
        cerr << "\t>>> " << P[i] << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;

    auto changeOfBasis = bsplineBasis * bezierBasis.inverse();

    Curve curve;
    curve.reserve((P.size() - 3) * (steps + 1));

    for (unsigned i = 0; i < P.size() - 3; i++) {
        auto controlPoints =
            vector<Vector3f>(P.cbegin() + i, P.cbegin() + i + 4);

        auto geometry = points2Matrix(controlPoints) * changeOfBasis;
        auto segment =
            evalBezier(matrix2Points(geometry), steps,
                       curve.empty() ? nullopt : make_optional(curve.back().B));

        curve.insert(curve.end(), segment.begin(), segment.end());
    }

    return curve;
}

Curve evalCircle(float radius, unsigned steps) {
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).

    // Preallocate a curve with steps+1 CurvePoints
    Curve R(steps + 1);

    // Fill it in counterclockwise
    for (unsigned i = 0; i <= steps; ++i) {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * static_cast<float>(i) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f(cos(t), sin(t), 0);

        // Tangent vector is first derivative
        R[i].T = Vector3f(-sin(t), cos(t), 0);

        // Normal vector is second derivative
        R[i].N = Vector3f(-cos(t), -sin(t), 0);

        // Finally, binormal is facing up.
        R[i].B = Vector3f(0, 0, 1);
    }

    return R;
}

void drawCurve(const Curve &curve, float framesize) {
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // Setup for line drawing
    glDisable(GL_LIGHTING);
    glColor4f(1, 1, 1, 1);
    glLineWidth(1);

    // Draw curve
    glBegin(GL_LINE_STRIP);
    for (unsigned i = 0; i < curve.size(); ++i) {
        glVertex(curve[i].V);
    }
    glEnd();

    glLineWidth(1);

    // Draw coordinate frames if framesize nonzero
    if (framesize != 0.0f) {
        Matrix4f M;

        for (unsigned i = 0; i < curve.size(); ++i) {
            M.setCol(0, Vector4f(curve[i].N, 0));
            M.setCol(1, Vector4f(curve[i].B, 0));
            M.setCol(2, Vector4f(curve[i].T, 0));
            M.setCol(3, Vector4f(curve[i].V, 1));

            glPushMatrix();
            glMultMatrixf(M);
            glScaled(framesize, framesize, framesize);
            glBegin(GL_LINES);
            glColor3f(1, 0, 0);
            glVertex3d(0, 0, 0);
            glVertex3d(1, 0, 0);
            glColor3f(0, 1, 0);
            glVertex3d(0, 0, 0);
            glVertex3d(0, 1, 0);
            glColor3f(0, 0, 1);
            glVertex3d(0, 0, 0);
            glVertex3d(0, 0, 1);
            glEnd();
            glPopMatrix();
        }
    }

    // Pop state
    glPopAttrib();
}
