#include <stdlib.h>
#include <kipr/wombat.h>
#include <locolib.h>
#include <owen.h>
#include <constants.h>
void d(int speed, double  distance){
    wrapper(speed,distance);
}
void s(int speed){
    demo_gyro_square_up(0);
}
void wrapper(int speed, double distance){
    double magic_coefficient = 1.0000;/// you can start at this point and adjust it so that the distance it actually travels is equal to desired distance
    double time = fabs(distance / speed)*270*magic_coefficient;
    long run_start_time =systime();
    long Drive_func_start_time = systime();
    demo_gyro_drive(speed,time);




    //tme may have to be divided by 1000 depending on whether Reese uses secs or msecs in guts
 
    long Drive_func_end_time = systime();
    double function_elapsed_time = (Drive_func_end_time - Drive_func_start_time)/1000;//this /1000 should turn it into seconds, if it doesnbt, divide by whatever multiple of 10 does it.
    double program_elapsed_time =(Drive_func_end_time - run_start_time)/1000;
    printf("Function Time = %f seconds Total Elapsed Time of Run = %f seconds",function_elapsed_time, program_elapsed_time);
}