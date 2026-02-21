//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: VBSController.cpp
//
// Code generated for Simulink model 'VBSController'.
//
// Model version                  : 1.208
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu Feb 19 19:15:47 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "VBSController.h"
#include <stdint.h>
#include <stdbool.h>
#include <array>
#include <cmath>
#include <cstring>
#include "cmath"
#include "limits"

// Named constants for MATLAB Function: '<S23>/optimizer'
const double VBSController_RYscale{ 0.0001 };

const int32_t VBSController_degrees{ 5 };

extern double rt_hypotd_snf(double u0, double u1);
static int32_t div_nde_s32_floor(int32_t numerator, int32_t denominator);
static void rate_scheduler(VBSController::RT_MODEL_VBSController_T *const
    VBSController_M);
extern "C"
{
    double rtNaN { -std::numeric_limits<double>::quiet_NaN() };

    double rtInf { std::numeric_limits<double>::infinity() };

    double rtMinusInf { -std::numeric_limits<double>::infinity() };

    float rtNaNF { -std::numeric_limits<float>::quiet_NaN() };

    float rtInfF { std::numeric_limits<float>::infinity() };

    float rtMinusInfF { -std::numeric_limits<float>::infinity() };
}

extern "C"
{
    // Return rtInf needed by the generated code.
    static double rtGetInf(void)
    {
        return rtInf;
    }

    // Get rtInfF needed by the generated code.
    static float rtGetInfF(void)
    {
        return rtInfF;
    }

    // Return rtMinusInf needed by the generated code.
    static double rtGetMinusInf(void)
    {
        return rtMinusInf;
    }

    // Return rtMinusInfF needed by the generated code.
    static float rtGetMinusInfF(void)
    {
        return rtMinusInfF;
    }
}

extern "C"
{
    // Return rtNaN needed by the generated code.
    static double rtGetNaN(void)
    {
        return rtNaN;
    }

    // Return rtNaNF needed by the generated code.
    static float rtGetNaNF(void)
    {
        return rtNaNF;
    }
}

extern "C"
{
    // Test if value is infinite
    static bool rtIsInf(double value)
    {
        return std::isinf(value);
    }

    // Test if single-precision value is infinite
    static bool rtIsInfF(float value)
    {
        return std::isinf(value);
    }

    // Test if value is not a number
    static bool rtIsNaN(double value)
    {
        return std::isnan(value);
    }

    // Test if single-precision value is not a number
    static bool rtIsNaNF(float value)
    {
        return std::isnan(value);
    }
}

static int32_t div_nde_s32_floor(int32_t numerator, int32_t denominator)
{
    return (((static_cast<int32_t>((static_cast<int32_t>(numerator < 0)) != (
                static_cast<int32_t>(denominator < 0)))) && (static_cast<int32_t>
              ((numerator % denominator) != 0))) ? -1 : 0) + (numerator /
        denominator);
}

//
//         This function updates active task flag for each subrate.
//         The function is called at model base rate, hence the
//         generated code self-manages all its subrates.
//
static void rate_scheduler(VBSController::RT_MODEL_VBSController_T *const
    VBSController_M)
{
    // Compute which subrates run during the next base time step.  Subrates
    //  are an integer multiple of the base rate counter.  Therefore, the subtask
    //  counter is reset when it reaches its limit (zero means run).

    (VBSController_M->Timing.TaskCounters.TID[1])++;
    if ((VBSController_M->Timing.TaskCounters.TID[1]) > 9) {// Sample time: [0.1s, 0.0s] 
        VBSController_M->Timing.TaskCounters.TID[1] = 0;
    }
}

// Function for MATLAB Function: '<S23>/optimizer'
double VBSController::VBSController_norm(const double x[5])
{
    double scale;
    double y;
    int32_t k;
    y = 0.0;
    scale = 3.3121686421112381E-170;
    for (k = 0; k < 5; k++) {
        double absxk;
        absxk = std::abs(x[k]);
        if (absxk > scale) {
            double t;
            t = scale / absxk;
            y = ((y * t) * t) + 1.0;
            scale = absxk;
        } else {
            double t;
            t = absxk / scale;
            y += t * t;
        }
    }

    y = scale * std::sqrt(y);
    if (std::isnan(y)) {
        k = 0;
        int32_t exitg1;
        do {
            exitg1 = 0;
            if (k < 5) {
                if (std::isnan(x[k])) {
                    exitg1 = 1;
                } else {
                    k++;
                }
            } else {
                y = (rtInf);
                exitg1 = 1;
            }
        } while (exitg1 == 0);
    }

    return y;
}

// Function for MATLAB Function: '<S23>/optimizer'
double VBSController::VBSController_maximum(const double x[5])
{
    double ex;
    int32_t idx;
    int32_t k;
    if (!(static_cast<int32_t>(std::isnan(x[0])))) {
        idx = 1;
    } else {
        bool exitg1;
        idx = 0;
        k = 2;
        exitg1 = false;
        while ((!exitg1) && (k < 6)) {
            if (!(static_cast<int32_t>(std::isnan(x[k - 1])))) {
                idx = k;
                exitg1 = true;
            } else {
                k++;
            }
        }
    }

    if (idx == 0) {
        ex = x[0];
    } else {
        ex = x[idx - 1];
        for (k = idx + 1; k < 6; k++) {
            double x_0;
            x_0 = x[k - 1];
            if (ex < x_0) {
                ex = x_0;
            }
        }
    }

    return ex;
}

// Function for MATLAB Function: '<S23>/optimizer'
double VBSController::VBSController_xnrm2(int32_t n, const double x[25], int32_t
    ix0)
{
    double y;
    y = 0.0;
    if (n < 1) {
    } else if (n == 1) {
        y = std::abs(x[ix0 - 1]);
    } else {
        double scale;
        int32_t k;
        int32_t kend;
        scale = 3.3121686421112381E-170;
        kend = (ix0 + n) - 1;
        for (k = ix0; k <= kend; k++) {
            double absxk;
            absxk = std::abs(x[k - 1]);
            if (absxk > scale) {
                double t;
                t = scale / absxk;
                y = ((y * t) * t) + 1.0;
                scale = absxk;
            } else {
                double t;
                t = absxk / scale;
                y += t * t;
            }
        }

        y = scale * std::sqrt(y);
        if (std::isnan(y)) {
            k = ix0;
            int32_t exitg1;
            do {
                exitg1 = 0;
                if (k <= kend) {
                    if (std::isnan(x[k - 1])) {
                        exitg1 = 1;
                    } else {
                        k++;
                    }
                } else {
                    y = (rtInf);
                    exitg1 = 1;
                }
            } while (exitg1 == 0);
        }
    }

    return y;
}

double rt_hypotd_snf(double u0, double u1)
{
    double a;
    double b;
    double y;
    a = std::abs(u0);
    b = std::abs(u1);
    if (a < b) {
        a /= b;
        y = b * std::sqrt((a * a) + 1.0);
    } else if (a > b) {
        b /= a;
        y = a * std::sqrt((b * b) + 1.0);
    } else if (std::isnan(b)) {
        y = (rtNaN);
    } else {
        y = a * 1.4142135623730951;
    }

    return y;
}

// Function for MATLAB Function: '<S23>/optimizer'
void VBSController::VBSController_xgemv(int32_t b_m, int32_t n, const double
    b_A[25], int32_t ia0, const double x[25], int32_t ix0, double y[5])
{
    if ((static_cast<int32_t>(b_m == 0)) || (static_cast<int32_t>(n == 0))) {
    } else {
        int32_t b;
        if ((n - 1) >= 0) {
            std::memset(&y[0], 0, (static_cast<uint32_t>(n)) * (sizeof(double)));
        }

        b = ia0 + (5 * (n - 1));
        for (int32_t b_iy{ia0}; b_iy <= b; b_iy += 5) {
            double c;
            int32_t d;
            int32_t ia;
            c = 0.0;
            d = b_iy + b_m;
            for (ia = b_iy; ia < d; ia++) {
                c += b_A[ia - 1] * x[((ix0 + ia) - b_iy) - 1];
            }

            ia = div_nde_s32_floor(b_iy - ia0, 5);
            y[ia] += c;
        }
    }
}

// Function for MATLAB Function: '<S23>/optimizer'
void VBSController::VBSController_xgerc(int32_t b_m, int32_t n, double alpha1,
    int32_t ix0, const double y[5], double b_A[25], int32_t ia0)
{
    if (alpha1 == 0.0) {
    } else {
        int32_t jA;
        jA = ia0;
        for (int32_t j{0}; j < n; j++) {
            double temp;
            temp = y[j];
            if (temp != 0.0) {
                int32_t b;
                temp *= alpha1;
                b = b_m + jA;
                for (int32_t ijA{jA}; ijA < b; ijA++) {
                    b_A[ijA - 1] += b_A[((ix0 + ijA) - jA) - 1] * temp;
                }
            }

            jA += 5;
        }
    }
}

// Function for MATLAB Function: '<S23>/optimizer'
double VBSController::VBSController_KWIKfactor(const double b_Ac[330], const
    int32_t iC[66], int32_t nA, const double b_Linv[25], double RLinv[25],
    double D[25], double b_H[25], int32_t n)
{
    std::array<double, 25> R;
    std::array<double, 25> TL;
    std::array<double, 25> b_A;
    std::array<double, 5> tau;
    std::array<double, 5> work;
    double Status;
    double atmp;
    double b_A_0;
    double xnorm;
    int32_t b_coltop;
    int32_t b_lastv;
    int32_t coltop;
    int32_t exitg1;
    int32_t ii;
    int32_t k_i;
    int32_t knt;
    bool exitg2;
    Status = 1.0;
    std::memset(&RLinv[0], 0, 25U * (sizeof(double)));
    for (k_i = 0; k_i < nA; k_i++) {
        ii = iC[k_i];
        for (b_coltop = 0; b_coltop < 5; b_coltop++) {
            RLinv[b_coltop + (5 * k_i)] = 0.0;
        }

        for (b_coltop = 0; b_coltop < 5; b_coltop++) {
            atmp = b_Ac[(ii + (66 * b_coltop)) - 1];
            for (knt = 0; knt < 5; knt++) {
                coltop = knt + (5 * k_i);
                RLinv[coltop] += b_Linv[knt + (5 * b_coltop)] * atmp;
            }
        }
    }

    std::memcpy(&b_A[0], &RLinv[0], 25U * (sizeof(double)));
    for (k_i = 0; k_i < 5; k_i++) {
        tau[k_i] = 0.0;
        work[k_i] = 0.0;
    }

    for (k_i = 0; k_i < 5; k_i++) {
        ii = (k_i * 5) + k_i;
        if ((k_i + 1) < 5) {
            atmp = b_A[ii];
            b_lastv = ii + 2;
            tau[k_i] = 0.0;
            xnorm = VBSController_xnrm2(4 - k_i, &b_A[0], ii + 2);
            if (xnorm != 0.0) {
                b_A_0 = b_A[ii];
                xnorm = rt_hypotd_snf(b_A_0, xnorm);
                if (b_A_0 >= 0.0) {
                    xnorm = -xnorm;
                }

                if (std::abs(xnorm) < 1.0020841800044864E-292) {
                    knt = 0;
                    coltop = (ii - k_i) + 5;
                    do {
                        knt++;
                        for (b_coltop = b_lastv; b_coltop <= coltop; b_coltop++)
                        {
                            b_A[b_coltop - 1] *= 9.9792015476736E+291;
                        }

                        xnorm *= 9.9792015476736E+291;
                        atmp *= 9.9792015476736E+291;
                    } while ((static_cast<int32_t>(std::abs(xnorm) <
                                                   1.0020841800044864E-292)) &&
                                    (static_cast<int32_t>(knt < 20)));

                    xnorm = rt_hypotd_snf(atmp, VBSController_xnrm2(4 - k_i,
                                           &b_A[0], ii + 2));
                    if (atmp >= 0.0) {
                        xnorm = -xnorm;
                    }

                    tau[k_i] = (xnorm - atmp) / xnorm;
                    atmp = 1.0 / (atmp - xnorm);
                    for (b_coltop = b_lastv; b_coltop <= coltop; b_coltop++) {
                        b_A[b_coltop - 1] *= atmp;
                    }

                    for (b_lastv = 0; b_lastv < knt; b_lastv++) {
                        xnorm *= 1.0020841800044864E-292;
                    }

                    atmp = xnorm;
                } else {
                    tau[k_i] = (xnorm - b_A_0) / xnorm;
                    atmp = 1.0 / (b_A_0 - xnorm);
                    b_coltop = (ii - k_i) + 5;
                    for (knt = b_lastv; knt <= b_coltop; knt++) {
                        b_A[knt - 1] *= atmp;
                    }

                    atmp = xnorm;
                }
            }

            b_A[ii] = 1.0;
            if (tau[k_i] != 0.0) {
                b_lastv = 5 - k_i;
                knt = (ii - k_i) + 4;
                while ((static_cast<int32_t>(b_lastv > 0)) &&
                        (static_cast<int32_t>(b_A[knt] == 0.0))) {
                    b_lastv--;
                    knt--;
                }

                knt = 4 - k_i;
                exitg2 = false;
                while ((!exitg2) && (knt > 0)) {
                    b_coltop = (ii + ((knt - 1) * 5)) + 5;
                    coltop = b_coltop;
                    do {
                        exitg1 = 0;
                        if ((coltop + 1) <= (b_coltop + b_lastv)) {
                            if (b_A[coltop] != 0.0) {
                                exitg1 = 1;
                            } else {
                                coltop++;
                            }
                        } else {
                            knt--;
                            exitg1 = 2;
                        }
                    } while (exitg1 == 0);

                    if (exitg1 == 1) {
                        exitg2 = true;
                    }
                }
            } else {
                b_lastv = 0;
                knt = 0;
            }

            if (b_lastv > 0) {
                VBSController_xgemv(b_lastv, knt, &b_A[0], ii + 6, &b_A[0], ii +
                                    1, &work[0]);
                VBSController_xgerc(b_lastv, knt, -tau[k_i], ii + 1, &work[0],
                                    &b_A[0], ii + 6);
            }

            b_A[ii] = atmp;
        } else {
            tau[4] = 0.0;
        }
    }

    for (k_i = 0; k_i < 5; k_i++) {
        for (ii = 0; ii <= k_i; ii++) {
            b_lastv = ii + (5 * k_i);
            R[b_lastv] = b_A[b_lastv];
        }

        for (ii = k_i + 2; ii < 6; ii++) {
            R[(ii + (5 * k_i)) - 1] = 0.0;
        }

        work[k_i] = 0.0;
    }

    for (k_i = 4; k_i >= 0; k_i--) {
        b_lastv = k_i + (k_i * 5);
        if ((k_i + 1) < 5) {
            b_A[b_lastv] = 1.0;
            if (tau[k_i] != 0.0) {
                knt = 5 - k_i;
                b_coltop = (b_lastv - k_i) + 4;
                while ((static_cast<int32_t>(knt > 0)) && (static_cast<int32_t>
                        (b_A[b_coltop] == 0.0))) {
                    knt--;
                    b_coltop--;
                }

                b_coltop = 4 - k_i;
                exitg2 = false;
                while ((!exitg2) && (b_coltop > 0)) {
                    coltop = (b_lastv + ((b_coltop - 1) * 5)) + 5;
                    ii = coltop;
                    do {
                        exitg1 = 0;
                        if ((ii + 1) <= (coltop + knt)) {
                            if (b_A[ii] != 0.0) {
                                exitg1 = 1;
                            } else {
                                ii++;
                            }
                        } else {
                            b_coltop--;
                            exitg1 = 2;
                        }
                    } while (exitg1 == 0);

                    if (exitg1 == 1) {
                        exitg2 = true;
                    }
                }
            } else {
                knt = 0;
                b_coltop = 0;
            }

            if (knt > 0) {
                VBSController_xgemv(knt, b_coltop, &b_A[0], b_lastv + 6, &b_A[0],
                                    b_lastv + 1, &work[0]);
                VBSController_xgerc(knt, b_coltop, -tau[k_i], b_lastv + 1,
                                    &work[0], &b_A[0], b_lastv + 6);
            }

            b_coltop = (b_lastv - k_i) + 5;
            for (knt = b_lastv + 2; knt <= b_coltop; knt++) {
                b_A[knt - 1] *= -tau[k_i];
            }
        }

        b_A[b_lastv] = 1.0 - tau[k_i];
        for (knt = 0; knt < k_i; knt++) {
            b_A[(b_lastv - knt) - 1] = 0.0;
        }
    }

    k_i = 0;
    do {
        exitg1 = 0;
        if (k_i <= (nA - 1)) {
            if (std::abs(R[k_i + (5 * k_i)]) < 1.0E-12) {
                Status = -2.0;
                exitg1 = 1;
            } else {
                k_i++;
            }
        } else {
            for (k_i = 0; k_i < n; k_i++) {
                for (ii = 0; ii < n; ii++) {
                    atmp = 0.0;
                    for (b_coltop = 0; b_coltop < 5; b_coltop++) {
                        atmp += b_Linv[b_coltop + (5 * k_i)] * b_A[b_coltop + (5
                            * ii)];
                    }

                    TL[k_i + (5 * ii)] = atmp;
                }
            }

            std::memset(&RLinv[0], 0, 25U * (sizeof(double)));
            for (k_i = nA; k_i >= 1; k_i--) {
                b_coltop = 5 * (k_i - 1);
                knt = (k_i + b_coltop) - 1;
                RLinv[knt] = 1.0;
                for (ii = k_i; ii <= nA; ii++) {
                    coltop = (k_i + (5 * (ii - 1))) - 1;
                    RLinv[coltop] /= R[knt];
                }

                if (k_i > 1) {
                    for (ii = 0; ii <= (k_i - 2); ii++) {
                        for (b_lastv = k_i; b_lastv <= nA; b_lastv++) {
                            knt = 5 * (b_lastv - 1);
                            coltop = ii + knt;
                            RLinv[coltop] -= R[ii + b_coltop] * RLinv[(k_i + knt)
                                - 1];
                        }
                    }
                }
            }

            for (k_i = 0; k_i < n; k_i++) {
                for (ii = k_i + 1; ii <= n; ii++) {
                    b_coltop = k_i + (5 * (ii - 1));
                    b_H[b_coltop] = 0.0;
                    for (b_lastv = nA + 1; b_lastv <= n; b_lastv++) {
                        knt = 5 * (b_lastv - 1);
                        b_H[b_coltop] -= TL[k_i + knt] * TL[(ii + knt) - 1];
                    }

                    b_H[(ii + (5 * k_i)) - 1] = b_H[b_coltop];
                }
            }

            for (k_i = 0; k_i < nA; k_i++) {
                for (ii = 0; ii < n; ii++) {
                    b_coltop = ii + (5 * k_i);
                    D[b_coltop] = 0.0;
                    for (b_lastv = k_i + 1; b_lastv <= nA; b_lastv++) {
                        knt = 5 * (b_lastv - 1);
                        D[b_coltop] += TL[ii + knt] * RLinv[k_i + knt];
                    }
                }
            }

            exitg1 = 1;
        }
    } while (exitg1 == 0);

    return Status;
}

// Function for MATLAB Function: '<S23>/optimizer'
void VBSController::VBSController_DropConstraint(int32_t kDrop, bool iA[66],
    int32_t *nA, int32_t iC[66])
{
    if (kDrop > 0) {
        iA[iC[kDrop - 1] - 1] = false;
        if (kDrop < (*nA)) {
            for (int32_t i{kDrop}; i < (*nA); i++) {
                iC[i - 1] = iC[i];
            }
        }

        iC[(*nA) - 1] = 0;
        (*nA)--;
    }
}

// Function for MATLAB Function: '<S23>/optimizer'
void VBSController::VBSController_qpkwik(const double b_Linv[25], const double
    b_Hinv[25], const double f[5], const double b_Ac[330], const double b[66],
    bool iA[66], int32_t maxiter, double FeasTol, double x[5], double lambda[66],
    int32_t *status)
{
    std::array<double, 25> D;
    std::array<double, 10> Opt;
    std::array<double, 25> RLinv;
    std::array<double, 10> Rhs;
    std::array<double, 25> U;
    std::array<double, 25> b_H;
    std::array<double, 66> cTol;
    std::array<int32_t, 66> iC;
    std::array<double, 5> r;
    std::array<double, 5> z;
    double Xnorm0;
    double cMin;
    double cVal;
    double rMin;
    double t;
    int32_t U_tmp;
    int32_t b_exponent;
    int32_t exitg1;
    int32_t exitg3;
    int32_t exponent;
    int32_t i;
    int32_t iC_0;
    int32_t iSave;
    int32_t nA;
    int32_t tmp;
    bool ColdReset;
    bool DualFeasible;
    bool cTolComputed;
    bool exitg2;
    bool exitg4;
    bool guard1;
    bool guard2;
    for (i = 0; i < 5; i++) {
        x[i] = 0.0;
    }

    std::memset(&lambda[0], 0, 66U * (sizeof(double)));
    *status = 1;
    for (i = 0; i < 5; i++) {
        r[i] = 0.0;
    }

    rMin = 0.0;
    cTolComputed = false;
    for (i = 0; i < 66; i++) {
        cTol[i] = 1.0;
        iC[i] = 0;
    }

    nA = 0;
    for (tmp = 0; tmp < 66; tmp++) {
        if (iA[tmp]) {
            nA++;
            iC[nA - 1] = tmp + 1;
        }
    }

    guard1 = false;
    if (nA > 0) {
        std::memset(&Opt[0], 0, 10U * (sizeof(double)));
        for (i = 0; i < 5; i++) {
            Rhs[i] = f[i];
            Rhs[i + 5] = 0.0;
        }

        DualFeasible = false;
        tmp = static_cast<int32_t>(std::round(0.3 * (static_cast<double>(nA))));
        ColdReset = false;
        do {
            exitg3 = 0;
            if ((static_cast<int32_t>((!(static_cast<int32_t>(DualFeasible))) &&
                  (static_cast<int32_t>(nA > 0)))) && (static_cast<int32_t>
                    ((*status) <= maxiter))) {
                Xnorm0 = VBSController_KWIKfactor(b_Ac, &iC[0], nA, b_Linv,
                    &RLinv[0], &D[0], &b_H[0], VBSController_degrees);
                if (Xnorm0 < 0.0) {
                    if (ColdReset) {
                        *status = -2;
                        exitg3 = 2;
                    } else {
                        nA = 0;
                        std::memset(&iA[0], 0, 66U * (sizeof(bool)));
                        std::memset(&iC[0], 0, 66U * (sizeof(int32_t)));
                        ColdReset = true;
                    }
                } else {
                    for (i = 0; i < nA; i++) {
                        Rhs[i + 5] = b[iC[i] - 1];
                        for (iSave = i + 1; iSave <= nA; iSave++) {
                            U_tmp = (iSave + (5 * i)) - 1;
                            U[U_tmp] = 0.0;
                            for (iC_0 = 0; iC_0 < nA; iC_0++) {
                                U[U_tmp] += RLinv[(iSave + (5 * iC_0)) - 1] *
                                    RLinv[i + (5 * iC_0)];
                            }

                            U[i + (5 * (iSave - 1))] = U[U_tmp];
                        }
                    }

                    for (i = 0; i < 5; i++) {
                        Xnorm0 = 0.0;
                        for (iC_0 = 0; iC_0 < 5; iC_0++) {
                            Xnorm0 += b_H[i + (5 * iC_0)] * Rhs[iC_0];
                        }

                        Opt[i] = Xnorm0;
                        for (iSave = 0; iSave < nA; iSave++) {
                            Opt[i] += D[i + (5 * iSave)] * Rhs[iSave + 5];
                        }
                    }

                    for (i = 0; i < nA; i++) {
                        Xnorm0 = 0.0;
                        for (iC_0 = 0; iC_0 < 5; iC_0++) {
                            Xnorm0 += D[iC_0 + (5 * i)] * Rhs[iC_0];
                        }

                        Opt[i + 5] = Xnorm0;
                        for (iSave = 0; iSave < nA; iSave++) {
                            Opt[i + 5] += U[i + (5 * iSave)] * Rhs[iSave + 5];
                        }
                    }

                    Xnorm0 = -1.0E-12;
                    i = -1;
                    for (iSave = 0; iSave < nA; iSave++) {
                        cMin = Opt[iSave + 5];
                        lambda[iC[iSave] - 1] = cMin;
                        if ((static_cast<int32_t>(cMin < Xnorm0)) && (
                                static_cast<int32_t>((iSave + 1) <= nA))) {
                            i = iSave;
                            Xnorm0 = cMin;
                        }
                    }

                    if ((i + 1) <= 0) {
                        DualFeasible = true;
                        for (i = 0; i < 5; i++) {
                            x[i] = Opt[i];
                        }
                    } else {
                        (*status)++;
                        if (tmp <= 5) {
                            iC_0 = 5;
                        } else {
                            iC_0 = tmp;
                        }

                        if ((*status) > iC_0) {
                            nA = 0;
                            std::memset(&iA[0], 0, 66U * (sizeof(bool)));
                            std::memset(&iC[0], 0, 66U * (sizeof(int32_t)));
                            ColdReset = true;
                        } else {
                            lambda[iC[i] - 1] = 0.0;
                            VBSController_DropConstraint(i + 1, iA, &nA, &iC[0]);
                        }
                    }
                }
            } else {
                if (nA <= 0) {
                    std::memset(&lambda[0], 0, 66U * (sizeof(double)));
                    for (tmp = 0; tmp < 5; tmp++) {
                        Xnorm0 = 0.0;
                        for (iC_0 = 0; iC_0 < 5; iC_0++) {
                            Xnorm0 += (-b_Hinv[tmp + (5 * iC_0)]) * f[iC_0];
                        }

                        x[tmp] = Xnorm0;
                    }
                }

                exitg3 = 1;
            }
        } while (exitg3 == 0);

        if (exitg3 == 1) {
            guard1 = true;
        }
    } else {
        for (tmp = 0; tmp < 5; tmp++) {
            Xnorm0 = 0.0;
            for (iC_0 = 0; iC_0 < 5; iC_0++) {
                Xnorm0 += (-b_Hinv[tmp + (5 * iC_0)]) * f[iC_0];
            }

            x[tmp] = Xnorm0;
        }

        guard1 = true;
    }

    if (guard1) {
        Xnorm0 = VBSController_norm(x);
        exitg2 = false;
        while ((!exitg2) && ((*status) <= maxiter)) {
            cMin = -FeasTol;
            tmp = -1;
            for (i = 0; i < 66; i++) {
                if (!(static_cast<int32_t>(cTolComputed))) {
                    for (iSave = 0; iSave < 5; iSave++) {
                        z[iSave] = std::abs(b_Ac[i + (66 * iSave)] * x[iSave]);
                    }

                    cTol[i] = std::fmax(cTol[i], VBSController_maximum(&z[0]));
                }

                if (!(static_cast<int32_t>(iA[i]))) {
                    cVal = 0.0;
                    for (iC_0 = 0; iC_0 < 5; iC_0++) {
                        cVal += b_Ac[i + (66 * iC_0)] * x[iC_0];
                    }

                    cVal = (cVal - b[i]) / cTol[i];
                    if (cVal < cMin) {
                        cMin = cVal;
                        tmp = i;
                    }
                }
            }

            cTolComputed = true;
            if ((tmp + 1) <= 0) {
                exitg2 = true;
            } else if ((*status) == maxiter) {
                *status = 0;
                exitg2 = true;
            } else {
                do {
                    exitg1 = 0;
                    if ((static_cast<int32_t>((tmp + 1) > 0)) &&
                            (static_cast<int32_t>((*status) <= maxiter))) {
                        guard2 = false;
                        if (nA == 0) {
                            for (iC_0 = 0; iC_0 < 5; iC_0++) {
                                z[iC_0] = 0.0;
                            }

                            for (iC_0 = 0; iC_0 < 5; iC_0++) {
                                cMin = b_Ac[tmp + (66 * iC_0)];
                                for (i = 0; i < 5; i++) {
                                    z[i] += b_Hinv[i + (5 * iC_0)] * cMin;
                                }
                            }

                            guard2 = true;
                        } else {
                            cMin = VBSController_KWIKfactor(b_Ac, &iC[0], nA,
                                b_Linv, &RLinv[0], &D[0], &b_H[0],
                                VBSController_degrees);
                            if (cMin <= 0.0) {
                                *status = -2;
                                exitg1 = 1;
                            } else {
                                for (iC_0 = 0; iC_0 < 25; iC_0++) {
                                    U[iC_0] = -b_H[iC_0];
                                }

                                for (iC_0 = 0; iC_0 < 5; iC_0++) {
                                    z[iC_0] = 0.0;
                                }

                                for (iC_0 = 0; iC_0 < 5; iC_0++) {
                                    cMin = b_Ac[tmp + (66 * iC_0)];
                                    for (i = 0; i < 5; i++) {
                                        z[i] += U[i + (5 * iC_0)] * cMin;
                                    }
                                }

                                for (i = 0; i < nA; i++) {
                                    cVal = 0.0;
                                    for (iC_0 = 0; iC_0 < 5; iC_0++) {
                                        cVal += b_Ac[tmp + (66 * iC_0)] * D[iC_0
                                            + (5 * i)];
                                    }

                                    r[i] = cVal;
                                }

                                guard2 = true;
                            }
                        }

                        if (guard2) {
                            i = 0;
                            cMin = 0.0;
                            DualFeasible = true;
                            ColdReset = true;
                            if (nA > 0) {
                                iSave = 0;
                                exitg4 = false;
                                while ((!exitg4) && (iSave <= (nA - 1))) {
                                    if (r[iSave] >= 1.0E-12) {
                                        ColdReset = false;
                                        exitg4 = true;
                                    } else {
                                        iSave++;
                                    }
                                }
                            }

                            if ((static_cast<int32_t>(nA != 0)) && (!(
                                    static_cast<int32_t>(ColdReset)))) {
                                for (iSave = 0; iSave < nA; iSave++) {
                                    cVal = r[iSave];
                                    if (cVal > 1.0E-12) {
                                        cVal = lambda[iC[iSave] - 1] / cVal;
                                        if ((static_cast<int32_t>(i == 0)) || (
                                                static_cast<int32_t>(cVal < rMin)))
                                        {
                                            rMin = cVal;
                                            i = iSave + 1;
                                        }
                                    }
                                }

                                if (i > 0) {
                                    cMin = rMin;
                                    DualFeasible = false;
                                }
                            }

                            t = 0.0;
                            for (iC_0 = 0; iC_0 < 5; iC_0++) {
                                t += z[iC_0] * b_Ac[tmp + (66 * iC_0)];
                            }

                            if (t <= 0.0) {
                                cVal = 0.0;
                                ColdReset = true;
                            } else {
                                cVal = 0.0;
                                for (iC_0 = 0; iC_0 < 5; iC_0++) {
                                    cVal += b_Ac[tmp + (66 * iC_0)] * x[iC_0];
                                }

                                cVal = (b[tmp] - cVal) / t;
                                ColdReset = false;
                            }

                            if ((static_cast<int32_t>(DualFeasible)) && (
                                    static_cast<int32_t>(ColdReset))) {
                                *status = -1;
                                exitg1 = 1;
                            } else {
                                if (ColdReset) {
                                    t = cMin;
                                } else if (DualFeasible) {
                                    t = cVal;
                                } else if (cMin < cVal) {
                                    t = cMin;
                                } else {
                                    t = cVal;
                                }

                                for (iSave = 0; iSave < nA; iSave++) {
                                    iC_0 = iC[iSave];
                                    lambda[iC_0 - 1] -= t * r[iSave];
                                    if ((static_cast<int32_t>(iC_0 <= 66)) && (
                                            static_cast<int32_t>(lambda[iC_0 - 1]
                                          < 0.0))) {
                                        lambda[iC_0 - 1] = 0.0;
                                    }
                                }

                                lambda[tmp] += t;
                                std::frexp(1.0, &exponent);
                                if (std::abs(t - cMin) < 2.2204460492503131E-16)
                                {
                                    VBSController_DropConstraint(i, iA, &nA,
                                        &iC[0]);
                                }

                                if (!(static_cast<int32_t>(ColdReset))) {
                                    for (iC_0 = 0; iC_0 < 5; iC_0++) {
                                        x[iC_0] += t * z[iC_0];
                                    }

                                    std::frexp(1.0, &b_exponent);
                                    if (std::abs(t - cVal) <
                                            2.2204460492503131E-16) {
                                        if (nA == VBSController_degrees) {
                                            *status = -1;
                                            exitg1 = 1;
                                        } else {
                                            nA++;
                                            iC[nA - 1] = tmp + 1;
                                            i = nA - 1;
                                            exitg4 = false;
                                            while ((!exitg4) && ((i + 1) > 1)) {
                                                iC_0 = iC[i - 1];
                                                if (iC[i] > iC_0) {
                                                    exitg4 = true;
                                                } else {
                                                    iSave = iC[i];
                                                    iC[i] = iC_0;
                                                    iC[i - 1] = iSave;
                                                    i--;
                                                }
                                            }

                                            iA[tmp] = true;
                                            tmp = -1;
                                            (*status)++;
                                        }
                                    } else {
                                        (*status)++;
                                    }
                                } else {
                                    (*status)++;
                                }
                            }
                        }
                    } else {
                        cMin = VBSController_norm(x);
                        if (std::abs(cMin - Xnorm0) > 0.001) {
                            Xnorm0 = cMin;
                            for (tmp = 0; tmp < 66; tmp++) {
                                cTol[tmp] = std::fmax(std::abs(b[tmp]), 1.0);
                            }

                            cTolComputed = false;
                        }

                        exitg1 = 2;
                    }
                } while (exitg1 == 0);

                if (exitg1 == 1) {
                    exitg2 = true;
                }
            }
        }
    }
}

// Model step function
void VBSController::step()
{
    std::array<double, 66> a__1;
    std::array<double, 66> b_Mlim;
    std::array<double, 5> f;
    std::array<double, 50> rseq;
    std::array<double, 5> zopt;
    double b_Kr;
    double rtb_last_mv;
    double rtb_xest_idx_0;
    double rtb_xest_idx_1;
    double xk;
    double xk_idx_0;
    double y_innov;
    int32_t f_tmp;
    int32_t i;
    static const std::array<double, 25> b_Linv{ { 0.044067444649480828,
            -0.058277554611098618, -0.035787182075276577, -0.025781350190724892,
            0.0, 0.0, 0.076037034617053384, -0.034119462652353827,
            -0.024632299238206007, 0.0, 0.0, 0.0, 0.085065039352462027,
            -0.02350824987871631, 0.0, 0.0, 0.0, 0.0, 0.089421152179763125, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.001 } };

    static const std::array<double, 25> b_Hinv{ { 0.0072836134679304257,
            -0.0025751590824480237, -0.0024381636290528746,
            -0.0023053980388045756, 0.0, -0.0025751590824480237,
            0.0075525185308008662, -0.0023233111876282764,
            -0.0022026485787170827, 0.0, -0.0024381636290528746,
            -0.0023233111876282764, 0.0077886987323960788, -0.002102134789884589,
            0.0, -0.0023053980388045756, -0.0022026485787170827,
            -0.002102134789884589, 0.0079961424571563557, 0.0, 0.0, 0.0, 0.0,
            0.0, 1.0E-6 } };

    static const std::array<double, 330> b_Ac{ { -0.0025138114032864606,
            -0.010055241226294552, -0.022624282888753795, -0.040220929810402331,
            -0.062845175410986914, -0.0904970131102629, -0.12317643632799426,
            -0.16088343848395356, -0.20361801299792204, -0.25138015328968949,
            -0.30416985277905434, -0.36198710488582359, -0.42483190302981294,
            -0.49270424063084661, -0.56560411110875752, -0.64353150788338709,
            -0.72648642437458544, -0.81446885400221136, -0.90747879018613209,
            -1.0055162263462236, -1.1085811559023704, -1.2166735722744657,
            -1.3297934688824111, -1.4479408391461173, -1.571115676485503,
            -1.6993179743204958, -1.8325477260710321, -1.9708049251570567,
            -2.114089564998523, -2.2624016390153932, -2.4157411406276377,
            -2.574108063255236, -2.737502400318176, -2.9059241452364541,
            -3.0793732914300755, -3.2578498323190539, -3.4413537613234113,
            -3.6298850718631792, -3.8234437573583966, -4.0220298112291122,
            -4.2256432268953823, -4.434283997777273, -4.6479521172948575,
            -4.8666475788682186, -5.0903703759174483, -5.3191205018626455,
            -5.55289795012392, -5.7917027141213868, -6.0355347872751732,
            -6.2843941630054134, -1.0, -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0,
            -1.0, -0.0, -0.0, -0.0, 1.0, 0.0, 0.0, 0.0, -0.0,
            -0.0025138114032864606, -0.010055241226294552, -0.022624282888753795,
            -0.040220929810402331, -0.062845175410986914, -0.0904970131102629,
            -0.12317643632799426, -0.16088343848395356, -0.20361801299792204,
            -0.25138015328968949, -0.30416985277905434, -0.36198710488582359,
            -0.42483190302981294, -0.49270424063084661, -0.56560411110875752,
            -0.64353150788338709, -0.72648642437458544, -0.81446885400221136,
            -0.90747879018613209, -1.0055162263462236, -1.1085811559023704,
            -1.2166735722744657, -1.3297934688824111, -1.4479408391461173,
            -1.571115676485503, -1.6993179743204958, -1.8325477260710321,
            -1.9708049251570567, -2.114089564998523, -2.2624016390153932,
            -2.4157411406276377, -2.574108063255236, -2.737502400318176,
            -2.9059241452364541, -3.0793732914300755, -3.2578498323190539,
            -3.4413537613234113, -3.6298850718631792, -3.8234437573583966,
            -4.0220298112291122, -4.2256432268953823, -4.434283997777273,
            -4.6479521172948575, -4.8666475788682186, -5.0903703759174483,
            -5.3191205018626455, -5.55289795012392, -5.7917027141213868,
            -6.0355347872751732, -0.0, -1.0, -1.0, -1.0, 0.0, 1.0, 1.0, 1.0,
            -0.0, -1.0, -0.0, -0.0, 0.0, 1.0, 0.0, 0.0, -0.0, -0.0,
            -0.0025138114032864606, -0.010055241226294552, -0.022624282888753795,
            -0.040220929810402331, -0.062845175410986914, -0.0904970131102629,
            -0.12317643632799426, -0.16088343848395356, -0.20361801299792204,
            -0.25138015328968949, -0.30416985277905434, -0.36198710488582359,
            -0.42483190302981294, -0.49270424063084661, -0.56560411110875752,
            -0.64353150788338709, -0.72648642437458544, -0.81446885400221136,
            -0.90747879018613209, -1.0055162263462236, -1.1085811559023704,
            -1.2166735722744657, -1.3297934688824111, -1.4479408391461173,
            -1.571115676485503, -1.6993179743204958, -1.8325477260710321,
            -1.9708049251570567, -2.114089564998523, -2.2624016390153932,
            -2.4157411406276377, -2.574108063255236, -2.737502400318176,
            -2.9059241452364541, -3.0793732914300755, -3.2578498323190539,
            -3.4413537613234113, -3.6298850718631792, -3.8234437573583966,
            -4.0220298112291122, -4.2256432268953823, -4.434283997777273,
            -4.6479521172948575, -4.8666475788682186, -5.0903703759174483,
            -5.3191205018626455, -5.55289795012392, -5.7917027141213868, -0.0,
            -0.0, -1.0, -1.0, 0.0, 0.0, 1.0, 1.0, -0.0, -0.0, -1.0, -0.0, 0.0,
            0.0, 1.0, 0.0, -0.0, -0.0, -0.0, -0.0025138114032864606,
            -0.010055241226294552, -0.022624282888753795, -0.040220929810402331,
            -0.062845175410986914, -0.0904970131102629, -0.12317643632799426,
            -0.16088343848395356, -0.20361801299792204, -0.25138015328968949,
            -0.30416985277905434, -0.36198710488582359, -0.42483190302981294,
            -0.49270424063084661, -0.56560411110875752, -0.64353150788338709,
            -0.72648642437458544, -0.81446885400221136, -0.90747879018613209,
            -1.0055162263462236, -1.1085811559023704, -1.2166735722744657,
            -1.3297934688824111, -1.4479408391461173, -1.571115676485503,
            -1.6993179743204958, -1.8325477260710321, -1.9708049251570567,
            -2.114089564998523, -2.2624016390153932, -2.4157411406276377,
            -2.574108063255236, -2.737502400318176, -2.9059241452364541,
            -3.0793732914300755, -3.2578498323190539, -3.4413537613234113,
            -3.6298850718631792, -3.8234437573583966, -4.0220298112291122,
            -4.2256432268953823, -4.434283997777273, -4.6479521172948575,
            -4.8666475788682186, -5.0903703759174483, -5.3191205018626455,
            -5.55289795012392, -0.0, -0.0, -0.0, -1.0, 0.0, 0.0, 0.0, 1.0, -0.0,
            -0.0, -0.0, -1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0 } };

    static const std::array<double, 66> b_Mlim_0{ { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0002, 0.0002, 0.0002, 0.0002, 0.0002, 0.0002, 0.0002,
            0.0002, 0.00010306, 0.00010306, 0.00010306, 0.00010306, 0.00010306,
            0.00010306, 0.00010306, 0.00010306 } };

    static const std::array<double, 132> a{ { -0.0001, -0.0001, -0.0001, -0.0001,
            -0.0001, -0.0001, -0.0001, -0.0001, -0.0001, -0.0001, -0.0001,
            -0.0001, -0.0001, -0.0001, -0.0001, -0.0001, -0.0001, -0.0001,
            -0.0001, -0.0001, -0.0001, -0.0001, -0.0001, -0.0001, -0.0001,
            -0.0001, -0.0001, -0.0001, -0.0001, -0.0001, -0.0001, -0.0001,
            -0.0001, -0.0001, -0.0001, -0.0001, -0.0001, -0.0001, -0.0001,
            -0.0001, -0.0001, -0.0001, -0.0001, -0.0001, -0.0001, -0.0001,
            -0.0001, -0.0001, -0.0001, -0.0001, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            -9.9999934558757648E-6, -1.9999973823514475E-5,
            -2.9999941102933263E-5, -3.9999895294149256E-5,
            -4.9999836397179591E-5, -5.9999764412041389E-5,
            -6.9999679338751775E-5, -7.9999581177327893E-5,
            -8.9999469927786859E-5, -9.9999345590145818E-5,
            -0.00010999920816442189, -0.00011999905765063221,
            -0.00012999889404879391, -0.00013999871735892412,
            -0.00014999852758103995, -0.00015999832471515855,
            -0.00016999810876129707, -0.00017999787971947261,
            -0.0001899976375897023, -0.00019999738237200327,
            -0.00020999711406639266, -0.00021999683267288759,
            -0.00022999653819150519, -0.0002399962306222626,
            -0.000249995909965177, -0.00025999557622026541,
            -0.00026999522938754507, -0.00027999486946703303,
            -0.00028999449645874642, -0.00029999411036270242,
            -0.00030999371117891811, -0.00031999329890741067,
            -0.00032999287354819719, -0.00033999243510129479,
            -0.00034999198356672065, -0.00035999151894449186,
            -0.00036999104123462554, -0.00037999055043713887,
            -0.00038999004655204894, -0.00039998952957937288,
            -0.0004099889995191278, -0.00041998845637133091,
            -0.00042998790013599927, -0.00043998733081315,
            -0.00044998674840280027, -0.00045998615290496718,
            -0.00046998554431966786, -0.00047998492264691951,
            -0.00048998428788673914, -0.00049998364003914393, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 } };

    static const std::array<double, 66> b_a{ { -0.0025138114032864606,
            -0.010055241226294552, -0.022624282888753795, -0.040220929810402331,
            -0.062845175410986914, -0.0904970131102629, -0.12317643632799426,
            -0.16088343848395356, -0.20361801299792204, -0.25138015328968949,
            -0.30416985277905434, -0.36198710488582359, -0.42483190302981294,
            -0.49270424063084661, -0.56560411110875752, -0.64353150788338709,
            -0.72648642437458544, -0.81446885400221136, -0.90747879018613209,
            -1.0055162263462236, -1.1085811559023704, -1.2166735722744657,
            -1.3297934688824111, -1.4479408391461173, -1.571115676485503,
            -1.6993179743204958, -1.8325477260710321, -1.9708049251570567,
            -2.114089564998523, -2.2624016390153932, -2.4157411406276377,
            -2.574108063255236, -2.737502400318176, -2.9059241452364541,
            -3.0793732914300755, -3.2578498323190539, -3.4413537613234113,
            -3.6298850718631792, -3.8234437573583966, -4.0220298112291122,
            -4.2256432268953823, -4.434283997777273, -4.6479521172948575,
            -4.8666475788682186, -5.0903703759174483, -5.3191205018626455,
            -5.55289795012392, -5.7917027141213868, -6.0355347872751732,
            -6.2843941630054134, -1.0, -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 } };

    static const std::array<double, 200> b_Kr_0{ { -0.0025138114032864606,
            -0.010055241226294552, -0.022624282888753795, -0.040220929810402331,
            -0.062845175410986914, -0.0904970131102629, -0.12317643632799426,
            -0.16088343848395356, -0.20361801299792204, -0.25138015328968949,
            -0.30416985277905434, -0.36198710488582359, -0.42483190302981294,
            -0.49270424063084661, -0.56560411110875752, -0.64353150788338709,
            -0.72648642437458544, -0.81446885400221136, -0.90747879018613209,
            -1.0055162263462236, -1.1085811559023704, -1.2166735722744657,
            -1.3297934688824111, -1.4479408391461173, -1.571115676485503,
            -1.6993179743204958, -1.8325477260710321, -1.9708049251570567,
            -2.114089564998523, -2.2624016390153932, -2.4157411406276377,
            -2.574108063255236, -2.737502400318176, -2.9059241452364541,
            -3.0793732914300755, -3.2578498323190539, -3.4413537613234113,
            -3.6298850718631792, -3.8234437573583966, -4.0220298112291122,
            -4.2256432268953823, -4.434283997777273, -4.6479521172948575,
            -4.8666475788682186, -5.0903703759174483, -5.3191205018626455,
            -5.55289795012392, -5.7917027141213868, -6.0355347872751732,
            -6.2843941630054134, -0.0, -0.0025138114032864606,
            -0.010055241226294552, -0.022624282888753795, -0.040220929810402331,
            -0.062845175410986914, -0.0904970131102629, -0.12317643632799426,
            -0.16088343848395356, -0.20361801299792204, -0.25138015328968949,
            -0.30416985277905434, -0.36198710488582359, -0.42483190302981294,
            -0.49270424063084661, -0.56560411110875752, -0.64353150788338709,
            -0.72648642437458544, -0.81446885400221136, -0.90747879018613209,
            -1.0055162263462236, -1.1085811559023704, -1.2166735722744657,
            -1.3297934688824111, -1.4479408391461173, -1.571115676485503,
            -1.6993179743204958, -1.8325477260710321, -1.9708049251570567,
            -2.114089564998523, -2.2624016390153932, -2.4157411406276377,
            -2.574108063255236, -2.737502400318176, -2.9059241452364541,
            -3.0793732914300755, -3.2578498323190539, -3.4413537613234113,
            -3.6298850718631792, -3.8234437573583966, -4.0220298112291122,
            -4.2256432268953823, -4.434283997777273, -4.6479521172948575,
            -4.8666475788682186, -5.0903703759174483, -5.3191205018626455,
            -5.55289795012392, -5.7917027141213868, -6.0355347872751732, -0.0,
            -0.0, -0.0025138114032864606, -0.010055241226294552,
            -0.022624282888753795, -0.040220929810402331, -0.062845175410986914,
            -0.0904970131102629, -0.12317643632799426, -0.16088343848395356,
            -0.20361801299792204, -0.25138015328968949, -0.30416985277905434,
            -0.36198710488582359, -0.42483190302981294, -0.49270424063084661,
            -0.56560411110875752, -0.64353150788338709, -0.72648642437458544,
            -0.81446885400221136, -0.90747879018613209, -1.0055162263462236,
            -1.1085811559023704, -1.2166735722744657, -1.3297934688824111,
            -1.4479408391461173, -1.571115676485503, -1.6993179743204958,
            -1.8325477260710321, -1.9708049251570567, -2.114089564998523,
            -2.2624016390153932, -2.4157411406276377, -2.574108063255236,
            -2.737502400318176, -2.9059241452364541, -3.0793732914300755,
            -3.2578498323190539, -3.4413537613234113, -3.6298850718631792,
            -3.8234437573583966, -4.0220298112291122, -4.2256432268953823,
            -4.434283997777273, -4.6479521172948575, -4.8666475788682186,
            -5.0903703759174483, -5.3191205018626455, -5.55289795012392,
            -5.7917027141213868, -0.0, -0.0, -0.0, -0.0025138114032864606,
            -0.010055241226294552, -0.022624282888753795, -0.040220929810402331,
            -0.062845175410986914, -0.0904970131102629, -0.12317643632799426,
            -0.16088343848395356, -0.20361801299792204, -0.25138015328968949,
            -0.30416985277905434, -0.36198710488582359, -0.42483190302981294,
            -0.49270424063084661, -0.56560411110875752, -0.64353150788338709,
            -0.72648642437458544, -0.81446885400221136, -0.90747879018613209,
            -1.0055162263462236, -1.1085811559023704, -1.2166735722744657,
            -1.3297934688824111, -1.4479408391461173, -1.571115676485503,
            -1.6993179743204958, -1.8325477260710321, -1.9708049251570567,
            -2.114089564998523, -2.2624016390153932, -2.4157411406276377,
            -2.574108063255236, -2.737502400318176, -2.9059241452364541,
            -3.0793732914300755, -3.2578498323190539, -3.4413537613234113,
            -3.6298850718631792, -3.8234437573583966, -4.0220298112291122,
            -4.2256432268953823, -4.434283997777273, -4.6479521172948575,
            -4.8666475788682186, -5.0903703759174483, -5.3191205018626455,
            -5.55289795012392 } };

    static const std::array<double, 8> b_Kx{ { 0.010790361874053219,
            0.040863364064094458, 0.010161922457752678, 0.038737412005276285,
            0.00955836897902516, 0.036675532874265095, 0.0089791987076130227,
            0.034677651340584592 } };

    static const std::array<double, 4> b_Ku1{ { 414.94905395997716,
            394.67572302399583, 374.94485852068169, 355.756429563771 } };

    // Outputs for Atomic SubSystem: '<Root>/VBSController'
    if ((&VBSController_M)->Timing.TaskCounters.TID[1] == 0) {
        // UnitDelay: '<S3>/last_mv'
        rtb_last_mv = VBSController_DW.last_mv_DSTATE;

        // MATLAB Function: '<S23>/optimizer' incorporates:
        //   Inport: '<Root>/Reference  Depth (m)'
        //   Inport: '<Root>/mo or x'
        //   Memory: '<S3>/Memory'
        //   Memory: '<S3>/last_x'

        for (i = 0; i < 50; i++) {
            rseq[i] = VBSController_U.ReferenceDepthm * VBSController_RYscale;
        }

        xk_idx_0 = VBSController_DW.last_x_PreviousInput[0];
        xk = VBSController_DW.last_x_PreviousInput[1];
        y_innov = (VBSController_U.moorx * VBSController_RYscale) - ((0.0001 *
            VBSController_DW.last_x_PreviousInput[0]) + (0.0 *
            VBSController_DW.last_x_PreviousInput[1]));
        rtb_xest_idx_0 = VBSController_DW.last_x_PreviousInput[0] +
            (954.01482258406793 * y_innov);
        rtb_xest_idx_1 = VBSController_DW.last_x_PreviousInput[1] +
            (478.16685883612843 * y_innov);
        for (i = 0; i < 5; i++) {
            f[i] = 0.0;
        }

        for (i = 0; i < 4; i++) {
            b_Kr = 0.0;
            for (f_tmp = 0; f_tmp < 50; f_tmp++) {
                b_Kr += b_Kr_0[f_tmp + (50 * i)] * rseq[f_tmp];
            }

            f_tmp = (i << 1);
            f[i] = (((b_Kx[f_tmp] * rtb_xest_idx_0) + (b_Kx[f_tmp + 1] *
                      rtb_xest_idx_1)) + b_Kr) + (b_Ku1[i] * rtb_last_mv);
        }

        for (f_tmp = 0; f_tmp < 66; f_tmp++) {
            b_Mlim[f_tmp] = -((b_Mlim_0[f_tmp] + ((a[f_tmp] * rtb_xest_idx_0) +
                                (a[f_tmp + 66] * rtb_xest_idx_1))) + (b_a[f_tmp]
                               * rtb_last_mv));
        }

        VBSController_qpkwik(&b_Linv[0], &b_Hinv[0], &f[0], &b_Ac[0], &b_Mlim[0],
                             &VBSController_DW.Memory_PreviousInput[0], 284,
                             1.0E-6, &zopt[0], &a__1[0], &i);
        if ((static_cast<int32_t>(i < 0)) || (static_cast<int32_t>(i == 0))) {
            for (i = 0; i < 5; i++) {
                zopt[i] = 0.0;
            }
        }

        rtb_last_mv += zopt[0];

        // Update for UnitDelay: '<S3>/last_mv' incorporates:
        //   MATLAB Function: '<S23>/optimizer'

        VBSController_DW.last_mv_DSTATE = rtb_last_mv;

        // Update for Memory: '<S3>/last_x' incorporates:
        //   MATLAB Function: '<S23>/optimizer'

        VBSController_DW.last_x_PreviousInput[0] = ((xk_idx_0 +
            (0.099999934558757636 * xk)) + (25.138114032864603 * rtb_last_mv)) +
            (1001.8314771758487 * y_innov);
        VBSController_DW.last_x_PreviousInput[1] = (((0.0 * xk_idx_0) +
            (0.999998691175438 * xk)) + (502.7621709859738 * rtb_last_mv)) +
            (478.1662329995994 * y_innov);

        // Outport: '<Root>/mv' incorporates:
        //   MATLAB Function: '<S23>/optimizer'

        VBSController_Y.mv = rtb_last_mv;
    }

    // End of Outputs for SubSystem: '<Root>/VBSController'
    rate_scheduler((&VBSController_M));
}

// Model initialize function
void VBSController::initialize()
{
    // (no initialization code required)
}

// Model terminate function
void VBSController::terminate()
{
    // (no terminate code required)
}

const char* VBSController::RT_MODEL_VBSController_T::getErrorStatus() const
{
    return (errorStatus);
}

void VBSController::RT_MODEL_VBSController_T::setErrorStatus(const char* const
    volatile aErrorStatus)
{
    (errorStatus = aErrorStatus);
}

// Constructor
VBSController::VBSController() :
    VBSController_U(),
    VBSController_Y(),
    VBSController_DW(),
    VBSController_M()
{
    // Currently there is no constructor body generated.
}

// Destructor
// Currently there is no destructor body gene
rated.
VBSController::~VBSController() = default;

// Real-Time Model get method
VBSController::RT_MODEL_VBSController_T * VBSController::getRTM()
{
    return (&VBSController_M);
}

//
// File trailer for generated code.
//
// [EOF]
//
