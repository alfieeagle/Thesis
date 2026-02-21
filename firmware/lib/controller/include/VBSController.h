//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: VBSController.h
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
#ifndef VBSController_h_
#define VBSController_h_
#include <stdbool.h>
#include <stdint.h>
#include <cmath>
#include "complex_types.h"
#include <array>

extern "C"
{
    static double rtGetInf(void);
    static float rtGetInfF(void);
    static double rtGetMinusInf(void);
    static float rtGetMinusInfF(void);
}                                      // extern "C"

extern "C"
{
    static double rtGetNaN(void);
    static float rtGetNaNF(void);
}                                      // extern "C"

extern "C"
{
    extern double rtInf;
    extern double rtMinusInf;
    extern double rtNaN;
    extern float rtInfF;
    extern float rtMinusInfF;
    extern float rtNaNF;
    static bool rtIsInf(double value);
    static bool rtIsInfF(float value);
    static bool rtIsNaN(double value);
    static bool rtIsNaNF(float value);
}                                      // extern "C"

// Class declaration for model VBSController
class VBSController final
{
    // public data and function members
  public:
    // Block states (default storage) for system '<Root>'
    struct DW_VBSController_T {
        double last_mv_DSTATE;         // '<S3>/last_mv'
        std::array<double, 2> last_x_PreviousInput;// '<S3>/last_x'
        std::array<bool, 66> Memory_PreviousInput;// '<S3>/Memory'
    };

    // Invariant block signals (default storage)
    struct ConstB_VBSController_T {
        double MathFunction;           // '<S3>/Math Function'
        double MathFunction1;          // '<S3>/Math Function1'
        double MathFunction2;          // '<S3>/Math Function2'
    };

    // External inputs (root inport signals with default storage)
    struct ExtU_VBSController_T {
        double moorx;                  // '<Root>/mo or x'
        double ReferenceDepthm;        // '<Root>/Reference  Depth (m)'
    };

    // External outputs (root outports fed by signals with default storage)
    struct ExtY_VBSController_T {
        double mv;                     // '<Root>/mv'
    };

    // Real-time Model Data Structure
    struct RT_MODEL_VBSController_T {
        const char * volatile errorStatus;

        //
        //  Timing:
        //  The following substructure contains information regarding
        //  the timing information for the model.

        struct {
            struct {
                uint8_t TID[2];
            } TaskCounters;
        } Timing;

        const char* getErrorStatus() const;
        void setErrorStatus(const char* const volatile aErrorStatus);
    };

    // Copy Constructor
    VBSController(VBSController const&) = delete;

    // Assignment Operator
    VBSController& operator= (VBSController const&) & = delete;

    // Move Constructor
    VBSController(VBSController &&) = delete;

    // Move Assignment Operator
    VBSController& operator= (VBSController &&) = delete;

    // Real-Time Model get method
    VBSController::RT_MODEL_VBSController_T * getRTM();

    // Root inports set method
    void setExternalInputs(const ExtU_VBSController_T *pExtU_VBSController_T)
    {
        VBSController_U = *pExtU_VBSController_T;
    }

    // Root outports get method
    const ExtY_VBSController_T &getExternalOutputs() const
    {
        return VBSController_Y;
    }

    // model initialize function
    static void initialize();

    // model step function
    void step();

    // model terminate function
    static void terminate();

    // Constructor
    VBSController();

    // Destructor
    ~VBSController();

    // private data and function members
  private:
    // External inputs
    ExtU_VBSController_T VBSController_U;

    // External outputs
    ExtY_VBSController_T VBSController_Y;

    // Block states
    DW_VBSController_T VBSController_DW;

    // private member function(s) for subsystem '<Root>'
    double VBSController_norm(const double x[5]);
    double VBSController_maximum(const double x[5]);
    double VBSController_xnrm2(int32_t n, const double x[25], int32_t ix0);
    void VBSController_xgemv(int32_t b_m, int32_t n, const double b_A[25],
        int32_t ia0, const double x[25], int32_t ix0, double y[5]);
    void VBSController_xgerc(int32_t b_m, int32_t n, double alpha1, int32_t ix0,
        const double y[5], double b_A[25], int32_t ia0);
    double VBSController_KWIKfactor(const double b_Ac[330], const int32_t iC[66],
        int32_t nA, const double b_Linv[25], double RLinv[25], double D[25],
        double b_H[25], int32_t n);
    void VBSController_DropConstraint(int32_t kDrop, bool iA[66], int32_t *nA,
        int32_t iC[66]);
    void VBSController_qpkwik(const double b_Linv[25], const double b_Hinv[25],
        const double f[5], const double b_Ac[330], const double b[66], bool iA
        [66], int32_t maxiter, double FeasTol, double x[5], double lambda[66],
        int32_t *status);

    // Real-Time Model
    RT_MODEL_VBSController_T VBSController_M;
};

extern const VBSController::ConstB_VBSController_T VBSController_ConstB;// constant block i/o 

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S3>/Constant' : Unused code path elimination
//  Block '<S3>/Floor' : Unused code path elimination
//  Block '<S3>/Floor1' : Unused code path elimination
//  Block '<S4>/Matrix Dimension Check' : Unused code path elimination
//  Block '<S5>/Matrix Dimension Check' : Unused code path elimination
//  Block '<S6>/Matrix Dimension Check' : Unused code path elimination
//  Block '<S7>/Matrix Dimension Check' : Unused code path elimination
//  Block '<S8>/Matrix Dimension Check' : Unused code path elimination
//  Block '<S9>/Matrix Dimension Check' : Unused code path elimination
//  Block '<S10>/Matrix Dimension Check' : Unused code path elimination
//  Block '<S11>/Matrix Dimension Check' : Unused code path elimination
//  Block '<S12>/Matrix Dimension Check' : Unused code path elimination
//  Block '<S13>/Matrix Dimension Check' : Unused code path elimination
//  Block '<S14>/Matrix Dimension Check' : Unused code path elimination
//  Block '<S15>/Matrix Dimension Check' : Unused code path elimination
//  Block '<S16>/Vector Dimension Check' : Unused code path elimination
//  Block '<S17>/Vector Dimension Check' : Unused code path elimination
//  Block '<S18>/Vector Dimension Check' : Unused code path elimination
//  Block '<S19>/Vector Dimension Check' : Unused code path elimination
//  Block '<S20>/Vector Dimension Check' : Unused code path elimination
//  Block '<S21>/Vector Dimension Check' : Unused code path elimination
//  Block '<S3>/Min' : Unused code path elimination
//  Block '<S3>/constant' : Unused code path elimination
//  Block '<S22>/Vector Dimension Check' : Unused code path elimination
//  Block '<S3>/umin_scale2' : Unused code path elimination
//  Block '<S3>/umin_scale3' : Unused code path elimination
//  Block '<S3>/umin_scale5' : Unused code path elimination
//  Block '<S3>/ym_zero' : Unused code path elimination
//  Block '<S2>/m_zero' : Unused code path elimination
//  Block '<S2>/p_zero' : Unused code path elimination
//  Block '<S3>/Reshape' : Reshape block reduction
//  Block '<S3>/Reshape1' : Reshape block reduction
//  Block '<S3>/Reshape2' : Reshape block reduction
//  Block '<S3>/Reshape3' : Reshape block reduction
//  Block '<S3>/Reshape4' : Reshape block reduction
//  Block '<S3>/Reshape5' : Reshape block reduction
//  Block '<S3>/ext.mv_scale' : Eliminated nontunable gain of 1
//  Block '<S3>/ext.mv_scale1' : Eliminated nontunable gain of 1
//  Block '<S3>/umin_scale1' : Eliminated nontunable gain of 1
//  Block '<S3>/umin_scale4' : Eliminated nontunable gain of 1
//  Block '<S3>/ymin_scale2' : Eliminated nontunable gain of 1


//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Note that this particular code originates from a subsystem build,
//  and has its own system numbers different from the parent model.
//  Refer to the system hierarchy for this subsystem below, and use the
//  MATLAB hilite_system command to trace the generated code back
//  to the parent model.  For example,
//
//  hilite_system('mpc_simulink/VBSController')    - opens subsystem mpc_simulink/VBSController
//  hilite_system('mpc_simulink/VBSController/Kp') - opens and selects block Kp
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'mpc_simulink'
//  '<S1>'   : 'mpc_simulink/VBSController'
//  '<S2>'   : 'mpc_simulink/VBSController/MPC Controller'
//  '<S3>'   : 'mpc_simulink/VBSController/MPC Controller/MPC'
//  '<S4>'   : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Matrix Signal Check'
//  '<S5>'   : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Matrix Signal Check1'
//  '<S6>'   : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Matrix Signal Check2'
//  '<S7>'   : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Preview Signal Check'
//  '<S8>'   : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Preview Signal Check1'
//  '<S9>'   : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Preview Signal Check2'
//  '<S10>'  : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Preview Signal Check3'
//  '<S11>'  : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Preview Signal Check4'
//  '<S12>'  : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Preview Signal Check5'
//  '<S13>'  : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Preview Signal Check6'
//  '<S14>'  : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Preview Signal Check7'
//  '<S15>'  : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Preview Signal Check8'
//  '<S16>'  : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Scalar Signal Check'
//  '<S17>'  : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Scalar Signal Check1'
//  '<S18>'  : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Scalar Signal Check2'
//  '<S19>'  : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Vector Signal Check'
//  '<S20>'  : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Vector Signal Check1'
//  '<S21>'  : 'mpc_simulink/VBSController/MPC Controller/MPC/MPC Vector Signal Check6'
//  '<S22>'  : 'mpc_simulink/VBSController/MPC Controller/MPC/moorx'
//  '<S23>'  : 'mpc_simulink/VBSController/MPC Controller/MPC/optimizer'
//  '<S24>'  : 'mpc_simulink/VBSController/MPC Controller/MPC/optimizer/optimizer'

#endif                                 // VBSController_h_

//
// File trailer for generated code.
//
// [EOF]
//
