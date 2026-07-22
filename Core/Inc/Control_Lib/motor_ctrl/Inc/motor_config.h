#define WHEEL_RADIUS 0.1 // Wheel radius in meters
#define ENCODER_RESOLUTION 256.0 // Encoder counts per revolution
#define REDUCTION_RATIO  24// Maximum duty cycle for PWM
#define PWM_ARR 1000
#define INTEGRAL_LIMIT 0.5
#define DT 10 // ms

#define ARM_ENCODER_CPR 424.0f
#define ARM_RATIO 30.0f

// AX01                    B10
// GO_1         T3C1       A6
// GO_2         T3C2       A7
// GO_3         T3C3       B0
// GO_4         T3C4       B1
// MG90         T5C1       A0

//                PWM             | DIR        |EN
// 抬升soho
//   |--- _R    T12C1    B14    B12/B13
//   |__  _L    T12C2    B15    C2/C3
// 升降          T5C2     A1     A12/C4         |T1C1 A8/ T1C2 A9
// 推桿
//   |---  _FR   T4C1     B6     A4/A13
//   |---  _FL   T4C2     B7     C0/C1
//   |---  _BR   T4C3     B8     A14/A15
//   |__   _BL   T4C4     B9     C14/C15
// 微動
//   |---  SWI_LIFT     C12
//   |---  SWI_CAS      D2
//   |---  SWI_MIS      C13
//   |__   SWI_FRO      B3
//   |__   SWI_1        H0
//   |__   SWI_2        H1
