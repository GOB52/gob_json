
#include <gtest/gtest.h>
#include <gob_json.hpp>

namespace
{
const char jma_json[] =
R"***([{
	"publishingOffice": "金沢地方気象台",
	"reportDatetime": "2023-01-07T18:00:00+09:00",
	"timeSeries": [{
		"timeDefines": ["2023-01-07T17:00:00+09:00", "2023-01-08T00:00:00+09:00", "2023-01-09T00:00:00+09:00"],
		"areas": [{
			"area": {
				"name": "加賀",
				"code": "170010"
			},
			"weatherCodes": ["304", "317", "218"],
			"weathers": ["雨か雪　所により　雷　を伴う", "雨か雪　昼前　から　くもり　所により　朝　まで　雷　を伴う", "くもり　後　雨か雪"],
			"winds": ["北東の風", "東の風　後　南の風　海上　では　北の風　やや強く", "南の風　後　北の風"],
			"waves": ["１メートル", "１．５メートル　後　１メートル", "１メートル　後　２．５メートル"]
		}, {
			"area": {
				"name": "能登",
				"code": "170020"
			},
			"weatherCodes": ["300", "206", "218"],
			"weathers": ["雨　所により　雷　を伴う", "くもり　明け方　まで　雨か雪　所により　朝　まで　雷　を伴う", "くもり　後　雨か雪"],
			"winds": ["北の風　海上　では　北の風　やや強く", "北の風　後　南の風　海上　では　はじめ　北の風　やや強く", "南の風　後　北の風"],
			"waves": ["１．５メートル", "１．５メートル　後　１メートル", "１メートル　後　３メートル"]
		}]
	}, {
		"timeDefines": ["2023-01-07T18:00:00+09:00", "2023-01-08T00:00:00+09:00", "2023-01-08T06:00:00+09:00", "2023-01-08T12:00:00+09:00", "2023-01-08T18:00:00+09:00"],
		"areas": [{
			"area": {
				"name": "加賀",
				"code": "170010"
			},
			"pops": ["70", "60", "50", "10", "20"]
		}, {
			"area": {
				"name": "能登",
				"code": "170020"
			},
			"pops": ["80", "50", "30", "10", "30"]
		}]
	}, {
		"timeDefines": ["2023-01-08T00:00:00+09:00", "2023-01-08T09:00:00+09:00"],
		"areas": [{
			"area": {
				"name": "金沢",
				"code": "56227"
			},
			"temps": ["3", "9"]
		}, {
			"area": {
				"name": "輪島",
				"code": "56052"
			},
			"temps": ["3", "8"]
		}]
	}]
}, {
	"publishingOffice": "金沢地方気象台",
	"reportDatetime": "2023-01-07T17:00:00+09:00",
	"timeSeries": [{
		"timeDefines": ["2023-01-08T00:00:00+09:00", "2023-01-09T00:00:00+09:00", "2023-01-10T00:00:00+09:00", "2023-01-11T00:00:00+09:00", "2023-01-12T00:00:00+09:00", "2023-01-13T00:00:00+09:00", "2023-01-14T00:00:00+09:00"],
		"areas": [{
			"area": {
				"name": "石川県",
				"code": "170000"
			},
			"weatherCodes": ["317", "218", "201", "101", "101", "101", "202"],
			"pops": ["", "80", "30", "20", "20", "20", "50"],
			"reliabilities": ["", "", "B", "A", "A", "B", "C"]
		}]
	}, {
		"timeDefines": ["2023-01-08T00:00:00+09:00", "2023-01-09T00:00:00+09:00", "2023-01-10T00:00:00+09:00", "2023-01-11T00:00:00+09:00", "2023-01-12T00:00:00+09:00", "2023-01-13T00:00:00+09:00", "2023-01-14T00:00:00+09:00"],
		"areas": [{
			"area": {
				"name": "金沢",
				"code": "56227"
			},
			"tempsMin": ["", "3", "1", "-1", "1", "1", "6"],
			"tempsMinUpper": ["", "4", "2", "1", "3", "3", "7"],
			"tempsMinLower": ["", "2", "0", "-2", "-1", "-1", "1"],
			"tempsMax": ["", "11", "6", "9", "12", "15", "15"],
			"tempsMaxUpper": ["", "13", "8", "11", "14", "17", "18"],
			"tempsMaxLower": ["", "10", "5", "7", "10", "13", "13"]
		}]
	}],
	"tempAverage": {
		"areas": [{
			"area": {
				"name": "金沢",
				"code": "56227"
			},
			"min": "1.4",
			"max": "7.3"
		}]
	},
	"precipAverage": {
		"areas": [{
			"area": {
				"name": "金沢",
				"code": "56227"
			},
			"min": "41.5",
			"max": "68.3"
		}]
	}
}]
)***";
//
}
using goblib::json::ElementPath;
using goblib::json::ElementValue;

class JmaHandler : public goblib::json::Handler
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

TEST(JSON, JMA)
{
    JmaHandler handler;
    goblib::json::StreamingParser parser(&handler);
    for(auto& e: jma_json)
    {
        parser.parse(e);
    }
}
