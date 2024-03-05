#ifndef STRUCT_CONSTANTS
#define STRUCT_CONSTANTS
#include <kipr/wombat.h>

#define gyro_axis_up_down()      (gyro_z())
#define gyro_axis_left_right() 	 (gyro_x())
#define gyro_axis_forward_back() (gyro_y())
static const int target = 3;
static const int claw =2; 
static const int lift = 1; ;

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
    .left  = 3,
    .right = 5,
    .black = 2300,
    .white = 300,
    .midpoint =2300,
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
    .min = {300,	150,	727,	0000},//calibrate servo on ground 1800
    .max = {1980,	1462,	1404,	2047}
}; //ports:	 ^0		 ^1		 ^2		 ^3	

struct {
    int et, light, slider;
}static const analogs = {
    .et = 0000,
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
