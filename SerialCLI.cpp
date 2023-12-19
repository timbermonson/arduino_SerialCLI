#include "Arduino.h"
#include "SerialCLI.h"

static char *trimPreceding(char *str) {
  if (str == NULL)
    return NULL;

  // Trim preceding spaces
  while (str[0] == ' ') {
    str++;
  }

  if (strlen(str) == 0) {
    str = NULL;
  }

  return str;
}

static char *findNextParam(const char *str) {
  if (str == NULL)
    return NULL;

  return trimPreceding(strchr(str, ' '));
}

static bool isNumberical(const char *str) {
  for (int i = 0; i < (int)strlen(str); i++) {
    if (!isDigit(str[i])) return false;
  }

  return true;
}

static bool isAlphabetical(const char *str) {
  for (int i = 0; i < (int)strlen(str); i++) {
    if (!isAlpha(str[i])) return false;
  }

  return true;
}

bool SerialCLI::validate(const char **paramList, const char validationString[]) {
  int numParams = strlen(validationString);
  // Ensure that validation string has proper length
  if (numParams < 0 || numParams > MAXPARAMS) {
    Serial.print(F("Error! validate() called with too many/few params: "));
    Serial.println(validationString);
    return false;
  }

  // Ensure that validation string has no invalid params (doing in own loop ensures this error takes priority)
  for (int i = 0; i < numParams; i++) {
    char v = validationString[i];

    if (v != 'n' && v != 'a' && v != '*') {
      Serial.print(F("Error! validate() called with invalid param: "));
      Serial.println(validationString);
      return false;
    }
  }

  // Perform validation on params
  for (int i = 0; i < numParams; i++) {
    char v = validationString[i];
    const char *param = paramList[i];

    if (param == NULL) {
      Serial.print(F("Error! Command requires: "));
      Serial.print(numParams);
      Serial.print(F(" parameter(s), received: "));
      Serial.println(i);
      return false;
    }

    switch (v) {
      case 'n':
        if (!isNumberical(param)){
          Serial.print(F("Error! Parameter #"));
          Serial.print(i+1);
          Serial.println(F(" must be numerical"));
          return false;
        }
        break;
      case 'a':
        if (!isAlphabetical(param)){
          Serial.print(F("Error! Parameter #"));
          Serial.print(i+1);
          Serial.println(F(" must be a string"));
          return false;
        }
        break;
      default:
        break;
    }
  }

  return true;
}

void SerialCLI::init() {
  serialBuffer[SERBUFFERLEN] = '\0';
  Serial.begin(SERBAUDRATE);
  delay(10);
  flushSerial();
}

void SerialCLI::flushSerial() {
  do {
    Serial.read();
  } while (Serial.available());
}

void SerialCLI::refresh() {
  getSerialMessage();
  if (!hasNewMessage)
    return;
  hasNewMessage = false;
  parseCommand();
  handleCommand();
}

void SerialCLI::getSerialMessage() {
  if (!Serial.available())
    return;

  delay(10);
  int availableBytes = Serial.available();

  if (availableBytes > SERBUFFERLEN) {
    Serial.println("Error: Serial buffer overflowed. Clearing...");
    flushSerial();
    return;
  }

  for (int i = 0; i < availableBytes; i++) {
    char serialCurChar = Serial.read();
    if (serialCurChar == '\n') {
      serialBuffer[i] = '\0';

      // Set newCommand flag so parser can handle
      hasNewMessage = true;

      // echo message back, emulating command terminal
      Serial.print(F("$ "));
      Serial.println(serialBuffer);

      return;
    }

    serialBuffer[i] = serialCurChar;
  }

  Serial.print(F("Error: received incomplete serial message (no newline): \""));
  serialBuffer[availableBytes] = '\0';
  Serial.print(serialBuffer);
  Serial.println(F("\""));
  serialBuffer[0] = '\0';
}
void SerialCLI::parseCommand() {
  // Put command pointer at start of command
  command = trimPreceding((char *)serialBuffer);

  // Put param pointers at the start of the first MAXPARAMS params
  paramList[0] = findNextParam(command);
  for (int i = 1; i < MAXPARAMS; i++) {
    paramList[i] = findNextParam(paramList[i - 1]);
  }

  // Replace every space in the entire serial buffer with a null terminator
  for (int i = 0; i < SERBUFFERLEN; i++) {
    if (serialBuffer[i] == ' ') {
      serialBuffer[i] = '\0';
    }
  }

  if (strlen(command) > CMDBUFFERLEN) {
    Serial.println(F("Command name too long!"));
    command = NULL;
  }
}

void SerialCLI::handleCommand() {
  if (command == NULL)
    return;

  if (strcmp(helpCommand, command) == 0) {
    Serial.println(F("LIST OF COMMANDS"));
    for (int i = 0; i < numFuncs; i++) {
      Serial.print(F("- "));
      Serial.println(funcMap[i].name);
    }
    Serial.println(F("- help"));
    return;
  }

  for (int i = 0; i < numFuncs; i++) {
    FuncDescriptor desc = funcMap[i];

    if (strcmp(desc.name, command) == 0) {
      if(!validate(paramList, desc.validatorString)) return;

      desc.func(paramList);
      return;
    }
  }

  Serial.println(F("Unknown Command!"));
}
