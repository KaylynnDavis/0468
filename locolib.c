#include <constants.h>
#include <locolib.h>
#include <time.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
// Version 3.2

//// ==== ==== ==== ==== ==== ==== ==== ==== ====

struct {
    int x,y,z;
}static gyro_bias;

struct servo_queue_head {
    struct list_head *queue[4];
    unsigned int next_base_id;
    mutex lock;
    thread thread;
}servo_hub = {
    .queue = {NULL,NULL,NULL,NULL},
    .next_base_id = -1
};

struct servo_message {
    struct list_head list;
    int end_pos, start_pos;
    unsigned int id;
    unsigned int start_ms, duration_ms, end_ms;
};

//// ==== ==== ==== ==== ==== ==== ==== ==== ====

#define container_of(pntr,type,member)({ \
	const typeof( ((type *)0)->member) *__mptr = (pntr); \
	(type *)( (char *)__mptr - offsetof(type,member));})
//^ Pointer magic used in the Linux Kernel for doubly-linked lists.
// using the pointer of list_head, one can gain the containing structure.

static inline void motormotor(int left_speed, int right_speed) {
    mav(motors.left, left_speed);   
    mav(motors.right,right_speed);
}

void to_lower(char *string, unsigned short length) {
    unsigned short index = 0;
    short diff = 'a' - 'A';
    while(index < length) {
        if(string[index]>='A' && string[index]<='Z') {
            string[index]+=diff;
        }
        index += 1;
    }
}

unsigned int milliseconds() {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return (now.tv_sec*1000) + (now.tv_nsec/1000000); 
}
unsigned int nanoseconds() {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_nsec; 
}

#define sign(x) (((x)>0)-((x)<0))

static void *safe_malloc(size_t size) {
    void *pntr = malloc(size);
    if(pntr) {
        return pntr;
    }
    printf("out of memory!\n");
    exit(EXIT_FAILURE);
}

//// ==== ==== ==== ==== ==== ==== ==== ==== ====

struct pid_struct* pid_create(double p, double i, double d) {
    struct pid_struct* pid = safe_malloc(sizeof(struct pid_struct));
    struct pid_struct two = {
        .p=p, .i=i, .d=d, 
        .i_sum=0, 
        .error_prev=0, 
        .start_prev=seconds()
    }; 
    *pid = two;
    return pid;
}

double pid_calc(struct pid_struct *pid, double setpoint, double process_var) {
    double dt, start = seconds();                   //> Get and calculate
    dt = start - pid -> start_prev;                 // the start time and 
    pid -> start_prev = start;                      // the delta of time

    double derror, error = setpoint - process_var;  //> Get and calculate
    derror = error - pid -> error_prev;             // error, and delta 
    pid -> error_prev = error;                      // error

    pid -> i_sum += error * dt * pid -> i;

    if(dt != 0) { //> if statent to avoid dividing by 0
        return (error * pid -> p) + pid -> i_sum - (derror * pid->d / dt);
    }
    return (error *pid -> p) + pid -> i_sum;
}

// gyro_?_bias is scalled by gyro_offset
// scale at the time of writing is 10. check the .h for new value
int gyro_x_bias() {
    return gyro_axis_left_right() * gyro_offset - gyro_bias.x;   
}
int gyro_y_bias() {
    return gyro_axis_forward_back() * gyro_offset - gyro_bias.y;
}
int gyro_z_bias() {
    return gyro_axis_up_down() * gyro_offset - gyro_bias.z;
}

void calibrate_gyro() {
    int i, loops = 100;
    for(i=0;i<loops;i++) {
        gyro_bias.z  += gyro_axis_up_down();
        gyro_bias.x  += gyro_axis_left_right();
        gyro_bias.y  += gyro_axis_forward_back();
        msleep(7);
    }
    loops /= gyro_offset;
    gyro_bias.x /= loops;
    gyro_bias.y /= loops;
    gyro_bias.z /= loops;
    printf("gyro calibration end\n");
}

////////////////////////////////////////////////////////////////////////////////
//// ==== ==== ==== ====

void list_prepend(struct list_head *prependee, struct list_head *head) {
    prependee->next = head;
    prependee->prev = head->prev;
    head->prev = prependee;
    if(prependee->prev != NULL) {
        prependee->prev->next = prependee;
    }
}

void list_append(struct list_head *appendee, struct list_head *head) {
    appendee->prev = head;
    appendee->next = head->next;
    head->next = appendee;
    if(appendee->next != NULL) {
        appendee->next->prev = appendee;
    }
}

void list_remove(struct list_head *removee) {
    if(removee->prev != NULL) {
        removee->prev->next = removee->next;
    }
    if(removee->next != NULL) {
        removee->next->prev = removee->prev;
    }
    removee->next = removee;
    removee->prev = removee;
}

////////////////////////////////////////////////////////////////////////////////
//// ==== ==== ==== ====

static void smart_servo_update_servo(int port)
{
    if(NULL == servo_hub.queue[port])return;
    unsigned int now = milliseconds();
    int exit=0;
    mutex_lock(servo_hub.lock);
    while(!exit) {
        struct servo_message *data = container_of(servo_hub.queue[port], struct servo_message, list);
        if(data->start_ms > now) {
            return mutex_unlock(servo_hub.lock);
        }
        if(data->start_pos==-1) {    
            data->start_pos = get_servo_position(port);
        }
        if(now > data->end_ms) {
            if(data->list.next==&data->list) {
                servo_hub.queue[port] = NULL;
                set_servo_position(port,data->end_pos);
                free(data);
                mutex_unlock(servo_hub.lock); 			//< One line Fix
                return;
            }
            servo_hub.queue[port] = data->list.next;
            list_remove(&data->list);
            set_servo_position(port,data->end_pos);
            free(data);
            continue;
        }

        float time_position =  (now - data->start_ms) / ((float)data->duration_ms);
        int new_position = time_position*(data->end_pos-data->start_pos) + data->start_pos;
        set_servo_position(port,new_position);
        exit=1;
    }
    mutex_unlock(servo_hub.lock);
}

static void smart_servo_thread()
{
    int port=0;
    while(1) {
        msleep(10);
        port = (port+1)%4;
        if(servo_hub.queue[port]==NULL) {
            continue;
        }
        if(!get_servo_enabled(port)) {
            enable_servo(port);
        }
        smart_servo_update_servo(port);
    }
}

unsigned int program_init;
static void smart_servo_init()
{
    if(servo_hub.next_base_id != -1 ) {
        return;
    }
    servo_hub.lock = mutex_create();
    servo_hub.thread = thread_create(smart_servo_thread);
    thread_start(servo_hub.thread);
    servo_hub.next_base_id = 0;
    program_init = milliseconds();
    //printf("size: %d\n",sizeof(struct servo_message));
}

unsigned int smart_servoX(int port, int percent, unsigned int duration_ms, unsigned int delay_ms) 
{
    smart_servo_init();

    struct list_head **queue = &servo_hub.queue[port];
    struct servo_message *new_message = safe_malloc(sizeof(struct servo_message));
    new_message->list.next = &new_message->list;
    new_message->list.prev = &new_message->list;
    new_message->end_pos = (servos.max[port]-servos.min[port])*(percent/100.0)+servos.min[port];
    new_message->start_pos = -1;
    new_message->id = servo_hub.next_base_id + port; //port added so that modulo 4 will return the port. Used in searching for list element
    new_message->start_ms = milliseconds() + delay_ms;
    new_message->duration_ms = duration_ms;
    new_message->end_ms = new_message->start_ms + duration_ms; 
    servo_hub.next_base_id += 4;
    
    //memcpy(new_message,holding,sizeof(struct servo_message))

    mutex_lock(servo_hub.lock);
    if(servo_hub.queue[port]==NULL) {//< Case for when this is the first element in the array
        *queue = &new_message->list;
        mutex_unlock(servo_hub.lock);
        //printf("replacing NULL\n");
        return new_message->id;
    }

    struct servo_message *last = container_of(servo_hub.queue[port]->prev, struct servo_message, list);
    if(new_message->start_ms > last->start_ms) {//< Check the end of the list to see if it fits at the end
        list_append(&new_message->list,&last->list);
        mutex_unlock(servo_hub.lock);
        //printf("appending to endof\n");
        return new_message->id;
    }

    struct list_head *initial = servo_hub.queue[port];
    struct servo_message *previous, *itterator; // = container_of(initial, struct servo_message, list);
    previous = container_of(initial, struct servo_message, list);

    if(previous->start_ms > new_message->start_ms)
    {
        list_prepend(&new_message->list,&previous->list);
        servo_hub.queue[port] = &new_message->list;
        //printf("prepended to %u. New list lead: %u\n",previous->id, new_message->id);
        mutex_unlock(servo_hub.lock);
        return new_message->id;
    }

    itterator = container_of(previous->list.next, struct servo_message, list);
    //printf("my: %lu	p: %lu	i: %lu\n",new_message->start_ms-program_init,previous->start_ms-program_init,itterator->start_ms-program_init);
    while(itterator->list.next != initial) {//< for any other case, make the long search through the list
        if(new_message->start_ms > previous->start_ms && new_message->start_ms < itterator->start_ms) {
            break;
        }
        previous = itterator;
        itterator = container_of(itterator->list.next, struct servo_message, list);
        //printf("my: %lu	p: %lu	i: %lu\n",new_message->start_ms-program_init,previous->start_ms-program_init,itterator->start_ms-program_init);
    }
    list_append(&new_message->list,&previous->list);
    //printf("appended to %u\n",previous->id);
    mutex_unlock(servo_hub.lock);
    return new_message->id;   
}

struct servo_message *smart_servo_search_id(unsigned int id)  {
    int port = id%4;
    struct servo_message *itterator = container_of(servo_hub.queue[port], struct servo_message, list);
    if(itterator==NULL)return NULL;
    struct servo_message *initial = itterator;
    do{
        if(itterator->id == id)
            return itterator;
        itterator = container_of(itterator->list.next, struct servo_message, list);
    }while(itterator!=initial);
    return NULL;
}

void document_queue()
{
    int port;
    struct servo_message *initial, *itterator;
    mutex_lock(servo_hub.lock);
    for(port=0;port<4;port++) {
        printf("servo %d: ",port);
        if(servo_hub.queue[port]==NULL) {
            printf("\n");
            continue;
        }
        initial = container_of(servo_hub.queue[port], struct servo_message, list);
        printf("%d",initial->id);

        itterator = container_of(initial->list.next, struct servo_message, list);
        while(itterator != initial) {
            printf(", %d",itterator->id);
            itterator = container_of(itterator->list.next,struct servo_message,list);
        }
        printf("\n");
    }
    mutex_unlock(servo_hub.lock);
}

#undef container_of
//// ==== ==== ==== ====

void demo_gyro_turn(int speed, int angleDegrees, double radius) {
    if( (angleDegrees %= 360) == 0 || speed == 0) return; //< exit if no turn is needed
    radius = fabs(radius); //< make radius positive as distance is always a non-negative value

    double start_time = seconds(), target_time = seconds() + abs(speed)*0.00015;
    //^ setup initial time and ending time for acceleration range

    struct {double speed_modifier; int port;}
    outter = { .port = (angleDegrees>0) * motors.left +(angleDegrees<0) * motors.right, //set port of outter motors
              .speed_modifier = sign(angleDegrees)},								 // set direction that outter motors travels
    inner  = { .port = (angleDegrees>0) * motors.right+(angleDegrees<0) * motors.left, // set port of inner motors
              .speed_modifier = ( (radius*2)/(radius+7.8)-1 )* sign(angleDegrees)}; // equation of inner motors for radius turns
    if(radius==-wheel_base_width) inner.speed_modifier = sign(angleDegrees); 	    // edge case that fills a divide by zero senario

    short sign_angle_speed = sign(angleDegrees) * sign(speed); //rotation direction
    angleDegrees*=sign(speed);	// gets the sign of speed. result is 1 or -1
    speed = abs(speed);			// makes speed a positive value

    double theta=0,pid_out; //set theta, the current angle, to zero, and create the output variable of the pid
    int inner_speed,outter_speed; //variables that will store the calculated speed of inner and outter wheels before mav (not entirely necessary)
    struct pid_struct *pid = pid_create(0.07,0,0.0065); // creates a pid with values in the order of: proportional, integral, and derrivative

    while( ((sign_angle_speed<0 && (theta>angleDegrees+0.3 || theta<angleDegrees-0.3)) || //case set if rotating left
            (sign_angle_speed>0 && (theta<angleDegrees-0.3 || theta>angleDegrees+0.3)))|| //case set of rotating left
          abs(gyro_z_bias())>200) // checks if rotating too fast
    {
        msleep(10); //changing this value requires a proportional change to theta's magic value
        pid_out = pid_calc(pid,angleDegrees,theta); 	// get the output of the pid
        pid_out = (-1<=pid_out && pid_out<=1) * pid_out + (1 < pid_out) - (pid_out <-1); // clamp pid_out within -1 - 1
        if( target_time > seconds() )									//test case if past acceleration time windOwO
            pid_out *= (seconds()<target_time) * (seconds()-start_time) /
            (target_time-start_time) + (seconds()>=target_time); 		//equation for reducing speed for acceleration

        inner_speed=speed*pid_out * inner.speed_modifier; 	// calculate inner speed
        mav(inner.port , inner_speed);					  	// set inner speed
        outter_speed=speed*pid_out * outter.speed_modifier; // calculate outter speed
        mav(outter.port, outter_speed);						// set outter speed

        theta += gyro_z_bias() * (0.00142/gyro_offset); 	// add change in rotation to theta
    }
    free(pid);			// free pid variable because it is a malloc-ed pointer
    freeze(inner.port);	// stop inner motor
    freeze(outter.port);// stop outter motor
    return; 			// return for good measure
}

void demo_gyro_drive(int speed, double distance)
{
    double theta=0;
    int sign_speed = sign(speed) * sign(distance);
    int reverse = sign(distance);
    double start = seconds();
    speed *= sign(distance);
    int desired_speed = speed;
    double start_time = seconds(), target_time = seconds() + abs(speed)*0.0003;
	
    distance = fabs(distance);
    while(seconds()-start< distance) {
        speed = desired_speed;
        if( target_time > seconds() ) {									//test case if past acceleration time window
            speed *= (seconds()<target_time)*(seconds()-start_time) /
            (target_time-start_time) + (seconds()>=target_time); 		//equation for reducing speed for acceleration
        }
		//printf("%f	",theta);
        mav(motors.left, speed - speed*theta*sign_speed/100000);
        mav(motors.right,speed + speed*theta*sign_speed/100000);
        msleep(10);
        theta += gyro_z_bias() * reverse;
    }
    freeze(motors.left);
    freeze(motors.right);
}

static int on_black(int port) {
    return analog(port)>irs.black;
}

void demo_gyro_square_up(int speed) {
    int theta=0, sign_speed = sign(speed);
    while(!on_black(irs.left) && !on_black(irs.right)) {
        mav(motors.left, speed - speed*theta*sign_speed/100000);
        mav(motors.right,speed + speed*theta*sign_speed/100000);
        msleep(10);
        theta += gyro_z_bias();
    }
    while(!on_black(irs.left) || !on_black(irs.right)) {
        mav(motors.left, on_black(irs.left) ? speed : -0.5*speed);
        mav(motors.right,on_black(irs.right)? speed : -0.5*speed);
        msleep(10);
    }
    freeze(motors.left);
    freeze(motors.right);
}
	int buffer(int sensor){
    int value = 0;
    int total = 0;
    int i = 0;
    while (i<5){
        i++;
        value = analog(sensor);
        msleep(3);
        total+=value;
    };
    total=total/5;
    return total;
};
int black_speed;
void square_up(int ending,int speed){
    int l_midpoint=2100;
    int r_midpoint=2150;
    if(speed > 0 && speed < 400){//failcheck for speed
        black_speed = speed;
    }
    else{black_speed = 1*speed;}
    if(ending == 1 || ending == 2){
        while(1){
            if(buffer(lIR)<l_midpoint && buffer(rIR)<r_midpoint){ //move to black
                move(speed,speed);
            }
            if(buffer(rIR)>r_midpoint){ //right wheel back if black
                move(speed,(speed*(-.83)));
            }
            if(buffer(lIR)>l_midpoint){//left wheel back if black
                move((speed*(-.53)),speed);
            }
            if(buffer(lIR)>l_midpoint && buffer(rIR)>r_midpoint) { //if both sensors on black, stop
                move(0,0);
                break;
            }
        }
    }

    switch(ending){
        case 1: //if ending on white:
            {
                while(1){

                    if(buffer(lIR)>l_midpoint && buffer(rIR)>r_midpoint){ //move forward if black
                        move(black_speed,black_speed);
                    }
                    if(buffer(lIR)<l_midpoint){ //left wheel back if left sensor is white
                        move(-0.33*black_speed,black_speed);
                    }
                    if(buffer(rIR)<r_midpoint){ //right wheel back if right sensor is white 

                        move(black_speed,-0.33*black_speed);
                    }
                    if(buffer(lIR)<l_midpoint && buffer(rIR)<r_midpoint){ //if white, stop
                        move(0,0);
                        break;
                    }
                }
            }
        case 2: //if ending on black:
            {
                while(1){
                    if(buffer(lIR)>l_midpoint && buffer(rIR)>r_midpoint){ //move backward if black
                        move(-1*black_speed,-1*black_speed);
                    }
                    if(buffer(lIR)<l_midpoint){ //right wheel backward if white
                        move(0,-.33*black_speed);
                    }
                    if(buffer(rIR)<r_midpoint){ //left wheel backward if white
                        move(-.33*black_speed,0);
                    }
                    if(buffer(lIR)<l_midpoint && buffer(rIR)<r_midpoint){ //if white, stop
                        move(0,0);
                        break;
                    }
                }
            }
    }
}
void move(int l_speed,int r_speed){//basic moving function thats based off mav
 
    //int    l_speed=65;
    // int r_speed=65;
    mav(MR,r_speed);
    mav(ML,l_speed);
}
