#include "static_avoidance.h"

namespace static_avoidance{

StaticAvoidance::StaticAvoidance():nh_(){
	initSetup();
}

StaticAvoidance::StaticAvoidance(ros::NodeHandle nh):nh_(nh){
	initSetup();
}

StaticAvoidance::void initSetup(){
	turn_left_flag = false;
	turn_right_flag = false;
	return_left_flag = false;
	return_right_flag = false;
	end_flag = false;
	sequence = 0;
}

void StaticAvoidance::obstacle_cb(const obstacle_detector::Obstacles data) {
  geometry_msgs::Point c, mycar;
  bool flag = false;
  int speed = CONSTANT_VEL;
  int steer = 1500;


  // Select nearest point and assign it to 'c'
  for(int i = 0; i < data.circles.size(); i++) {
	
    if( (data.circles[i].radius >= OBSTACLE_RADIUS) && (sqrt(data.circles[i].center.x * data.circles[i].center.x + data.circles[i].center.y * data.circles[i].center.y)  <= DETECT_DISTANCE)) {
        flag = true;
        c = data.circles[i].center;

	//c.y is lateral axis. so if c.y > 0 means the obstacles are on the left.
	if(c.y < 0){
	  sequence = 1;
	}
      	else{
          sequence = 2;
      	}

    }
  }
  

  double distance = sqrt(c.x * c.x + c.y * c.y);
// c.x is longitudinal axis. so if c.x >0 means the obstacles are in the rear.
     if(flag == 1){
      //avoidance right Obstacles
      if(sequence == 1){
        if(c.x > 0.02){
          turn_left_flag = true;
          return_right_flag = false;
	  end_flag = false;
        }
        else{
          return_right_flag = true;
          turn_left_flag = false;
	  end_flag = false;
        }
      }
      else if(sequence == 2){
        if(c.x > 0.02){
          turn_right_flag = true;
          return_left_flag = false;
	  end_flag = false;
        }
        else{
          return_left_flag = true;
          turn_right_flag = false;
	  end_flag = false;
         }
      }
    }
    else if(flag == 0){
	end_flag = true;
	steer = 1500;
	speed = 1500;
    }
    
    
    if(turn_left_flag){
        steer = CONSTANT_STEER - ((1 - distance) * 500);
        //turn_left_flag = false;
    }
    if(return_right_flag){
        steer = CONSTANT_STEER + (distance * 550);
        return_right_flag = false;
    }
    if(turn_right_flag){
        steer = CONSTANT_STEER + ((1 - distance) * 500);
        //turn_right_flag = false;
    }
    if(return_left_flag){
        steer = CONSTANT_STEER - (distance * 550);
        return_left_flag = false;
    }


 
 // ackermann_msgs::AckermannDriveStamped msg;
  std_msgs::String msg;
  ROS_INFO("distance: %f", distance);
  ROS_INFO("sequence : %d", sequence);
  ROS_INFO("c.x : %f", c.x);
  ROS_INFO("c.y : %f", c.y);
  ROS_INFO("left turn flag : %d", turn_left_flag);
  ROS_INFO("right turn flag : %d", turn_right_flag);
  ROS_INFO("end flag : %d",end_flag);
  ROS_INFO("flag : %d",flag);
  ROS_INFO("Steer : %d", steer);
  ROS_INFO("Speed : %d", speed);
  ROS_INFO("-----------------------------------------");
    
  
  //std::string steer_speed = std::to_string(steer) + std::to_string(speed);
  msg.data = std::to_string(steer) + "," + std::to_string(speed) + "," ;
  // msg.drive.steering_angle = steer;
  // msg.drive.speed = speed;
  
   pub.publish(msg);
}

} //end namespace
