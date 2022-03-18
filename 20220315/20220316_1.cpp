#include "ros/ros.h"
#include "std_msgs/Int8.h"
#include "std_msgs/Bool.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Range.h"
#include "geometry_msgs/Twist.h"

std_msgs::Bool flag_AEB;
std_msgs::Bool flag_AEB2;

void UltraSonarCallback(const sensor_msgs::Range::ConstPtr& msg){
	
	ROS_INFO("sonar Seq: [%d]",msg->header.seq);
	ROS_INFO("sonar Range: [%f]",msg->range);
	
	if(msg->range <=1.0){
		ROS_INFO("AEB_Activation!!");
		flag_AEB.data = true;
		}
		else{
			
			flag_AEB.data=false;
			}
	
	}
	
	void UltraSonarCallback2(const sensor_msgs::Range::ConstPtr& msg2){
	
	
	
	if(msg2->range <=1.0){
		ROS_INFO("!!BOOM!!");
		
		flag_AEB2.data = true;
		}
		else{
			
			flag_AEB2.data=false;
			}
	
	}

	
	

int main(int argc, char **argv){
	
	int count = 0;
	ros::init(argc, argv, "aeb_controller");
	
	ros::NodeHandle n;
	
	ros::Rate loop_rate(10);
	
	ros::Subscriber sub= n.subscribe("range",1000,UltraSonarCallback);
	ros::Subscriber sub2= n.subscribe("range",1000,UltraSonarCallback2);
	while(ros::ok()){
		
		loop_rate.sleep();
		ros::spinOnce();
		++count;
		}
		return 0;
	}
