
#include <Arduino.h>
#include "SimpleHandler.h"

void SimpleHandler::startDocument() {
  Serial.println("start document");
}

void SimpleHandler::startArray(const ElementPath& path) {
  Serial.println("start array. ");
}

void SimpleHandler::startObject(const ElementPath& path) {
  Serial.println("start object. ");
}

void SimpleHandler::value(const ElementPath& path, const ElementValue& value) {
  Serial.print(path.toString());
  Serial.print("': ");
  Serial.println(value.toString());
  
  const char* currentKey = path.getKey();
  // Object entry?
  if(currentKey[0] != '\0') {
    if(strcmp(currentKey, "mykey") == 0) {
      
      // CONTEXTUAL AWARE EXTRACTION OF JSON VALUES
      // Used later in getMyKeyString
      mykey_value = value.getInt();
      
    } else if(strcmp(currentKey, "pressure") == 0) {
      
      //TODO: use the value.
    }
    // else ... 
  } 
  // Array item.
  else {
    int currentIndex = path.getIndex();
    if(currentIndex == 0) {
      //TODO: use the value.
    } else if(currentIndex < 5) {
      //TODO: use the value.
    }
    // else ... 
  }
}

void SimpleHandler::endArray(const ElementPath& path) {
  Serial.println("end array. ");
}

void SimpleHandler::endObject(const ElementPath& path) {
  Serial.println("end object. ");
}

void SimpleHandler::endDocument() {
  Serial.println("end document. ");
}

void SimpleHandler::whitespace(const char c) {
  Serial.println("whitespace");
}
