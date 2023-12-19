#define SERBAUDRATE 115200

#define CMDBUFFERLEN 30
#define PARAMBUFFERLEN 30
#define MAXPARAMS 4

#include "Arduino.h"

#define SERBUFFERLEN (CMDBUFFERLEN + PARAMBUFFERLEN + 1)  // extra "one" is for the space between the command and parameters.

#define len(X) sizeof(X) / sizeof(X[0])

typedef void (*genFunc)(const char *[]);

struct FuncDescriptor {
  char name[CMDBUFFERLEN];
  genFunc func;
  char validatorString[MAXPARAMS];
};

class SerialCLI {
public:
  SerialCLI(FuncDescriptor fmap[], int numFunctions)
    : funcMap(fmap), numFuncs(numFunctions) {
  }
  void init();
  void refresh();
  static bool validate(const char **, const char[]);

private:
  FuncDescriptor *funcMap;
  int numFuncs;

  char serialBuffer[SERBUFFERLEN + 1];
  bool hasNewMessage;
  char *command;
  const char *paramList[MAXPARAMS];

  char helpCommand[5] = "help";

  void flushSerial();

  void getSerialMessage();
  void parseCommand();
  void handleCommand();
};