#include <Keyboard.h>
#include <EEPROM.h>

String password;
int pswdMaxLength = 50;
int startAddrPswd = 0;

void setup() {
  Serial.begin(9600);
  Keyboard.begin();
  delay(600);
  readPasswordFromEEPROM();

  if (password.length() > 0) {
    delay(10);
    Keyboard.print(password);
    Keyboard.press(KEY_RETURN);
    delay(100);
    Keyboard.releaseAll();
  }
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil(' ');
    
    if (command == "pswd") {
      String arg1 = Serial.readStringUntil(' ');

      if (arg1 == "set") {
        String arg2 = Serial.readStringUntil(' ');
        String arg3 = Serial.readStringUntil('\n');

        if (arg2 == arg3) {
          setPassword(arg2);
        } else {
          Serial.println("Passwords do not match. Please try again.");
        }
      } else if (arg1 == "clear") {
        clearPassword();
      } else if (arg1 == "read") {
        readPasswordFromEEPROM();
      } else {
        commandInvalid();
        contextCommandPswd();
      }
    } else if (command == "help") {
      contextCommandHelp();
    } else {
      commandNotFound(command);
      contextCommandHelp();
    }
  }
}

void clearPassword() {
  setPassword("");
  Serial.println("Password cleared.");
}

void readPasswordFromEEPROM() {
  char values[pswdMaxLength];

  for (int addrOffset = 0; addrOffset <= pswdMaxLength; addrOffset++) {
    int addr = startAddrPswd + addrOffset;
    int val = EEPROM.read(addr);

    if (val != 0x00) {
      values[addrOffset] = char(val);
    } else {
      values[addrOffset] = 0x00;
    }
    
    //Serial.print("Read value '");
    //Serial.print(val);
    //Serial.print("' from EEPROM at address ");
    //Serial.println(addr);
  }

  String pswd(values);

  Serial.print("Read password [");
  Serial.print(pswd);
  Serial.println("]");
  
  password = pswd;
}

void setPassword(String pswd) {
  Serial.print("Proposed Password [");
  Serial.print(pswd);
  Serial.println("]\n");

  pswd.trim();
  password = pswd;
  
  int bufferSize = pswd.length() + 1;
  char pswdBuf[bufferSize];
  pswd.toCharArray(pswdBuf, bufferSize);

  for (int addrOffset = 0; addrOffset <= pswdMaxLength; addrOffset++) {
    
    int addr = startAddrPswd + addrOffset;
    char value;

    if (addrOffset >= bufferSize) {
      value = 0x00;
    } else {
      value = pswdBuf[addrOffset];
    }
    
    EEPROM.write(addr, value);
    Serial.print("Writing value '");
    Serial.print(value);
    Serial.print("' to EEPROM at address ");
    Serial.println(addr);
  }

  Serial.println("\nNew password programmed into internal memory.");
}

void commandNotFound(String command) {
  Serial.print("Command '");
  Serial.print(command);
  Serial.println("' not found. See 'help'.");
}

void commandInvalid() {
  Serial.println("Command invalid.");
}

void contextCommandPswd() {
  String args[] = { "set/clear/read", "password", "repeat_password" };
  contextCommand("pswd", args, "Set or clear the password this key stores.");
}

void contextCommandHelp() {
  Serial.println();
  Serial.println("help - Provides help for this password key.");
  contextCommandPswd();
  Serial.println();
}

void contextCommand(String cmd, String args[], String details) {
  Serial.print(cmd);
  Serial.print(" ");

  for (int i = 0; i <= sizeof(args); i++) {
   String arg = args[i];
   Serial.print("<");
   Serial.print(arg);
   Serial.print("> ");
  }
  
  Serial.print("- ");
  Serial.println(details);
}
