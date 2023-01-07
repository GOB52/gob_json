
#include <gtest/gtest.h>
#include <gob_json.hpp>

namespace
{
const char rm_json[] =
R"***({
  "name": "OneBedroomHouse",
  "templateBody": {
    "Version": "2",
    "Buildings": [
      {
        "Floors": [
          {
            "Floorplan": {
              "Footprint": {
                "DesiredAspectRatio": {
                  "x": 1,
                  "y": 1
                }
              },
              "Ceiling": {
                "Height": 3
              },
              "Rooms": [
                {
                  "Type": "Bedroom",
                  "Name": "Bedroom",
                  "DesiredShape": {
                    "Area": 25,
                    "AspectRatio": {
                      "x": 1,
                      "y": 1.2
                    }
                  }
                },
                {
                  "Type": "Living",
                  "Name": "Living room",
                  "DesiredShape": {
                    "Area": 30,
                    "AspectRatio": {
                      "x": 1,
                      "y": 1.5
                    }
                  }
                },
                {
                  "Type": "Bathroom",
                  "Name": "Bathroom",
                  "DesiredShape": {
                    "Area": 10,
                    "AspectRatio": {
                      "x": 1,
                      "y": 1.5
                    }
                  }
                },
                {
                  "Type": "Kitchen",
                  "Name": "Kitchen",
                  "DesiredShape": {
                    "Area": 15,
                    "AspectRatio": {
                      "x": 1.5,
                      "y": 1
                    }
                  }
                }
              ],
              "DesiredConnections": [
                {
                  "Location": [
                    "Bathroom",
                    "Living room"
                  ],
                  "ConnectionType": "Doorway"
                },
                {
                  "Location": [
                    "Living room",
                    "Kitchen"
                  ],
                  "ConnectionType": "Opening"
                },
                {
                  "Location": [
                    "Bedroom",
                    "Living room"
                  ],
                  "ConnectionType": "Doorway"
                }
              ]
            },
            "Interior": {
              "Flooring": {
                "MaterialSets": [
                  {
                    "Name": "Floorboard room types",
                    "TargetSet": {
                      "RoomTypes": [
                        "Kitchen"
                      ]
                    },
                    "SampleSet": {
                      "MaterialTypes": [
                        "Floorboards"
                      ]
                    }
                  },
                  {
                    "Name": "Carpet room types",
                    "TargetSet": {
                      "RoomTypes": [
                        "Living",
                        "Bedroom"
                      ]
                    },
                    "SampleSet": {
                      "MaterialTypes": [
                        "Carpet"
                      ]
                    }
                  },
                  {
                    "Name": "Bathroom",
                    "TargetSet": {
                      "RoomNames": [
                        "Bathroom"
                      ]
                    },
                    "SampleSet": {
                      "MaterialTypes": [
                        "Parquetry"
                      ]
                    }
                  }
                ]
              },
              "Walls": {
                "MaterialSets": [
                  {
                    "Name": "Brick room types",
                    "TargetSet": {
                      "RoomTypes": [
                        "Living"
                      ]
                    },
                    "SampleSet": {
                      "MaterialTypes": [
                        "Brick"
                      ]
                    }
                  },
                  {
                    "Name": "Tiles room types",
                    "TargetSet": {
                      "RoomTypes": [
                        "Bathroom"
                      ]
                    },
                    "SampleSet": {
                      "MaterialTypes": [
                        "Tiles"
                      ]
                    }
                  }
                ]
              },
              "Furniture": {
                "FurnitureArrangements": [
                  {
                    "Name": "Dense furniture room types",
                    "TargetSet": {
                      "RoomTypes": [
                        "Living",
                        "Bedroom",
                        "Kitchen",
                        "Bathroom"
                      ]
                    },
                    "DesiredSpatialDensity": "Dense"
                  }
                ]
              }
            }
          }
        ]
      }
    ]
  }
}
)***";
//
}


using goblib::json::ElementPath;
using goblib::json::ElementValue;

class RmHandler : public goblib::json::Handler
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

TEST(JSON, RoboMaker)
{
    RmHandler handler;
    goblib::json::StreamingParser parser(&handler);
    for(auto& e: rm_json)
    {
        parser.parse(e);
    }
}

