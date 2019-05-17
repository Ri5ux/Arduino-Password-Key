#include <DigiKeyboard.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

String password = "";
int pswdMaxLength = 50;
int startAddrPswd = 0;

SoftwareSerial line(4, 5); // RX, TX

void setup() {
  line.begin(115200);
  delay(600);
  readPasswordFromEEPROM();

  if (password.length() > 0) {
    delay(10);
    DigiKeyboard.print(password);
    DigiKeyboard.sendKeyStroke(0);
    delay(100);
    DigiKeyboard.sendKeyStroke(KEY_ENTER);
  }
}

void loop() {
    String command = readUntil(' ');
    
    if (command == "pswd") {
      String arg1 = readUntil(' ');

      if (arg1 == "set") {
        String arg2 = readUntil(' ');
        String arg3 = readUntil('\n');

        if (arg2 == arg3) {
          setPassword(arg2);
        } else {
          line.println("Passwords do not match.");
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
      contextCommandHelp();
    }
}

String readUntil(char c)
{
  String lineBuffer = "";
  
  while (line.available() > 0) {
    char chr = line.read();
    
    if (chr == c)
    {
      return lineBuffer;
    }
    
    lineBuffer += chr;
  }
}

void clearPassword() {
  setPassword("");
  line.println("Password cleared.");
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

  line.print("Read [");
  line.print(pswd);
  line.println("]");
  
  password = pswd;
}

void setPassword(String pswd) {
  line.print("Writing [");
  line.print(pswd);
  line.println("]\n");

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
    line.print("Writing '");
    line.print(value);
    line.print("' to addr ");
    line.println(addr);
  }

  line.println("\nPassword set.");
}

void commandInvalid() {
  line.println("Command invalid.");
}

void contextCommandPswd() {
  String args[] = { "set/clear/read", "password", "repeat_password" };
  contextCommand("pswd", args, "Set or clear the password this key stores.");
}

void contextCommandHelp() {
  line.println();
  line.println("help - Provides help for this password key.");
  contextCommandPswd();
  line.println();
}

void contextCommand(String cmd, String args[], String details) {
  line.print(cmd);
  line.print(" ");

  for (int i = 0; i <= sizeof(args); i++) {
   String arg = args[i];
   line.print("<");
   line.print(arg);
   line.print("> ");
  }
  
  line.print("- ");
  line.println(details);
}
