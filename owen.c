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
	
    smart_servo(lift,85,1.3);
    smart_servo(claw,51,1.3);     // Pre run servo positions 
    smart_servo(3,0,1.3);
    msleep(500);
    wrapper(950,6);
    square_up(1,850);//// square up starting box
    smart_servo(3,100,1.2);
    wrapper(850,7);//this is the perfect drive forward distance  
    smart_servo(lift,85,1.2);
    msleep(500);
    demo_gyro_turn(800,-36,0);
    wrapper(850,9);
    
    smart_servo(claw,95,1.5);
    msleep(1500);
    smart_servo(lift,95,1.5);
    msleep(1500);//// drawer should be hooked
    demo_gyro_turn(800,3,0);
    wrapper(-800,2);//backing out with drawer
    smart_servo(claw,0,1.2);
    msleep(1200);
    smart_servo(lift,95,1.2);
    msleep(1200); // hooker off drawer
    demo_gyro_turn(800,37,0);/// turn towards mid line



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
smart_servo(claw,100,1.5);
    msleep(1500);
    smart_servo(lift,75,1.5);
    wrapper(900,20);
   
    // rockish pushesh quarry    22  HERE NOAH IS LONG DRIVE    

    demo_gyro_turn(800,30,2.5);
    wrapper(900,14);
    wrapper(-900,10);  // prepare rockish pushy
    demo_gyro_turn(800,-55,0);

   
   
    smart_servo(claw,0,1.5);
    msleep(1000);
    wrapper(900,9);
    wrapper(-900,6);
    smart_servo(lift,-5,1.5);
    demo_gyro_turn(800,-60,0);
    square_up(2,-650);
    wrapper(900,5);
    demo_gyro_turn(800,90,0);
    loop();
   
  double qu=seconds()-qu;
    printf("d,qu");




    // red cube grab 1327
}
void setup_for_purple(){
    //move into position
    smart_servo(frisbee,100,1.5);
    homeArm();
    msleep(500);
    while(gmpc(fris)<35){
        motor(fris,25);
    }
    motor(fris,0);
    msleep(500);//wait for noodles to drop. I don't know why this happens lol.
    wrapper(-1200,6.9);
    demo_gyro_turn(800,35,0);
// seiz();  // HERE IS JULIAS SEIZURE CLEMENT!
 
    
 //   while(gmpc(fris)<80){//100
  //      motor(fris,15);//25
//    }
 //   motor(fris,0);
}
void setup_for_dropoff(){
    demo_gyro_turn(800,-20,0);
    while(gmpc(fris)<100){
        motor(fris,25);
    }
    motor(fris,0);
    square_up(1,900);
    wrapper(1000,15);
    demo_gyro_turn(800,-90,0);
    square_up(1,-900);
    wrapper(1000,-2);
    demo_gyro_turn(800,90,0);
    
    
    //wrapper(-900,8);
    homeArm();
    while(gmpc(fris)<80){
        motor(fris,25);
    }
    motor(fris,0);
    wrapper(900,10);
}
void dropoff(){
    smart_servo(frisbee,10,1.5);
    msleep(1500);
    wrapper(-700,7.5);
}   

void red_cube_approach()
{
  /*  smart_servo(lift,-5,1.5);
    wrapper(900,3.1);
    msleep(300);
    demo_gyro_turn(800,-140,0);
    wrapper(900,5);
    smart_servo(claw,50,1);
    msleep(1500);
    wrapper(-900,3);
    demo_gyro_turn(800,140,1.5);
    msleep(500);
    smart_servo(claw,0,1.5);
    wrapper(-900,3);
    demo_gyro_turn(800,-90,0);
   
    square_up(1,-600);
    wrapper(-900,3);
   
    
    demo_gyro_turn(800,-87,0);
    wrapper(900,3);
   */
    //this is where the start to the next section begins. Robot is facing starting boxes, inside of big zone. Denoted by blue dot on board which should be on left wheel.(+/-1' any direction)
 smart_servo(lift,10,1.5);
    smart_servo(claw,0,1.5);
    smart_servo(3,100,1.5); /// not needed
    square_up(1,800);
    wrapper(1200,5);
    demo_gyro_turn(800,85,0);    // do not change
  msleep(500);
    demo_gyro_drive_right_black(800, 2);//this moves toward perfect distance from cubes
    msleep(2000);//place holder
    wrapper(1200,.5);//micro adjustment // do not change
    smart_servo(claw,70,.7);//close around two red cubes

    msleep(900);
       demo_gyro_turn(1200,-90,0);
    wrapper(-1200,7);
    square_up(2,800);
    demo_gyro_turn(1200,-25,0);
    wrapper(1200,20.5);
   smart_servo(lift,50,.5);
    smart_servo(claw,8,.1);
    msleep(1000);
    demo_gyro_turn(1200,-70,0);
    smart_servo(lift,100,.5);
    square_up(2,-800);
    wrapper(900,12);
  demo_gyro_turn(1000,-90,0);
    wrapper(1200,6);
    square_up(2,800);
  
   
   
    
    
   
//    demo_gyro_turn(800,35,0);



}

void loop()
{
    double q=seconds();
    while((seconds()-q<3)) 
    {

        mav(0,3*(600-analog(3)));
        mav(2,3*(600-analog(3)));
        msleep(3);
        int q=q+1;
    }
}
	void seiz(){
         motor(0,-100);
    motor(2,-100);
    msleep(35);
      motor(0,100);
    motor(2,100);
    msleep(35);
      motor(0,-100);
    motor(2,-100);
    msleep(35);
      motor(0,100);
    motor(2,100);
    msleep(35);
      motor(0,-100);
    motor(2,-100);
    msleep(35);
      motor(0,100);
    motor(2,100);
    msleep(35);
      motor(0,-100);
    motor(2,-100);
    msleep(35);
      motor(0,100);
    motor(2,100);
    msleep(35);
   
    motor(0,100);
    motor(2,-100);
    msleep(35);
      motor(0,-100);
    motor(2,100);
    msleep(35);
      motor(0,100);
    motor(2,-100);
    msleep(35);
      motor(0,100);
    motor(2,-100);
    msleep(35);
      motor(0,100);
    motor(2,-100);
    msleep(35);
      motor(0,100);
    motor(2,-100);
    msleep(35);
      motor(0,100);
    motor(2,-100);
    msleep(35);
      motor(0,100);
    motor(2,-100);
    msleep(35);
\
     motor(0,-100);
    motor(2,-100);
    msleep(35);
      motor(0,100);
    motor(2,100);
    msleep(35);
      motor(0,-100);
    motor(2,-100);
    msleep(35);
      motor(0,100);
    motor(2,100);
    msleep(35);
      motor(0,-100);
    motor(2,-100);
    msleep(35);
      motor(0,100);
    motor(2,100);
    msleep(35);
      motor(0,-100);
    motor(2,-100);
    msleep(35);
      motor(0,100);
    motor(2,100);
    msleep(35);
    motor(0,0);
    motor(2,0);
    msleep(35);
    
    }






