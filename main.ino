#include <Adafruit_MCP4725.h>

#define REC_PIN 2
#define MIC_PIN A3
#define KNOB_PIN A0
#define GRAIN_SIZE 4000

Adafruit_MCP4725 dac;

int grainLocWrite = 0;
int grainLocRead = 0;
int grain[GRAIN_SIZE];

bool recording = false;

void setup() {
  Serial.begin(9600);

  while (!Serial);

  dac = Adafruit_MCP4725();
  bool res = dac.begin(0x62);

  if (!res) {
    Serial.println("Error: Unable to connect to DAC");
  }
}

void loop() {
  bool currRecording = digitalRead(REC_PIN);

  if (!recording && currRecording) {
    grainLocWrite = 0;
  }

  recording = currRecording;

  if (recording) {
    int mic = analogRead(MIC_PIN);
    mic = mic * 4;

    grain[grainLocWrite] = mic;
    ++grainLocWrite;

    if (grainLocWrite == GRAIN_SIZE) grainLocWrite = 0;

    return;
  }

  int knobVal = analogRead(KNOB_PIN);
  float m = (float)knobVal / 1023.f;

  float y = m * ((float)grainLocRead / (float)GRAIN_SIZE) + (1 - m);

  dac.setVoltage(grain[grainLocRead] * y, false);

  ++grainLocRead;
  if (grainLocRead == GRAIN_SIZE) grainLocRead = 0;
}
