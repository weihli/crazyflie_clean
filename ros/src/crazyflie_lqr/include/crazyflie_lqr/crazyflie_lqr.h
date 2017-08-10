/*
 * Copyright (c) 2017, The Regents of the University of California (Regents).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *    3. Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Please contact the author(s) of this library if you have any questions.
 * Authors: David Fridovich-Keil   ( dfk@eecs.berkeley.edu )
 */

///////////////////////////////////////////////////////////////////////////////
//
// LQR controller for the Crazyflie. Uses an LQR control matrix for hovering
// at each specified reference point.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CRAZYFLIE_LQR_CRAZYFLIE_LQR_H
#define CRAZYFLIE_LQR_CRAZYFLIE_LQR_H

#include <crazyflie_lqr/types.h>
#include <crazyflie_msgs/StateStamped.h>
#include <crazyflie_msgs/ControlStamped.h>

#include <ros/ros.h>
#include <math.h>
#include <fstream>

class CrazyflieLQR {
public:
  ~CrazyflieLQR() {}
  CrazyflieLQR()
    : K_(MatrixXd::Zero(7, 12)),
      u_ref_(VectorXd::Zero(7)),
      x_ref_(VectorXd::Zero(12)) {}

  // Initialize this class by reading parameters and loading callbacks.
  bool Initialize(const ros::NodeHandle& n);

private:
  // Load parameters and register callbacks.
  bool LoadParameters(const ros::NodeHandle& n);
  bool RegisterCallbacks(const ros::NodeHandle& n);

  // Process an incoming reference point.
  void ReferenceCallback(const crazyflie_msgs::StateStamped::ConstPtr& msg);

  // Process an incoming state measurement.
  void StateCallback(const crazyflie_msgs::StateStamped::ConstPtr& msg);

  // Static helper functions for angle wrapping and angle conversions.
  static inline double DegreesToRadians(double d) { return d * M_PI / 180.0; }
  static inline double RadiansToDegrees(double r) { return r * 180.0 / M_PI; }
  static inline double WrapAngleDegrees(double d) {
    d = std::fmod(d + 180.0, 360.0) - 180.0;

    if (d < -180.0)
      d += 180.0;

    return d;
  }
  static inline double WrapAngleRadians(double r) {
    r = std::fmod(r + M_PI, 2.0 * M_PI) - M_PI;

    if (r < -M_PI)
      r += 2.0 * M_PI;

    return r;
  }

  // Publishers and subscribers.
  ros::Subscriber state_sub_;
  ros::Subscriber reference_sub_;
  ros::Publisher control_pub_;

  std::string state_topic_;
  std::string reference_topic_;
  std::string control_topic_;

  // K matrix and reference state/control (to fight gravity). These are
  // hard-coded since they will not change.
  MatrixXd K_;
  VectorXd u_ref_;
  VectorXd x_ref_;

  std::string K_filename_;
  std::string u_ref_filename_;
  std::string x_ref_filename_;

  // Dimensions of control and state spaces.
  static const size_t U_DIM;
  static const size_t X_DIM;

  // Initialized flag and name.
  bool initialized_;
  std::string name_;
}; //\class CrazyflieLQR

#endif
