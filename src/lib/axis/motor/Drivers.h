#pragma once

typedef struct DriverOutputStatus {
  bool shortToGround;
  bool openLoad;
} DriverOutputStatus;

typedef struct DriverStatus {
  bool active;
  DriverOutputStatus outputA;
  DriverOutputStatus outputB;
  bool overTemperatureWarning;
  bool overTemperature;
  bool standstill;
  bool fault;
} DriverStatus;
