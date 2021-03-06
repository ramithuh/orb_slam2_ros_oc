/*

*/

#ifndef ORBSLAM2_PROJECTMAP_H_
#define ORBSLAM2_PROJECTMAP_H_

// standard
#include <vector>
#include <string>
#include <utility>
#include <iostream>

// ROS
#include <ros/ros.h>
#include "geometry_msgs/PoseArray.h"
#include "geometry_msgs/PoseStamped.h"
#include "nav_msgs/OccupancyGrid.h"

// ORB_SLAM
#include "System.h"
#include "MapPoint.h"
#include "Converter.h"

// thirdparty
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/highgui/highgui.hpp>


namespace ProjectMap
{
    class Pub
    {
    public:
        Pub (ORB_SLAM2::System* orb_slam_ptr, ros::NodeHandle &node_handle);
        ~Pub (){ };

        void GetAndPublishMsgs ();

    protected:
        ORB_SLAM2::System* slam_ptr_;

    private:
        geometry_msgs::PoseArray GetAllKfsPts ();
        geometry_msgs::PoseArray GetSingleKfPts ();
        geometry_msgs::Pose TransformFromMapPt_orb2ros (const cv::Mat translation_);
        geometry_msgs::Pose TransformFromCamera_orb2ros (const cv::Mat translation_, cv::Mat rotation_);

        ros::NodeHandle node_handle_;
        ros::Publisher all_kfs_pts_publisher_;
        ros::Publisher single_kf_pts_publisher_;
        std::string name_of_node_;

        bool pub_all_pts_ = false;
        unsigned int all_pts_pub_gap_ = 30;
        unsigned int pub_count_ = 0;
        unsigned int min_observations_per_point_ = 2;

        cv::Mat orb2ros = (cv::Mat_<float>(3,3) << 0,0,1,-1,0,0,0,-1,0);
    };

    class Map
    {
    public:
        Map (ros::NodeHandle &node_handle);
        ~Map () { };

        void KeyboardUI ();

    private:

        void GetROSParameter ();
        void SetGridParameter ();
        void CreateCvMat (const unsigned int h, const unsigned int w);
        void PublishTopic (const ros::Publisher &pub, nav_msgs::OccupancyGrid &msg);
        void SingleCallback (const geometry_msgs::PoseArray::ConstPtr& kf_pts_array);
        void AllCallback (const geometry_msgs::PoseArray::ConstPtr& kfs_pts_array);
        void UpdateGridMap (const geometry_msgs::PoseArray::ConstPtr& kf_pts_array);
        void ProcessPts (const std::vector<geometry_msgs::Pose> &pts, unsigned int n_pts, unsigned int start_id);
        void ProcessPt (const geometry_msgs::Point &curr_pt, cv::Mat &occupied, cv::Mat &visited, cv::Mat &pt_mask);
        void GetGridMap ();
        void ResetGridMap(const geometry_msgs::PoseArray::ConstPtr& kfs_pts_array);
        void SetGridOrigin (nav_msgs::OccupancyGrid &grid, float &grid_min_x, float &grid_min_y);

        float scale_fac_;
        //float resize_fac_ = 1; // removed
        // cloud_min_x, cloud_max_x, cloud_min_y, cloud_max_y
        float cloud_lim_[4];
        float free_thresh_;
        float occupied_thresh_;
        unsigned int visit_thresh_;
        bool use_local_counter_ = false;
        float thresh_diff_ = 0.01;

        // grid_min_x, grid_max_x, grid_min_y, grid_max_y
        float grid_lim_[4];
        cv::Mat global_occupied_counter_, global_visit_counter_;
        cv::Mat local_occupied_counter_, local_visit_counter_;
        cv::Mat local_map_pt_mask_;
        cv::Mat grid_map_;                 // [0.0, 1.0], to compute free/occupied probability
        cv::Mat grid_map_int_;             // [0, 100], free/occupied probability normalized to integer 100
        cv::Mat grid_map_thresh_;          // for visualization, uint8 : 0, 128, 255 -> black, gray, white
        //cv::Mat grid_map_thresh_resized_;
        float kf_pos_x_, kf_pos_y_;
        int kf_pos_grid_x_, kf_pos_grid_y_;

        // x, y
        float norm_fac_[2];
        unsigned int h_, w_;

        bool loop_closure_being_processed_ = false;
        unsigned int n_kf_received_ = 0;

        ros::NodeHandle node_handle_;
        ros::Publisher grid_map_cost_publisher_;
        ros::Publisher grid_map_visual_publisher_;
        ros::Subscriber all_kfs_pts_subscriber_;
        ros::Subscriber single_kf_pts_subscriber_;
        nav_msgs::OccupancyGrid grid_map_cost_msg_;
        nav_msgs::OccupancyGrid grid_map_visual_msg_;
        std::string name_of_node_;

        std::string all_kfs_pts_topic_param_;
        std::string single_kf_pts_topic_param_;
        std::string frame_id_param_;
        bool publish_grid_map_cost_param_;
        bool publish_grid_map_visual_param_;
        bool use_keyboardUI_param_;
        double free_thresh_param_;
        double occupied_thresh_param_;
        double scale_factor_param_;
        double cloud_min_x_param_;
        double cloud_max_x_param_;
        double cloud_min_y_param_;
        double cloud_max_y_param_;
        int visit_thresh_param_;

        unsigned int map_count_ = 0;
    };
}
#endif
