/**
 * Author: Vamsi Kalagaturu
 *
 * Description: Library to handle basic utilities for the arm_actions package
 *
 * Copyright (c) [2023]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <array>
#include <iostream>
#include <string>
#include <vector>

#include "kdl/chain.hpp"
#include "kdl/frames.hpp"
#include "kdl/jacobian.hpp"
#include "kdl/jntarray.hpp"
#include "kdl/tree.hpp"
#include "kdl/chainfksolverpos_recursive.hpp"
#include "kdl/chainfksolvervel_recursive.hpp"
#include "kdl/chainhdsolver_vereshchagin.hpp"
#include "kdl_parser/kdl_parser.hpp"


struct Kinova {
  int nj;
  int ns;
  double* q;
  double* q_dot;
  double* q_ddot;
  double** s;
  double** s_dot;
  double** s_ddot;
};
//{ 0.0, 0.26, 0.0, 2.26, 0.0, -0.95, -1.57 }

/**
 * @brief Initializes the robot state struct.
 * @param num_joints The number of joints.
 * @param num_segments The number of segments.
 * @param rob [out] The robot state struct.
 */
void initialize_robot_state(int num_joints, int num_segments, Kinova* rob);

/**
 * @brief Initializes the robot state struct.
 * @param num_joints The number of joints.
 * @param num_segments The number of segments.
 * @param init_q The initial joint positions.
 * @param rob [out] The robot state struct.
 */
void initialize_robot_state(int num_joints, int num_segments, double* init_q, Kinova* rob);

/**
 * @brief Initializes the robot chain.
 * @param robot_urdf The path to the robot urdf.
 * @param base_link The name of the base link.
 * @param tool_link The name of the tool link.
 * @param robot_chain [out] The KDL chain representing the robot.
 */
void initialize_robot_chain(std::string robot_urdf,
                            std::string base_link,
                            std::string tool_link,
                            KDL::Chain& robot_chain);

/**
 * @brief Computes forward velocity kinematics.
 * @param link_name The name of the link.
 * @param rob Robot state struct.
 * @param robot_chain The KDL chain representing the robot.
 * @param out_twist [out] The twist of the link std::array<double, 6>.
 */
void computeForwardVelocityKinematics(std::string link_name,
                                      Kinova* rob,
                                      KDL::Chain* robot_chain,
                                      double *out_twist);
               
/**
 * @brief Adds two arrays.
 * @param arr1 The first array.
 * @param arr2 The second array.
 * @param result [out] The result of the addition.
 * @param size The size of the arrays.
 */
extern void add(double* arr1, double* arr2, double* result, size_t size);

/**
 * @brief Updates the q and q_dot values of the robot state struct.
 * @param q_ddot The joint accelerations.
 * @param dt The time step.
 * @param rob [in/out] The robot state struct.
 */
void updateQandQdot(double *q_ddot, double dt, Kinova* rob);

/**
 * @brief Solves the hybrid dynamics problem for a given robot chain.
 * @param rob The robot state struct.
 * @param robot_chain The KDL::Chain object representing the robot's kinematic chain.
 * @param num_constraints The number of constraints.
 * @param root_acceleration The root acceleration.
 * @param alpha The alpha matrix.
 * @param beta The beta vector.
 * @param ext_wrench The external wrenches.
 * @param tau_ff The feedforward torques.
 * @param predicted_acc [out] The output predicted joint accelerations.
 * @param constraint_tau [out] The output constraint torques.
 */
void achd_solver(Kinova *rob, KDL::Chain *chain, int num_constraints, 
                  double *root_acceleration,
                 double **alpha, double *beta,
                 double **ext_wrench, double *tau_ff,
                 double *predicted_acc, double *constraint_tau);

void getLinkIdFromChain(KDL::Chain &chain, std::string link_name, int &link_id);

#endif  // UTILS_HPP
