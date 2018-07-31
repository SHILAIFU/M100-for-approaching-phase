#include <ros/ros.h>
#include <ros/console.h>

#include <vector>
#include <iterator>
#include <string>

#include <dji_sdk/dji_sdk.h>
#include <dji_sdk/GimbalAngleControl.h>
#include <dji_sdk/GimbalSpeedControl.h>
#include <dji_sdk/VelocityControl.h>

#include <std_msgs/Float64.h>
#include <std_msgs/Bool.h>

#include <geometry_msgs/PoseArray.h>
#include <cmath>
#include <iostream>

#include <Eigen/Geometry> 

ros::Subscriber velocity_control_x_sub;
ros::Subscriber velocity_control_y_sub;
ros::Subscriber velocity_control_yaw_sub;

ros::ServiceClient velocity_control_service;
ros::ServiceClient sdk_permission_control_service;
ros::ServiceClient drone_task_control_service;

double velocity_control_effort_x=0;
double velocity_control_effort_y=0;
double velocity_control_effort_yaw=0;

void velocityControlEffortXCallback(std_msgs::Float64 velocity_control_effort_x_msg)
{
	velocity_control_effort_x = velocity_control_effort_x_msg.data;
}

void velocityControlEffortYCallback(std_msgs::Float64 velocity_control_effort_y_msg)
{
	velocity_control_effort_y = velocity_control_effort_y_msg.data;
}

void velocityControlEffortYawCallback(std_msgs::Float64 velocity_control_effort_yaw_msg)
{
	velocity_control_effort_yaw = velocity_control_effort_yaw_msg.data;
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "m100_track_velocity_controller_test");
	ros::NodeHandle nh;

	velocity_control_x_sub = nh.subscribe("/m100_track/velocity_control_effort_x", 10, velocityControlEffortXCallback);
	velocity_control_y_sub = nh.subscribe("/m100_track/velocity_control_effort_y", 10, velocityControlEffortYCallback);
	velocity_control_yaw_sub = nh.subscribe("/m100_track/velocity_control_effort_yaw", 10, velocityControlEffortYawCallback);

	velocity_control_service = nh.serviceClient<dji_sdk::VelocityControl>("dji_sdk/velocity_control");
	sdk_permission_control_service = nh.serviceClient<dji_sdk::SDKPermissionControl>("dji_sdk/sdk_permission_control");
	drone_task_control_service = nh.serviceClient<dji_sdk::DroneTaskControl>("dji_sdk/drone_task_control");

	dji_sdk::VelocityControl velocity_control;

	int frame = 1;//0->body frame;1->world frame
	double yaw_rate = 0;

	dji_sdk::SDKPermissionControl sdk_permission_control;
	sdk_permission_control.request.control_enable = 1;
	bool control_requested = false;

	while(!(sdk_permission_control_service.call(sdk_permission_control) && sdk_permission_control.response.result))
	{
		ROS_ERROR("Velocity controller: request control failed!");
	}
	
	dji_sdk::DroneTaskControl drone_task_control;
	drone_task_control.request.task = 4;
	while(!(drone_task_control_service.call(drone_task_control) && drone_task_control.response.result))
	{
		ROS_ERROR("take off failed!");
	}
	ROS_INFO("take off successfully");

	while(ros::ok())
	{
		ros::spinOnce();

		ROS_DEBUG_THROTTLE(3, "Velocity controller: Received control effort, flying the drone");
		velocity_control.request.frame = frame;
		velocity_control.request.vx = velocity_control_effort_x;
		velocity_control.request.vy = velocity_control_effort_y;
		velocity_control.request.vz = 0;
		velocity_control.request.yawRate = velocity_control_effort_yaw;

		if(!(velocity_control_service.call(velocity_control) && velocity_control.response.result))
		{
			ROS_ERROR("Velocity controller: velocity control failed!");
		}
		else
		ROS_INFO_ONCE("velocity control successfully");			
	}

	sdk_permission_control.request.control_enable = 0;
	sdk_permission_control_service.call(sdk_permission_control);
}
