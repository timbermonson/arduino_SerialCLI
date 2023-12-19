#include "SerialCLI.h"

// My CLI functions to be triggered
void ping(const char** params) {
  Serial.print(F("Pong! Millis since bootup: "));
  Serial.println(millis());
}

void getName(const char** params) {
  Serial.println(F("I'm an arduino!"));
}

void doubler(const char** params) {
  long inputNum = strtol(params[0], NULL, 10);  //parse a string input to a number

  Serial.print(F("Your input, doubled, equals: "));
  Serial.println(inputNum * 2);
}

// Pairing my functions with CLI command names.
// The third properties are "validator strings" for parameters.
// n: numerical param, a: alphabetical param, *: any 
FuncDescriptor funcMap[] = {
  { "ping", ping, "" },
  { "whoami", getName, "" },
  { "doublethis", doubler, "n" }
};

// Creating the CLI object
SerialCLI cli(funcMap, len(funcMap));

// And the actual Arduino code :)
void setup() {
  cli.init();
}

void loop() {
  cli.refresh();

  delay(50);
}