#ifndef TAG_H
#define TAG_H

#include <Eigen/Geometry> 
#include <geometry_msgs/Pose.h>

class Tag
{
public:
	//
	Eigen::Vector3d position_camera_frame_;
	Eigen::Vector3d position_drone_frame_;

    Eigen::Vector3d tag_to_camera_euler;

	Eigen::Quaternion<double> orientation_camera_frame_;
	Eigen::Quaternion<double> orientation_drone_frame_;

	Eigen::Vector3d to_landing_center_translation_;

	//Eigen::Isometry3d tag_to_camera_transformation_;
	//Eigen::Isometry3d camera_to_drone_transformation_;
	//Eigen::Isometry3d tag_to_drone_transformation_;
	Eigen::Matrix4d tag_to_camera_transformation;
	Eigen::Matrix4d camera_to_drone_transformation;
	Eigen::Matrix4d tag_to_drone_transformation;

	Eigen::Vector3d landing_center_position_;
	Eigen::Quaternion<double> landing_center_orientation_;
	bool found_;
	bool to_landing_center_translation_set_;

public:
	Tag();
	//void updateTagState(const Eigen::Vector3d pos, const Eigen::Quaternion<double> ori);
	void updateTagState(const geometry_msgs::Pose);
	double getYaw_incameraframe();
	
	void setToLandingCenterTranslation(const Eigen::Vector3d trans);
	void calculateDroneFramePosition(const Eigen::Matrix3d camera_to_drone_rotation);
	void calculateDroneFrameOrientation(const Eigen::Matrix3d camera_to_drone_rotation);
	bool isFound(){return found_;}
	void setMissing();
	void setFound();
	Eigen::Vector3d getLandingCenterPosition();
	Eigen::Quaternion<double> getLandingCenterOrientation();
	double getYawError();
};


#endif
