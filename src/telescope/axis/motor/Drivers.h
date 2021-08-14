#pragma once

typedef struct DriverOutputStatus {
  bool shortToGround;
  bool openLoad;
} DriverOutputStatus;

typedef struct DriverStatus {
  DriverOutputStatus outputA;
  DriverOutputStatus outputB;
  bool overTemperaturePreWarning;
  bool overTemperature;
  bool standstill;
  bool fault;
} DriverStatus;
