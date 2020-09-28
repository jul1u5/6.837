#include "curve.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>

#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;

namespace {
// Approximately equal to.  We don't want to use == because of
// precision issues with floating point.
inline bool approx(const Vector3f &lhs, const Vector3f &rhs) {
    const float eps = 1e-8f;
    return (lhs - rhs).absSquared() < eps;
}
} // namespace

const Vector3f &orthogonal(const Vector3f &v) {
    const array<Vector3f, 3> &candidates{Vector3f(0, v.z(), -v.y()),
                                         Vector3f(-v.z(), 0, v.x()),
                                         Vector3f(-v.y(), v.x(), 0)};

    return *max_element(candidates.begin(), candidates.end(),
                        [](const Vector3f &x, const Vector3f &y) {
                            return x.absSquared() < y.absSquared();
                        });
}

Curve evalBezier(const vector<Vector3f> &P, unsigned steps) {
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

    auto q = [&P](float t) {
        return pow(1 - t, 3) * P[0] +           //
               3 * t * pow(1 - t, 2) * P[1] +   //
               3 * pow(t, 2) * (1 - t) * P[2] + //
               pow(t, 3) * P[3];
    };

    auto dq = [&P](float t) {
        return -3 * pow(1 - t, 2) * P[0] +                    //
               (3 * pow(1 - t, 2) - 6 * t * (1 - t)) * P[1] + //
               (6 * t * (1 - t) - 3 * pow(t, 2)) * P[2] +     //
               3 * pow(t, 2) * P[3];
    };

    Curve curve;
    curve.reserve(steps + 1);

    for (unsigned i = 0; i <= steps; i++) {
        auto t = static_cast<float>(i) / steps;

        CurvePoint p;

        p.V = q(t);
        p.T = dq(t).normalized();

        const auto &prev_B = i == 0 ? orthogonal(p.T) : curve[i - 1].B;
        prev_B.print();
        if (i > 0) {
            curve[i - 1].B.print();
        }

        p.N = Vector3f::cross(prev_B, p.T).normalized();
        p.B = Vector3f::cross(p.T, p.N).normalized();
        p.T.print();
        p.N.print();
        Vector3f::cross(prev_B, p.T).print();

        curve.emplace_back(p);
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;

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

    cerr << "\t>>> evalBSpline has been called with the following input:"
         << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): " << endl;
    for (unsigned i = 0; i < P.size(); ++i) {
        cerr << "\t>>> " << P[i] << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    cerr << "\t>>> Returning empty curve." << endl;

    // Return an empty curve right now.
    return Curve();
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
