This library provides an easy framework for triggering callback functions via serial messages.

It handles parameterized callbacks, ex. `$doublethis 17` -> Callback: `Your input, doubled, equals: 34`

_As well as_ parameter validation, ex. `$doublethis aaaa` -> Library: `Error! Parameter #1 must be numerical`

# Example Code & Output

**Code:**
```cpp
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
```

**Output (demonstrating the CLI created by the above code):**
```
$ a
Unknown Command!

$ help
LIST OF COMMANDS
- ping
- whoami
- doublethis (syntax: "n")
- help

$ ping
Pong! Millis since bootup: 7318

$ whoami
I'm an arduino!

$ doublethis
Error! Command requires: 1 parameter(s), received: 0

$ doublethis aaaa
Error! Parameter #1 must be numerical

$ doublethis 222
Your input, doubled, equals: 444
```
