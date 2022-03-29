#include "ros/ros.h"
#include "std_msgs/Int8.h"
#include "std_msgs/Bool.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Range.h"
#include "geometry_msgs/Twist.h"
#include "std_msgs/Float32.h"
#include "nav_msgs/Odometry.h"
#define move_stop 2.5
#define frequency_odom_pub 50

std_msgs::Bool flag_AEB;
geometry_msgs::Twist cmd_vel_msg;
std_msgs::Float32 delta_range;
std_msgs::Float32 old_sonar_range;
nav_msgs::Odometry delta_pos,past_pos,pos;
geometry_msgs::Twist vel_msg;


float x,y;
float delta_x, delta_y;
float vx,vy;
float aeb_collision_distance=200;



void odomCallback(const nav_msgs::Odometry& msg){
		
		float old_x,old_y;
		old_x=x;
		old_y=y;
		
	ROS_INFO("%.2lf %.2lf",msg.pose.pose.position.x, msg.pose.pose.position.y);
	x=msg.pose.pose.position.x;
	y=msg.pose.pose.position.y;
	delta_x= x - old_x;
	delta_y= y - old_y;
	vx = delta_x * frequency_odom_pub;
	vy = delta_y * frequency_odom_pub;
	
	if(x >=move_stop)
	{
		 cmd_vel_msg.linear.x = 0;
	     ROS_INFO("Car STOP 6m.");
    }	
	
	
	
}

void UltraSonarCallback(const sensor_msgs::Range::ConstPtr& msg){
	
	ROS_INFO("sonar Seq: [%d]",msg->header.seq);
	ROS_INFO("sonar Range: [%f]",msg->range);
	
	aeb_collision_distance= vx *(0.7 + 0.1) *0.22778 *2.5; //1m/sec = 0.22778 km/h , 2.1 = dkswjsrPtn
	
	
	if(msg->range <= aeb_collision_distance){
		                   
		       ROS_INFO("AEB_Activation!!");
		       flag_AEB.data = true;		        
    }            		                	
		
	else{	
		    flag_AEB.data=false;
		}
		
		
		delta_range.data =old_sonar_range.data -msg->range;	
	    old_sonar_range.data= msg->range;
	
	}
	
	void UltraSonarCallback2(const sensor_msgs::Range::ConstPtr& msg){
	
	//ROS_INFO("sonar Seq: [%d]",msg->header.seq);
	//ROS_INFO("sonar Range: [%f]",msg->range);
	printf("test");
	
	if(msg->range <=2.0){
		ROS_INFO("AEB_Activation!!");
		flag_AEB.data = true;
		}
		else{
			
			flag_AEB.data=false;
			}
	
	}
	
	void CarControlCallback(const geometry_msgs::Twist& msg){
		
		//ROS_INFO("Cmd_vel : linear_x [%f]",msg.linear.x);
		cmd_vel_msg = msg;
		//ROS_INFO("cmd_vel : linear_x [%f]",cmd_vel_msg.linear.x);
		}

int main(int argc, char **argv){
	
	std::string odom_sub_topic="ackermann_steering_controller/odom";
	
	
	
	 old_sonar_range.data = 0;
	int count = 0;
	ros::init(argc, argv, "aeb_controller");
	
	ros::NodeHandle n;
	
	
	
	ros::Subscriber sub= n.subscribe("range",1000,UltraSonarCallback);
	ros::Subscriber sub2= n.subscribe("RangeSonar1",1000,UltraSonarCallback2);
	ros::Subscriber cmd_vel_sub = n.subscribe("/cmd_vel",10,&CarControlCallback);
	
	ros::Subscriber sub_odom = n.subscribe(odom_sub_topic,10,&odomCallback);
	
	ros::Publisher pub_cmd_vel = n.advertise<geometry_msgs::Twist>("/ackermann_steering_controller/cmd_vel",10);
	ros::Publisher pub_aeb_activation_flag = n.advertise<std_msgs::Bool>("/aeb_activation_flag",1);
	//ros::Publisher pub_delta_range = n.advertise<std_msgs::Float32>("delta_range",1);
	ros::Publisher pub_vel = n.advertise<nav_msgs::Odometry>("/velocity",10);
	
	
	
	ros::Rate loop_rate(2);
	
	
	while(ros::ok()){
		
		
		
		if((count%10)==0)
		{
			pub_aeb_activation_flag.publish(flag_AEB);
		}
		
		if(flag_AEB.data== true){
			
			if(cmd_vel_msg.linear.x>0) cmd_vel_msg.linear.x=0;
	         pub_cmd_vel.publish(cmd_vel_msg);
			}
			
			else{
				
			pub_cmd_vel.publish(cmd_vel_msg);
		}
		//collision_distance = vx*(1/10);
		
		ROS_INFO("Odom : [%6.3f %6.3f] m | Veloctiy : [%6.3f %6.3f] m/s",x,y,vx,vy);
		ROS_INFO("Collision Distance : %6.3f",aeb_collision_distance);
		
		
		loop_rate.sleep();
		ros::spinOnce();
		++count;
		}
		return 0;
	}
