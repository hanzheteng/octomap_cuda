#include "ros/ros.h"
#include "sensor_msgs/PointCloud2.h"
#include "std_msgs/Float32.h"

//Since we already know the laserscan used has fixed size, we will not bother using dynamic size
#define SIZE 511

float *average3(int num, float *in1, float *in2, float *in3, float *out);
float in1[SIZE], in2[SIZE], in3[SIZE], out[SIZE];
sensor_msgs::PointCloud2 msg_pcl;

void pointcloudCallback(const sensor_msgs::PointCloud2& msg)
{		
	msg_pcl=msg;
	// Cycle vectors (in3=in2, in2=in1, in1=new)
	for(int i=0;i<SIZE;i++)
	{
		in3[i]=in2[i];
		in2[i]=in1[i];
		in1[i]=msg.data[i];
	}
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "octomap_cuda");	  
	ros::NodeHandle n;
	ros::Publisher pub = n.advertise<sensor_msgs::PointCloud2>("/points/filtered", 1);
	ros::Subscriber sub = n.subscribe("/camera/depth/points", 100, pointcloudCallback);

	// Initializes the vectors with zeros
	for (int i = 0; i < SIZE; ++i)
	{
		in1[i]=in2[i]=in3[i]=out[i]=0;
	}
	while(ros::ok())
	{   // Get new message and perform average
		ros::spinOnce();
		average3(SIZE, in1, in2, in3, out);
		// Assign frame_id and ranges size to be able to publish and visualize topic
		//msg_pcl.header.frame_id="camera_rgb_optical_frame";
		msg_pcl.data.resize(511);
		// Assign values
		for(int i=0;i<SIZE;i++)
		{
			msg_pcl.data[i]=out[i];	
		}
		pub.publish(msg_pcl);
	}
	return 0;
}
