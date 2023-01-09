

#include <gtest/gtest.h>
#include <gob_json.hpp>
#include <gob_json_arduino.hpp>

using goblib::json::ElementPath;
using goblib::json::ElementValue;

struct TestHandler: public goblib::json::Handler
{
    virtual void startDocument() override{}
    virtual void endDocument() override {}
    virtual void startObject(const ElementPath& ) override {}
    virtual void endObject(const ElementPath& ) override {}
    virtual void startArray(const ElementPath& ) override {}
    virtual void endArray(const ElementPath& ) override {}
    virtual void whitespace(const char/*ch*/) override {}
    virtual void value(const ElementPath& path, const ElementValue& value) override
    {
        if(strcmp(path.getKey(),"string_key") == 0) { str = value.getString(); }
        else if(strcmp(path.getKey(),"int_key") == 0) { i32 = value.getInt(); }
    }

    String str;
    int32_t i32;
};


TEST(Arduino, Stream)
{
    TestHandler handler;
    goblib::json::arduino::Stream stream(&handler);


    stream.printf(R"({"string_key":)");
    stream.printf(R"("Arduino library for parsing potentially huge json streams on devices with scarce memory",)");
    
    const char block[] = R"("int_key":98765})";
    stream.write(block, (size_t)sizeof(block) - 1);


    EXPECT_FALSE(stream.hasError());
    EXPECT_EQ(stream.getWriteError(), 0);

    EXPECT_STREQ(handler.str.c_str(),
                 "Arduino library for parsing potentially huge json streams on devices with scarce memory");
    EXPECT_EQ(handler.i32, 98765);
}
