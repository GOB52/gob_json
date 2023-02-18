
#include <gtest/gtest.h>

#include <gob_json.hpp>
#include <gob_json_element.hpp>
#include <gob_json_delegate_handler.hpp>

// TEST(Element, Basic)
namespace
{
const char test_json[] =
R"***(
{
  "boolean": true,
  "boolean_array0": [false, true, true],
  "boolean_array1": [false, true, true],
  "boolean_array2": [false, true, true],
  "integer": 123456789,
  "integer_array0": [9, 8, -7 ],
  "integer_array1": [6, 5, -4 ],
  "integer_array2": [3, 2, -1 ],
  "float": 123.456,
  "float_array0": [1.23, 4.56, -7.89],
  "float_array1": [12.3, 45.6, -78.9],
  "float_array2": [123.0, 456.0, -789.0],
  "string": "あいうABC",
  "string_array0": ["A", "あ", "☎️"],
  "string_array1": ["B", "い", "✌️"],
  "string_array2": ["C", "う", "😄"]
}
)***";
//
}

using goblib::json::string_t;
using goblib::json::ElementPath;
using goblib::json::ElementValue;
using goblib::json::Element;
using goblib::json::ElementBase;


class TestHandler : public goblib::json::Handler
{
  public:
    virtual void startDocument() override{}
    virtual void endDocument() override {}
    virtual void startObject(const ElementPath& ) override {}
    virtual void endObject(const ElementPath& ) override {}
    virtual void startArray(const ElementPath& ) override {}
    virtual void endArray(const ElementPath& ) override {}
    virtual void whitespace(const char/*ch*/) override {}
    virtual void value(const ElementPath& path, const ElementValue& value) override
    {
        Element<decltype(b)>   e_b   { "boolean", &b };
        Element<decltype(ba0)> e_ba0 { "boolean_array0", &ba0 };
        Element<decltype(ba1)> e_ba1 { "boolean_array1", &ba1 };
        Element<decltype(ba2)> e_ba2 { "boolean_array2", &ba2 };
        Element<decltype(i)>   e_i   { "integer", &i };
        Element<decltype(ia0)> e_ia0 { "integer_array0", &ia0 };
        Element<decltype(ia1)> e_ia1 { "integer_array1", &ia1 };
        Element<decltype(ia2)> e_ia2 { "integer_array2", &ia2 };
        Element<decltype(f)>   e_f   { "float", &f };
        Element<decltype(fa0)> e_fa0 { "float_array0", &fa0 };
        Element<decltype(fa1)> e_fa1 { "float_array1", &fa1 };
        Element<decltype(fa2)> e_fa2 { "float_array2", &fa2 };
        Element<decltype(s)>   e_s   { "string", &s };
        Element<decltype(sa0)> e_sa0 { "string_array0", &sa0 };
        Element<decltype(sa1)> e_sa1 { "string_array1", &sa1 };
        Element<decltype(sa2)> e_sa2 { "string_array2", &sa2 };

        //        GOB_JSON_LOGI("[%s] %d <%s>\n", path.toString().c_str(), value.isBool(), value.toString().c_str());
        ElementBase* tbl[] =
        {
            &e_b, &e_ba0, &e_ba1, &e_ba2,
            &e_i, &e_ia0, &e_ia1, &e_ia2,
            &e_f, &e_fa0, &e_fa1, &e_fa2,
            &e_s, &e_sa0, &e_sa1, &e_sa2,
        };
        // Not JSON array
        if(path.getIndex() < 0)
        {
            for(auto& e : tbl) { if(*e == path.getKey()) { e->store(value, path.getIndex()); return; } }
        }
        // JSON Array
        auto parent = path.getParent();
        if(parent)
        {
            for(auto& e : tbl) { if(*e == parent->getKey()) { e->store(value, path.getIndex()); return; } }
        }
        GOB_JSON_LOGD("Oops");
    }

    bool b{};
    bool ba0[3]{true,false,false};
    std::array<bool,3> ba1;
    std::vector<bool> ba2;

    int32_t i{};
    int32_t ia0[3]{};
    std::array<int32_t,3> ia1;
    std::vector<int32_t> ia2;

    float f{};
    float fa0[3]{};
    std::array<float,3> fa1;
    std::vector<float> fa2;

    string_t s{};
    string_t sa0[3]{};
    std::array<string_t,3> sa1;
    std::vector<string_t> sa2;
};

TEST(Element, Basic)
{
    TestHandler handler;
    goblib::json::StreamingParser parser(&handler);
    for(auto& e: test_json) { parser.parse(e); }

    EXPECT_FALSE(parser.hasError());
    //
    EXPECT_TRUE(handler.b);
    EXPECT_EQ(handler.ba0[0], false);
    EXPECT_EQ(handler.ba0[1], true);
    EXPECT_EQ(handler.ba0[2], true);
    EXPECT_EQ(handler.ba1[0], false);
    EXPECT_EQ(handler.ba1[1], true);
    EXPECT_EQ(handler.ba1[2], true);
    EXPECT_EQ(handler.ba2.size(), 3U);
    if(handler.ba2.size() >= 3)
    {
        EXPECT_EQ(handler.ba2[0], false);
        EXPECT_EQ(handler.ba2[1], true);
        EXPECT_EQ(handler.ba2[2], true);
    }
    //
    EXPECT_EQ(handler.i, 123456789);
    EXPECT_EQ(handler.ia0[0], 9);
    EXPECT_EQ(handler.ia0[1], 8);
    EXPECT_EQ(handler.ia0[2], -7);
    EXPECT_EQ(handler.ia1[0], 6);
    EXPECT_EQ(handler.ia1[1], 5);
    EXPECT_EQ(handler.ia1[2], -4);
    EXPECT_EQ(handler.ia2.size(), 3U);
    if(handler.ia2.size() >= 3)
    {
        EXPECT_EQ(handler.ia2[0], 3);  
        EXPECT_EQ(handler.ia2[1], 2);
        EXPECT_EQ(handler.ia2[2], -1);
    }
    //
    EXPECT_FLOAT_EQ(handler.f, 123.456f);
    EXPECT_FLOAT_EQ(handler.fa0[0], 1.23f);
    EXPECT_FLOAT_EQ(handler.fa0[1], 4.56f);
    EXPECT_FLOAT_EQ(handler.fa0[2], -7.89f);
    EXPECT_FLOAT_EQ(handler.fa1[0], 12.3f);
    EXPECT_FLOAT_EQ(handler.fa1[1], 45.6f);
    EXPECT_FLOAT_EQ(handler.fa1[2], -78.9f);
    EXPECT_EQ(handler.fa2.size(), 3U);
    if(handler.fa2.size() >= 3)
    {
        EXPECT_FLOAT_EQ(handler.fa2[0], 123.0f);
        EXPECT_FLOAT_EQ(handler.fa2[1], 456.0f);
        EXPECT_FLOAT_EQ(handler.fa2[2], -789.0f);
    }
    //
    EXPECT_STREQ(handler.s.c_str(), "あいうABC");
    EXPECT_STREQ(handler.sa0[0].c_str(), "A");
    EXPECT_STREQ(handler.sa0[1].c_str(), "あ");
    EXPECT_STREQ(handler.sa0[2].c_str(), "☎️");
    EXPECT_STREQ(handler.sa1[0].c_str(), "B");
    EXPECT_STREQ(handler.sa1[1].c_str(), "い");
    EXPECT_STREQ(handler.sa1[2].c_str(), "✌️");
    EXPECT_EQ(handler.sa2.size(), 3U);
    if(handler.sa2.size() >= 3)
    {
        EXPECT_STREQ(handler.sa2[0].c_str(), "C");
        EXPECT_STREQ(handler.sa2[1].c_str(), "う");
        EXPECT_STREQ(handler.sa2[2].c_str(), "😄");
    }
}

// TEST(Element, Custom)
namespace
{
// Nested object and custom type values.
const char custom_types_json[] =
R"***(
{
  "rgba": "##12abcdef",
  "datetime":
  {
    "start_at": "2009-08-07T12:34:56",
    "end_at": "2009-08-07T13:57:00"
  }
}
)***";
//
}

// RGBA color
struct RGBA
{
    static RGBA parse(const char* str)
    {
        int rr,gg,bb,aa;
        sscanf(str, "##%02x%02x%02x%02x", &aa, &rr, &gg, &bb);
        return RGBA(rr,gg,bb,aa);
    }
    RGBA() : RGBA(0,0,0,0) {}
    RGBA(uint8_t rr, uint8_t gg, uint8_t bb, uint8_t aa) : r(rr), g(gg), b(bb), a(aa) {}
    uint8_t r{}, g{}, b{}, a{};
};

// Wrapping time_t (time_t is typedefed some integer, So to be a separate type.
struct Time_t
{
    static Time_t parse(const char* str)
    {
        struct tm tmp{};
        strptime(str, "%FT%T", &tmp);
        auto tt = mktime(&tmp);
        return Time_t(tt);
    }
    explicit Time_t(const time_t tt) : t(tt) {}
    Time_t() : Time_t(0) {}
    time_t t{};
};

struct CustomTypes
{
    RGBA rgba{};
    struct Datetime
    {
        Time_t start_at;
        Time_t end_at;
    } datetime;
};

// You can make the specific element helper
template<typename T> struct CustomElement : public Element<T>
{
    CustomElement(const char*k, T* vp) : Element<T>(k, vp) {} // Need constructor
    virtual void store(const ElementValue& ev, const int index = -1) override { _store(ev, index); }

    // Declrare _store functions for each custom type.(SFINAE)
    // For RGBA
    template<typename U = T,
             typename std::enable_if<std::is_same<U, RGBA>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int)
    {
        *this->value = ev.isString() ? RGBA::parse(ev.getString()) : RGBA{};
    }
    // For Time_t
    template<typename U = T,
             typename std::enable_if<std::is_same<U, Time_t>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int)
    {
        *this->value = ev.isString() ? Time_t::parse(ev.getString()) : Time_t{};
    }
};

class CustomTypesHandler : public goblib::json::DelegateHandler
{
  public:
    // The hierarchical structure shall be the same as in CustomTypes
    struct CustomTypesDelegater : Delegater
    {
        struct DatetimeDelegater : Delegater
        {
            DatetimeDelegater(CustomTypes::Datetime& target) : _v(target) {}
            virtual void value(const ElementPath& path, const ElementValue& value) override
            {
                CustomElement<decltype(_v.start_at)>  e_s    { "start_at", &_v.start_at };
                CustomElement<decltype(_v.end_at)>    e_e    { "end_at",   &_v.end_at   };
                ElementBase* tbl[] = { &e_s, &e_e };
                for(auto& e : tbl) { if(*e == path.getKey()) { e->store(value, path.getIndex()); return; } }
            }
          private:
            CustomTypes::Datetime& _v;
        };

        CustomTypesDelegater(CustomTypes& target) : _v(target) {}
        virtual Delegater* startObject(const ElementPath& path) override
        {
            if(strcmp(path.getKey(),"datetime")==0) { return new DatetimeDelegater(_v.datetime); }
            return Delegater::startObject(path);
        }
        virtual void value(const ElementPath& path, const ElementValue& value) override
        {
            CustomElement<decltype(_v.rgba)> e_rgba { "rgba",   &_v.rgba };
            ElementBase* tbl[] = { &e_rgba };
            for(auto& e : tbl) { if(*e == path.getKey()) { e->store(value, path.getIndex()); return; } }
        }
      private:
        CustomTypes& _v;
    };

    CustomTypesHandler(CustomTypes& v) : _value(v) {}
    void startObject(const ElementPath& path) override
    {
        if(path.getCount() == 0) { pushDelegater(new CustomTypesDelegater(_value)); return; }
        DelegateHandler::startObject(path);
    }
  private:
    CustomTypes& _value;
};

TEST(Element, Custom)
{
    {
        CustomTypes cs{};
        CustomTypesHandler handler(cs);
        goblib::json::StreamingParser parser(&handler);
        for(auto& e: custom_types_json) { parser.parse(e); }

        EXPECT_FALSE(parser.hasError());

        EXPECT_EQ(cs.rgba.r, 0xab);
        EXPECT_EQ(cs.rgba.g, 0xcd);
        EXPECT_EQ(cs.rgba.b, 0xef);
        EXPECT_EQ(cs.rgba.a, 0x12);

        struct tm tmp;
        tmp = *localtime(&cs.datetime.start_at.t);
        EXPECT_EQ(tmp.tm_year + 1900, 2009);
        EXPECT_EQ(tmp.tm_mon + 1, 8);
        EXPECT_EQ(tmp.tm_mday, 7);
        EXPECT_EQ(tmp.tm_hour,12);
        EXPECT_EQ(tmp.tm_min, 34);        
        EXPECT_EQ(tmp.tm_sec, 56);

        tmp = *localtime(&cs.datetime.end_at.t);
        EXPECT_EQ(tmp.tm_year + 1900, 2009);
        EXPECT_EQ(tmp.tm_mon + 1, 8);
        EXPECT_EQ(tmp.tm_mday, 7);
        EXPECT_EQ(tmp.tm_hour,13);
        EXPECT_EQ(tmp.tm_min, 57);        
        EXPECT_EQ(tmp.tm_sec, 0);;
    }
}
