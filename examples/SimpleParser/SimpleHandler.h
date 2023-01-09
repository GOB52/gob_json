#pragma once

#include <gob_json_handler.hpp>

using goblib::json::ElementPath;
using goblib::json::ElementValue;

class SimpleHandler: public goblib::json::Handler {

  private:
    int mykey_value = 0;

  public:
    virtual void startDocument() override;

    virtual void startArray(const ElementPath& path) override;

    virtual void startObject(const ElementPath& path) override;

    virtual void endArray(const ElementPath& path) override;

    virtual void endObject(const ElementPath& path) override;

    virtual void endDocument() override;

    virtual void value(const ElementPath& path, const ElementValue& value) override;
  
    virtual void whitespace(const char c) override;

    // our custom function to extract something back from the json when parsed
    inline String getMyKeyString()
    {
      return String("The value of myKey was: " + String(mykey_value));
    }
};
