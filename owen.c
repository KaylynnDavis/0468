#include <stdlib.h>
#include <kipr/wombat.h>
#include <locolib.h>
#include <owen.h>
#include <constants.h>
void d(int speed, double  distance)
{
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
void win()
{

    drawer();
    //rock
    //orange

}
void drawer() {
    //344 up 2000 down
    //demo_gyro_turn(int speed, int angleDegrees, double radius)
    //demo_gyro_square_up(int speed)

    smart_servo(lift,75,1.5);
    smart_servo(claw,75,1.5);
    msleep(1500);
    wrapper(800,6);
    square_up(1,650);//// square up starting box
    wrapper(800,7);  
    smart_servo(lift,85,1.5);
    msleep(1500);
    demo_gyro_turn(800,-36,0);
    wrapper(800,8.75);
    smart_servo(claw,95,1.5);
    msleep(1500);
    smart_servo(lift,100,1.5);
    msleep(1500);//// drawer should be hooked
    demo_gyro_turn(800,3,0);
    wrapper(-800,2);
    smart_servo(claw,0,1.5);
    msleep(1500);
    smart_servo(lift,95,1.5);
    msleep(1500); // hooker off drawer
    demo_gyro_turn(800,39,0);/// turn towards mid line
    


}


void starting_pos()
{
    //wait_for_light();
    //shut_down_in(119);

    smart_servo(claw,100,1.5);

    msleep(1500);


}


void homeArm(){
    //placeholder values, replace with variables later
    while(digital(0)==0){
        motor(fris,20*(digital(0)-1));
        msleep(50);
    }
    motor(fris,0);
    cmpc(fris);
}
void rock(){
    square_up(2,650);
  wrapper(-800,1);
   
    
    
      smart_servo(claw,103,1.2);   ///
    demo_gyro_turn(800,15,0);   ///
    msleep(500);
    demo_gyro_turn(800,-15,0);  ///
    smart_servo(lift,25,1.5);
   msleep(1000);
    
    wrapper(900,22);// rockish pushesh quarry
    demo_gyro_turn(800,30,0);
    wrapper(900,14);
    wrapper(-900,9);  // prepare rockish pushy
    demo_gyro_turn(800,-60,0);

    smart_servo(lift,90,1.5);
    msleep(1500);
    smart_servo(claw,0,1.5);
    msleep(1000);
    wrapper(900,5);
    wrapper(-900,4);
    smart_servo(lift,-5,1.5);
    demo_gyro_turn(800,-55,0);
    square_up(1,-650);
    
   

    // red cube grab 1327
}
void setup_for_purple(){
    smart_servo(frisbee,100,1.5);
    homeArm();
	msleep(500);
    while(gmpc(fris)<45){
    	motor(fris,25);
    }
    motor(fris,0);
    //needs to be moved into position before the noodles drop. Raise arm up slightly before the drop off.
}
void setup_for_dropoff(){
    wrapper(-900,8);
    homeArm();
    while(gmpc(fris)<80){
    	motor(fris,25);
    }
    motor(fris,0);
    wrapper(900,8);
}
void dropoff(){
	smart_servo(frisbee,10,1.5);
    msleep(1500);
    wrapper(-700,7.5);
}   

void red_cube_approach()
{
    smart_servo(lift,-5,1.5);
    wrapper(900,3.1);
    msleep(300);
    demo_gyro_turn(800,-38,3.9);
    wrapper(900,6);
    smart_servo(claw,110,1.5);
    msleep(1000);
    demo_gyro_turn(800,90,1.5);
    smart_servo(claw,0,1.5);
    
    

}







