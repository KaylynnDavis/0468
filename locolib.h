#ifndef LOCOMOTION_LIBRARY
#define LOCOMOTION_LIBRARY
#include <constants.h>
// Version 3.2


//// ==== ==== ==== ==== ====
// General Utilities (exposed)
void to_lower(char *string, unsigned short length);
unsigned int milliseconds();
unsigned int nanoseconds();
int sign(int x);

struct pid_struct* pid_create(double p, double i, double d);
double pid_calc(struct pid_struct *pid, double setpoint, double process_var);
    
struct pid_struct{
    double p, i, d;
    double i_sum;
    double error_prev;
    double start_prev;
};

#define vrg_cnt(vrg1,vrg2,vrg3,vrg4,vrg5,vrg6,vrg7,vrg8,vrgN, ...) vrgN
#define vrg_argn(...)  vrg_cnt(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define vrg_cat0(x,y)  x ## y
#define vrg_cat(x,y)   vrg_cat0(x,y)
#define vrg(vrg_f,...) vrg_cat(vrg_f, vrg_argn(__VA_ARGS__))(__VA_ARGS__)

//// ==== ==== ==== ==== ====
// Gyro Calibration
#define gyro_offset 10 
// This implomentation is scaled by gyro_offset
int gyro_x_bias();
int gyro_y_bias();
int gyro_z_bias();
void calibrate_gyro();

//// ==== ==== ==== ==== ====
// Linked list management
struct list_head {
    struct list_head *next, *prev;   
};

void list_prepend(struct list_head *prependee, struct list_head *head);
void list_append(struct list_head *appendee, struct list_head *head);
void list_remove(struct list_head *removee);

//// ==== ==== ==== ==== ===
// Smart Servo Controls
#define percent_default 50
#define time_default 1.5
#define delay_default 0

unsigned int smart_servoX(int port, int percent, unsigned int duration_ms, unsigned int delay_ms);
#define smart_servo_ms(x,y,z,a) smart_servoX(x,y,z,a)
#define smart_servo_sec(x,y,z,a) smart_servoX(x,y,z*1000,a*1000)
struct servo_message *smart_servo_search_id(unsigned int id);
#define smart_servo_get(x) smart_servo_search_id(x)
void document_queue();

#define smart_servo(...)      vrg(smart_servo, __VA_ARGS__)
#define smart_servo1(x)       smart_servo_sec(x,percent_default,time_default,delay_default)
#define smart_servo2(x,y)     smart_servo_sec(x,y,time_default,delay_default)
#define smart_servo3(x,y,z)   smart_servo_sec(x,y,z,delay_default)
#define smart_servo4(x,y,z,i) smart_servo_sec(x,y,z,i)

//// ==== ==== ==== ==== ====
// Demo-Gyro Movements
#define wheel_base_width 7.8
void demo_gyro_turn(int speed, int angleDegrees, double radius);
void demo_gyro_drive(int speed, double distance);
void demo_gyro_square_up(int speed);
#endif
