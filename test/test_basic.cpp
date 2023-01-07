#include <gtest/gtest.h>
#include <gob_json.hpp>

namespace
{
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

class BasicHandler : public goblib::json::Handler
{
  public:
    int countStart{}, countEnd{};

    virtual void startDocument() { ++countStart; printf("%s\n", __func__); }
    virtual void endDocument() { ++countEnd; printf("%s\n", __func__); }
    virtual void startObject(const ElementPath& path) {printf("%s:%s\n", __func__, path.toString().c_str()); }
    virtual void endObject(const ElementPath& path) { printf("%s:%s\n", __func__, path.toString().c_str()); }
    virtual void startArray(const ElementPath& path) { printf("%s:%s\n", __func__, path.toString().c_str()); }
    virtual void endArray(const ElementPath& path) { printf("%s:%s\n", __func__, path.toString().c_str()); }
    virtual void value(const ElementPath& path, const ElementValue& value)
    {
        printf("%s:%s:[%s]\n", __func__, path.toString().c_str(), value.toString().c_str());
    }
    virtual void whitespace(const char/*ch*/) {}
};

TEST(JSON, Basic)
{
    BasicHandler handler;
    goblib::json::StreamingParser parser(&handler);
    for(auto& e: test_json)
    {
        parser.parse(e);
    }
    EXPECT_EQ(handler.countStart, 1);
    EXPECT_EQ(handler.countEnd, 1);
    
    BasicHandler handler2;
    parser.reset();
    parser.setRecursively(true);
    parser.setHandler(&handler2);
    for(auto& e: test_json)
    {
        parser.parse(e);
    }
    EXPECT_EQ(handler2.countStart, 2);
    EXPECT_EQ(handler2.countEnd, 2);

}
