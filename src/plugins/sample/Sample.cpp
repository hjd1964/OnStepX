// Sample plugin

#include "Sample.h"
#include "../../Common.h"
#include "../../lib/serial/Serial_Local.h"
#include "../../lib/tasks/OnTask.h"

void sampleWrapper() { sample.loop(); }

void Sample::init() {
  VLF("MSG: Plugins, starting: sample");

  // start a task that runs twice a second, run at priority level 7 so
  // we can block using tasks.yield(); fairly aggressively without significant impact on operation
  tasks.add(500, 0, true, 7, sampleWrapper);
}

void Sample::loop() {
  SERIAL_LOCAL.transmit(":GR#");
  // let OnStepX run for 0.1 second to process the command
  tasks.yield(100);
  Serial.print("RA = ");
  Serial.println(SERIAL_LOCAL.receive());

  SERIAL_LOCAL.transmit(":GD#");
  tasks.yield(100);
  Serial.print("Dec=");
  Serial.println(SERIAL_LOCAL.receive());

  Serial.println();
}

Sample sample;
