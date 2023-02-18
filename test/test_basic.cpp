#include <gtest/gtest.h>

#include <gob_json.hpp>
#include <gob_json_delegate_handler.hpp>
#include <array>
#include<cmath>
#include <ctime>

// TEST(Basic, Recursive)
namespace
{
// Include 2 JSON documents.
const char test_json[] =
R"***(
{ "name"   : "John Smith",
  "sku"    : "20223",
  "price"  : 23.95,
  "shipTo" : { "name" : "Jane Smith",
               "address" : "123 Maple Street",
               "city" : "Pretendville",
               "state" : "NY",
               "zip"   : "12345" },
  "billTo" : { "name" : "John Smith",
               "address" : "123 Maple Street",
               "city" : "Pretendville",
               "state" : "NY",
               "zip"   : "12345" }
}
{ "name"   : "Alice Brown",
  "sku"    : "54321",
  "price"  : 199.95,
  "shipTo" : { "name" : "Bob Brown",
               "address" : "456 Oak Lane",
               "city" : "Pretendville",
               "state" : "HI",
               "zip"   : "98999" },
  "billTo" : { "name" : "Alice Brown",
               "address" : "456 Oak Lane",
               "city" : "Pretendville",
               "state" : "HI",
               "zip"   : "98999" }
}        
)***";
//
}

using goblib::json::ElementPath;
using goblib::json::ElementValue;

class DocumentCountHandler : public goblib::json::Handler
{
  public:
    int countStart{}, countEnd{};

    virtual void startDocument() override{ ++countStart; }
    virtual void endDocument() override { ++countEnd; }
    virtual void startObject(const ElementPath& ) override {}
    virtual void endObject(const ElementPath& ) override {}
    virtual void startArray(const ElementPath& ) override {}
    virtual void endArray(const ElementPath& ) override {}
    virtual void value(const ElementPath& , const ElementValue& ) override {}
    virtual void whitespace(const char/*ch*/) override {}
};

TEST(Basic, Recursive)
{
    // By default, once a document is finished, it is not parsed thereafter.
    {
        DocumentCountHandler handler;
        goblib::json::StreamingParser parser(&handler);
        for(auto& e: test_json) { parser.parse(e); }

        // Parse 1 document?
        EXPECT_FALSE(parser.hasError());
        EXPECT_EQ(handler.countStart, 1);
        EXPECT_EQ(handler.countEnd, 1);
    }

    // Parse multiple documents if setRecursively(true)
    {
        DocumentCountHandler handler;
        goblib::json::StreamingParser parser(&handler);
        parser.setRecursively(true);
        for(auto& e: test_json)
        {
            parser.parse(e);
        }
        // Parse 2 documents?
        EXPECT_FALSE(parser.hasError());
        EXPECT_EQ(handler.countStart, 2);
        EXPECT_EQ(handler.countEnd, 2);
    }
}

// TEST(Basic, Arithmetic)
namespace
{
const char values_json[] =
R"***(
{
  "values":
  {
    "int8_t"   : [                 -128, "127" ],
    "uint8_t"  : [                    0, "255" ],
    "int16_t"  : [               -32768, "32767" ],
    "uint16_t" : [                    0, "65535" ],
    "int32_t"  : [          -2147483648, "2147483647" ],
    "uint32_t" : [                    0, "4294967295" ],
    "int64_t"  : [ -9223372036854775808, "9223372036854775807"  ],
    "uint64_t" : [                    0, "18446744073709551615" ],
    "float"    : [        -3.402823e+38, " 340282346638528859811704183484516925440.000000" ],
    "double"   : [ -1.797693134862316e+308 , "179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000" ]
  }
}
)***";
//
}

// Target to hold JSON values.
struct Values
{
    Values() { i64 = new int64_t[2]; u64 = new uint64_t[2]; }
    ~Values() { delete[] i64; delete[] u64; };
    
    std::array< int8_t,2>    i8;
    std::array<uint8_t,2>    u8;
    std::vector< int16_t>    i16;
    std::vector<uint16_t>    u16;
    int32_t                  i32[2];
    uint32_t                 u32[2];
    int64_t*                 i64;
    uint64_t*                u64;
    float                    f[2];
    std::array<double,2>     d;
    std::vector<long double> ld;
};

using goblib::json::ElementBase;
using goblib::json::Element;

// DelegateHandler for struct Values.
class ValuesHandler : public goblib::json::DelegateHandler
{
  public:
    struct ValuesDelegater : Delegater
    {
        ValuesDelegater(Values& target) : _v(target) {}
        // Use Elements and iteration instead of if elseif else if else if....
        virtual void value(const ElementPath& path, const ElementValue& value) override
        {   //                                   key,        Pointer to variable to store value
            Element<decltype(_v.i8)>     e_i8  { "int8_t",   &_v.i8 };
            Element<decltype(_v.u8)>     e_u8  { "uint8_t",  &_v.u8 };
            Element<decltype(_v.i16)>    e_i16 { "int16_t",  &_v.i16 };
            Element<decltype(_v.u16)>    e_u16 { "uint16_t", &_v.u16 };
            Element<decltype(_v.i32)>    e_i32 { "int32_t",  &_v.i32 };
            Element<decltype(_v.u32)>    e_u32 { "uint32_t", &_v.u32 };
            Element<decltype(_v.i64)>    e_i64 { "int64_t",  &_v.i64 };
            Element<decltype(_v.u64)>    e_u64 { "uint64_t", &_v.u64 };
            Element<decltype(_v.f)>      e_f   { "float",    &_v.f };
            Element<decltype(_v.d)>      e_d   { "double",   &_v.d };
            ElementBase* tbl[] = { &e_i8, &e_u8, &e_i16, &e_u16, &e_i32, &e_u32, &e_i64, &e_u64, &e_f, &e_d };
            if(path.getIndex() < 0) { return; } // Not array?
            auto parent = path.getParent();
            if(parent)
            {
                for(auto& e : tbl) { if(*e == parent->getKey()) { e->store(value, path.getIndex()); return; } }
            }
        }
      private:
        Values& _v;
    };

    ValuesHandler(Values& v) : _value(v) {}
    void startObject(const ElementPath& path) override
    {
        // Pass to delegater.
        if(path.getCount() == 1 && strcmp(path.getKey(), "values") == 0)
        {
            pushDelegater(new ValuesDelegater(_value));
            return;
        }
        DelegateHandler::startObject(path);
    }
  private:
    Values& _value;
};

TEST(Basic, Arithmetic)
{
    {
        Values vs{};
        ValuesHandler handler(vs);
        goblib::json::StreamingParser parser(&handler);
        for(auto& e: values_json) { parser.parse(e); }

        EXPECT_FALSE(parser.hasError());

        EXPECT_EQ(vs.i8[0], -128);
        EXPECT_EQ(vs.i8[1],  127);
        EXPECT_EQ(vs.u8[0],    0);
        EXPECT_EQ(vs.u8[1],  255);
        
        EXPECT_EQ(vs.i16.size(), 2U);
        if(vs.i16.size() >= 2)
        {
            EXPECT_EQ(vs.i16[0], -32768);
            EXPECT_EQ(vs.i16[1],  32767);
        }
        
        EXPECT_EQ(vs.u16.size(), 2U);
        if(vs.u16.size() >= 2)
        {
            EXPECT_EQ(vs.u16[0],      0U);
            EXPECT_EQ(vs.u16[1],  65535);
        }

        EXPECT_EQ(vs.i32[0], -2147483648);
        EXPECT_EQ(vs.i32[1],  2147483647);
        EXPECT_EQ(vs.u32[0],          0U);
        EXPECT_EQ(vs.u32[1],  4294967295);

        //EXPECT_EQ(vs.i64[0],  INTMAX_C(-9223372036854775808)); // warning: integer constant is so large that it is unsigned
        EXPECT_EQ(vs.i64[0],  INTMAX_C( -9223372036854775807) - 1); // Avoid warning. 
        EXPECT_EQ(vs.i64[1],  INTMAX_C(  9223372036854775807));
        EXPECT_EQ(vs.u64[0],                              0U);
        EXPECT_EQ(vs.u64[1], UINTMAX_C(18446744073709551615));

        EXPECT_FLOAT_EQ(vs.f[0], -3.402823e+38);
        EXPECT_FLOAT_EQ(vs.f[1],  3.402823e+38);
        //EXPECT_DOUBLE_EQ(vs.d[0], -1.797693134862316e+308); // warning: floating constant exceeds range of 'double' [-Woverflow]
        EXPECT_EQ(vs.d.size(), 2U);
        EXPECT_DOUBLE_EQ(vs.d[0], -0.797693134862316e+308 - 1.0e+308); // Avoid warning
        EXPECT_DOUBLE_EQ(vs.d[1],  0.797693134862316e+308 + 1.0e+308); // Avoid warning
    }
}

// TEST(Basic, AnyType)
namespace
{
const char any_type_json[] =
R"***(
{
  "bool_key_0": true,
  "bool_key_1": false,
  "null_i16": null,
  "string": "漢字カナまじりANK☎️"
}
)***";

struct AnyTestHandler: public goblib::json::Handler
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
        if     (strcmp(path.getKey(),"bool_key_0") == 0) { b0 = value.getBool(); }
        else if(strcmp(path.getKey(),"bool_key_1") == 0) { b1 = value.getBool(); }
        else if(strcmp(path.getKey(),"null_i16"  ) == 0) { if(!value.isNull()) { i16 = value.getInt(); } }
        else if(strcmp(path.getKey(),"string"    ) == 0) { str = value.getString(); }
    }

    bool b0{}, b1{true};
    int16_t i16{42};
    goblib::json::string_t str;
};
//
}

TEST(Basic, AnyType)
{
    AnyTestHandler handler;
    goblib::json::StreamingParser parser(&handler);
    for(auto& e: any_type_json) { parser.parse(e); }

    EXPECT_FALSE(parser.hasError());
    EXPECT_TRUE(handler.b0);
    EXPECT_FALSE(handler.b1);
    EXPECT_EQ(handler.i16, 42);
    EXPECT_STREQ(handler.str.c_str(), "漢字カナまじりANK☎️");
}

// TEST(Basic, Unicode)
namespace
{
const char unicode_escape_json[] =
R"***(
{
  "text":"\u30b31\u30de\\\u30f3&\u30c9@",
  "surrogate":"\ud867\ude3d\ud840\udc0b\ud842\udfb7\ud853\udd14"
}
)***";

struct UnicodeEscapeTestHandler: public goblib::json::Handler
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
        if(strcmp(path.getKey(),"text") == 0) { str = value.getString(); }
        else if(strcmp(path.getKey(),"surrogate") == 0) { str2 = value.getString(); }
        else if(strcmp(path.getKey(),"JISX0201") == 0) { jisx0201 = value.getString(); }
        else if(strcmp(path.getKey(),"JISX0213") == 0) { jisx0213 = value.getString(); }
    }
    goblib::json::string_t str{"deadbeaf"};
    goblib::json::string_t str2{"deadbeaf"};
    goblib::json::string_t jisx0201{"deadbeaf"};
    goblib::json::string_t jisx0213{"deadbeaf"};
};
//
}

TEST(Basic, UnicodeEscape)
{
    UnicodeEscapeTestHandler handler;
    goblib::json::StreamingParser parser(&handler);
    for(auto& e: unicode_escape_json) { parser.parse(e); }

    EXPECT_FALSE(parser.hasError());
    EXPECT_STREQ(handler.str.c_str(), "コ1マ\\ン&ド@");
    EXPECT_STREQ(handler.str2.c_str(), "𩸽𠀋𠮷𤴔");

}

