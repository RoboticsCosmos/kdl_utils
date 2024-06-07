#ifndef LOG_STRUCTS_HPP
#define LOG_STRUCTS_HPP

#include <vector>
#include <iostream>
#include <cstring>

#include "motion_spec_utils/robot_structs.hpp"

struct LogControlData
{
  double measured_value;
  double reference_value;

  double control_singal;

  void populate(double measured_value, double reference_value, double control_signal)
  {
    this->measured_value = measured_value;
    this->reference_value = reference_value;
    this->control_singal = control_signal;
  }
};

struct LogControlDataVector
{
  const char *control_variable;
  std::vector<LogControlData> control_data;

  const char *log_dir;
  char *filename;
  FILE *file;

  // constructor
  LogControlDataVector(const char *control_variable, const char *log_dir)
  {
    this->control_variable = control_variable;
    this->log_dir = log_dir;

    // create the filename
    sprintf(this->filename, "%s/control_log_%s.csv", log_dir, control_variable);

    // create the file
    this->file = fopen(this->filename, "w");
    if (this->file == NULL)
    {
      std::cerr << "Error opening file: " << this->filename << std::endl;
      exit(1);
    }

    // write the header
    fprintf(file, "Reference Value,Measured Value,Control Signal\n");
  }

  // destructor
  ~LogControlDataVector()
  {
    fclose(this->file);
  }

  void addControlData(double measured_value, double reference_value, double control_signal)
  {
    LogControlData data;
    data.populate(measured_value, reference_value, control_signal);
    this->control_data.push_back(data);

    if (this->control_data.size() >= 100)
    {
      writeToOpenFile();
    }
  }

  void writeToOpenFile()
  {
    for (size_t i = 0; i < this->control_data.size(); i++)
    {
      fprintf(file, "%f,%f,%f\n", this->control_data[i].reference_value,
              this->control_data[i].measured_value, this->control_data[i].control_singal);
    }
    // clear the data
    this->control_data.clear();
  }
};

struct LogManipulatorData
{
  // kinova info
  // double q[7];
  // double q_dot[7];
  double f_tool_measured[6]{};
  double tool_pose[6]{};
  double tool_twist[6]{};
  // double tool_acc_twist[6];

  // elbow
  double elbow_pose[6]{};
  double elbow_twist[6]{};

  // achd info
  double beta[6]{};
  double tau_command[7]{};
  double f_tool_command[6]{};
  // double q_ddot[7]{};

  // add methods to populate the data
  void populateManipulatorData(Manipulator<kinova_mediator> *rob)
  {
    // std::memcpy(this->q, rob->state->q, sizeof(this->q));
    // std::memcpy(this->q_dot, rob->state->q_dot, sizeof(this->q_dot));

    std::memcpy(this->f_tool_measured, rob->state->f_tool_measured, sizeof(this->f_tool_measured));
    std::memcpy(this->tool_pose, rob->state->s[rob->state->ns - 1], sizeof(this->tool_pose));
    std::memcpy(this->tool_twist, rob->state->s_dot[rob->state->ns - 1], sizeof(this->tool_twist));

    // elbow
    std::memcpy(this->elbow_pose, rob->state->s[rob->state->ns - 4], sizeof(this->elbow_pose));
    std::memcpy(this->elbow_twist, rob->state->s_dot[rob->state->ns - 4],
                sizeof(this->elbow_twist));
  }

  void populateAchdData(double *beta, double *tau_command, double *f_tool_command, double *q_ddot)
  {
    std::memcpy(this->tau_command, tau_command, sizeof(this->tau_command));

    if (beta != nullptr)
    {
      std::memcpy(this->beta, beta, sizeof(this->beta));
    }

    // if (q_ddot != nullptr)
    // {
    //   std::memcpy(this->q_ddot, q_ddot, sizeof(this->q_ddot));
    // }

    if (f_tool_command != nullptr)
    {
      std::memcpy(this->f_tool_command, f_tool_command, sizeof(this->f_tool_command));
    }
  }

  void populate(Manipulator<kinova_mediator> *rob, double *beta, double *tau_command,
                double *f_tool_command, double *q_ddot)
  {
    populateManipulatorData(rob);
    populateAchdData(beta, tau_command, f_tool_command, q_ddot);
  }
};

struct LogMobileBaseData
{
  // mobile base info
  double pivot_angles[4];
  double platform_force[3];
  double tau_command[8];

  double x_platform[3];
  double xd_platform[3];

  // add methods to populate the data
  void populateMobileBaseData(RobileBase *rob)
  {
    std::memcpy(this->pivot_angles, rob->state->pivot_angles, sizeof(this->pivot_angles));
  }

  void setPlatformData(double *x_platform, double *xd_platform)
  {
    std::memcpy(this->x_platform, x_platform, sizeof(this->x_platform));
    std::memcpy(this->xd_platform, xd_platform, sizeof(this->xd_platform));
  }

  void populateSolverData(double *platform_force, double *tau_command)
  {
    std::memcpy(this->platform_force, platform_force, sizeof(this->platform_force));
    std::memcpy(this->tau_command, tau_command, sizeof(this->tau_command));
  }
};

struct LogManipulatorDataVector
{
  const char *arm_name;
  std::vector<LogManipulatorData> log_data;

  const char *log_dir;
  char *filename;
  FILE *file;

  // constructor
  LogManipulatorDataVector(const char *arm_name, const char *log_dir)
  {
    this->log_dir = log_dir;
    this->arm_name = arm_name;

    // create the filename
    sprintf(this->filename, "%s/%s_manipulator_log.csv", log_dir, arm_name);

    // create the file
    this->file = fopen(this->filename, "w");
    if (this->file == NULL)
    {
      std::cerr << "Error opening file: " << this->filename << std::endl;
      exit(1);
    }

    // write the header
    fprintf(file,
            "ee_s_x,ee_s_y,ee_s_z,ee_s_qx,ee_s_qy,ee_s_qz,ee_s_qw,"
            "ee_twist_x,ee_twist_y,ee_twist_z,ee_twist_qx,ee_twist_qy,ee_twist_qz"
            "elbow_s_x,elbow_s_y,elbow_s_z,elbow_s_qx,elbow_s_qy,elbow_s_qz,elbow_s_qw,"
            "ee_f_e_x, ee_f_e_y, ee_f_e_z, ee_f_e_qx, ee_f_e_qy, ee_f_e_qz,"
            "ee_beta_x,ee_beta_y,ee_beta_z,ee_beta_qx,ee_beta_qy,ee_beta_qz,"
            "tau_c_1,tau_c_2,tau_c_3,tau_c_4,tau_c_5,tau_c_6,tau_c_7,"
            "ee_f_c_x,ee_f_c_y,ee_f_c_z,ee_f_c_qx,ee_f_c_qy,ee_f_c_qz\n");
  }

  // destructor
  ~LogManipulatorDataVector()
  {
    fclose(this->file);
  }

  void addManipulatorData(Manipulator<kinova_mediator> *rob, double *beta, double *tau_command,
                          double *f_tool_command, double *q_ddot)
  {
    LogManipulatorData data;
    data.populate(rob, beta, tau_command, f_tool_command, q_ddot);
    this->log_data.push_back(data);

    if (this->log_data.size() >= 100)
    {
      writeToOpenFile();
    }
  }

  void writeToOpenFile()
  {
    for (size_t i = 0; i < this->log_data.size(); i++)
    {
      // append all the data to a string
      std::stringstream ss;
      appendArrayToStream(ss, this->log_data[i].tool_pose, 6);
      appendArrayToStream(ss, this->log_data[i].tool_twist, 6);
      appendArrayToStream(ss, this->log_data[i].elbow_pose, 6);
      appendArrayToStream(ss, this->log_data[i].elbow_twist, 6);
      appendArrayToStream(ss, this->log_data[i].f_tool_measured, 6);
      appendArrayToStream(ss, this->log_data[i].beta, 6);
      appendArrayToStream(ss, this->log_data[i].tau_command, 7);
      appendArrayToStream(ss, this->log_data[i].f_tool_command, 6);

      // write the string to the file
      fprintf(file, "%s\n", ss.str().c_str());
    }
    // clear the data
    this->log_data.clear();
  }
};

struct LogMobileBaseDataVector
{
  std::vector<LogMobileBaseData> log_data;

  const char *log_dir;
  char *filename;
  FILE *file;

  // constructor
  LogMobileBaseDataVector(const char *log_dir)
  {
    this->log_dir = log_dir;

    // create the filename
    sprintf(this->filename, "%s/mobile_base_log.csv", log_dir);

    // create the file
    this->file = fopen(this->filename, "w");
    if (this->file == NULL)
    {
      std::cerr << "Error opening file: " << this->filename << std::endl;
      exit(1);
    }

    // write the header
    fprintf(
        file,
        "pivot_1,pivot_2,pivot_3,pivot_4,platform_force_x,platform_force_y,platform_force_z,"
        "tau_c_1,tau_c_2,tau_c_3,tau_c_4,tau_c_5,tau_c_6,tau_c_7,tau_c_8,"
        "x_platform_x,x_platform_y,x_platform_qz,xd_platform_x,xd_platform_y,xd_platform_qz\n");
  }

  // destructor
  ~LogMobileBaseDataVector()
  {
    fclose(this->file);
  }

  void addMobileBaseData(RobileBase *rob, double *x_platform, double *xd_platform)
  {
    LogMobileBaseData data;
    data.populateMobileBaseData(rob);
    data.setPlatformData(x_platform, xd_platform);
    this->log_data.push_back(data);

    if (this->log_data.size() >= 100)
    {
      writeToOpenFile();
    }
  }

  void writeToOpenFile()
  {
    for (size_t i = 0; i < this->log_data.size(); i++)
    {
      // append all the data to a string
      std::stringstream ss;
      appendArrayToStream(ss, this->log_data[i].pivot_angles, 4);
      appendArrayToStream(ss, this->log_data[i].platform_force, 3);
      appendArrayToStream(ss, this->log_data[i].tau_command, 8);
      appendArrayToStream(ss, this->log_data[i].x_platform, 3);
      appendArrayToStream(ss, this->log_data[i].xd_platform, 3);

      // write the string to the file
      fprintf(file, "%s\n", ss.str().c_str());
    }
    // clear the data
    this->log_data.clear();
  }
};

void appendArrayToStream(std::stringstream &ss, double *arr, size_t size)
{
  for (size_t i = 0; i < size; i++)
  {
    ss << arr[i] << ",";
  }
}

#endif  // LOG_STRUCTS_HPP