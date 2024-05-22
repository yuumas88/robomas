#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include <cmath>
#include "robomas_plugins/msg/robomas_target.hpp"
#include "robomas_plugins/msg/robomas_frame.hpp"

robomas_plugins::msg::RobomasFrame generate_robomas_mode(const uint8_t id, const uint8_t mode)
{  
  robomas_plugins::msg::RobomasFrame frame;
  frame.motor = id;
  frame.mode = mode;

  return frame;
}

robomas_plugins::msg::RobomasTarget generate_robomas_target(const float data)
{ 
  robomas_plugins::msg::RobomasTarget frame;
  frame.target = data;

  return frame;
}

class RobomasPublisher : public rclcpp::Node
{
  public:
    RobomasPublisher()
    : Node("robomas_node")
    {
      publisher_target1 = this->create_publisher<robomas_plugins::msg::RobomasTarget>("robomas_target", 10);
      publisher_target2 = this->create_publisher<robomas_plugins::msg::RobomasTarget>("robomas_target", 10);
      publisher_target3 = this->create_publisher<robomas_plugins::msg::RobomasTarget>("robomas_target", 10);
      publisher_target4 = this->create_publisher<robomas_plugins::msg::RobomasTarget>("robomas_target", 10);
      publisher_frame = this->create_publisher<robomas_plugins::msg::RobomasFrame>("robomas_can_tx", 10);
      joy_subscriber_ = this->create_subscription<sensor_msgs::msg::Joy>(
        "joy", 10, std::bind(&RobomasPublisher::joy_callback, this, std::placeholders::_1)
      );
    }

  private:
    void joy_callback(const sensor_msgs::msg::Joy::SharedPtr joy_msg)
    {
      if (joy_msg->buttons[0] == 1){      
        publisher_frame->publish(generate_robomas_mode(0x08, 1));
      }

      if (joy_msg->buttons[1] == 1){      
        publisher_frame->publish(generate_robomas_mode(0x08, 0));
      }

      float x = -(joy_msg->axes[4]);
      float y = (joy_msg->axes[5]);

      publisher_target1->publish(generate_robomas_target(x + y));
      publisher_target2->publish(generate_robomas_target(-x + y));
      publisher_target3->publish(generate_robomas_target(-(x + y)));
      publisher_target4->publish(generate_robomas_target(x - y ));
    }

    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_subscriber_;
    rclcpp::Publisher<robomas_plugins::msg::RobomasTarget>::SharedPtr publisher_target1;
    rclcpp::Publisher<robomas_plugins::msg::RobomasTarget>::SharedPtr publisher_target2;
    rclcpp::Publisher<robomas_plugins::msg::RobomasTarget>::SharedPtr publisher_target3;
    rclcpp::Publisher<robomas_plugins::msg::RobomasTarget>::SharedPtr publisher_target4;
    rclcpp::Publisher<robomas_plugins::msg::RobomasFrame>::SharedPtr publisher_frame;

};


int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<RobomasPublisher>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}
