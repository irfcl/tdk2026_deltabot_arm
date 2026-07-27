#ifndef UROS_MISSION_HPP
#define UROS_MISSION_HPP

#ifdef __cplusplus
extern "C" {
#endif

// Include statements
//0724
typedef enum
{
    ARM_MANUAL = 0,
    ARM_MISSION
} ArmMode;
//
extern volatile ArmMode arm_mode;

extern int mission_type;
extern int prev_mission_type;
extern int mission_status;
extern int task_created;

void mission_init(void);
void mission_ctrl(void);
void mission_99(void *pvParameters);
void mission_1(void *pvParameters);
void mission_2(void *pvParameters);
void mission_3(void *pvParameters);


#ifdef __cplusplus
}
#endif

#endif // UROS_MISSION_HPP
