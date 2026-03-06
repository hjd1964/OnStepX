// bluetooth manager
#include "BluetoothManager.h"

#if SERIAL_BT_MODE == MASTER

#include "../nv/Nv.h"

bool BluetoothManager::init() {
  if (!active) {
    readSettings();

    setStation(stationNumber);

    VF("MSG: Bluetooth, init device ");
    VF(sta->address); DF(" "); VF(sta->host);

    int channel = 0;
    BTAddress address = BTAddress(sta->address);
    std::map<int, std::string> channels = SERIAL_BT.getChannels(address);
    if (channels.size() > 0) { channel = channels.begin()->first; }
    VF(" on ch "); V(channel);

    if (strlen(sta->passkey) > 0) {
      VF(" w/passkey "); V(bluetoothManager.sta->passkey);
      SERIAL_BT.setPin(bluetoothManager.sta->passkey);
    }

    VF("...");
    if (channel != 0 && SERIAL_BT.connect(address, channel, ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE)) {
      VLF(" success");
      active = true;
    } else {
      VLF(" failed");
    }
  }

  return active;
}

void BluetoothManager::setStation(int number) {
  if (number >= 1 && number <= BluetoothStationCount) stationNumber = number;
  sta = &settings.station[stationNumber - 1];
}

void BluetoothManager::disconnect() {
  active = false;
  VLF("MSG: Bluetooth, disconnected");      
}

void BluetoothManager::readSettings() {
  if (settingsReady) return;

  #ifdef NV_BT_SETTINGS
  if (!nv().kv().getOrInit("BT_SETTINGS", settings)) { DLF("WRN: Nv, init failed for BT_SETTINGS"); }
  #endif

  VF("MSG: Bluetooth, Master Pwd = "); VL(settings.masterPassword);

  VF("MSG: Bluetooth, Sta Select = "); VL(stationNumber);

  int currentStationNumber = stationNumber;

  for (int station = 1; station <= BluetoothStationCount; station++) {
    setStation(station);
    VF("MSG: Bluetooth, Sta"); V(station); VF(" NAME  = "); VL(sta->host);
    VF("MSG: Bluetooth, Sta"); V(station); VF(" MAC   = "); VL(sta->address);
    VF("MSG: Bluetooth, Sta"); V(station); VF(" KEY   = "); VL(sta->passkey);
  }

  stationNumber = currentStationNumber;
  
  settingsReady = true;
}

void BluetoothManager::writeSettings() {
  if (!settingsReady) return;
  
  #ifdef NV_BT_SETTINGS
    VLF("MSG: BluetoothManager, writing settings to NV");
    nv().kv().put("BT_SETTINGS", settings);
  #endif
}

BluetoothManager bluetoothManager;

#endif
