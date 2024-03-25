#ifndef STRUCT_CONSTANTS
#define STRUCT_CONSTANTS
#include <kipr/wombat.h>
#define tuck 100
#define yolk 0
#define dangle 50
#define gyro_axis_up_down()      (gyro_z())
#define gyro_axis_left_right() 	 (gyro_x())
#define gyro_axis_forward_back() (gyro_y())
static const int MR=2;
static const int ML=0;
static const int lIR=1;
static const int rIR=5;
static const int frisbee = 3;
static const int claw =2; 
static const int lift = 1; 
static const int fris = 1;
struct {
    int left,right;   
}static const motors = {
    .left = 0,
    .right= 2,
    
};

struct {
    int left, right;
    int black, white, midpoint;
}static const irs = {
    .left  = 1,
    .right = 5,
    .black = 3000,
    .white = 300,
    .midpoint =1600,
};

typedef int intfunc();
struct {
    intfunc *l, *r, *lf, *rf;
    int black, white, midpoint;
}static const cliffs = {
    .l = get_create_lcliff,
    .r = get_create_rcliff,
    .lf = get_create_lfcliff,
    .rf = get_create_rfcliff,
    .black = 0000,
    .white = 0000,
    .midpoint = 0000
};

struct {
    int min[4], max[4];
}static const servos = {
    .min = {0000,	100,	580,	150},//calibrate servo on ground 1800
    .max = {0000,	1400,	1840,	1600}
}; //ports:	 ^0		 ^1		 ^2		 ^3	
//orange lego red lego parrallelel  1700 to reset claw

//0=baby claw
//1=arm
//2=
//3=frisbee
struct {
    int et, light, slider;
}static const analogs = {
    .et = 2,
    .light = 0000,
    .slider = 0000
};

struct digital {
    int l, r, reset;
}static const digitals = {
    .l = 0000,
    .r = 0000,
    .reset = 0000
};
#endif
