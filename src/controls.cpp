#include "controls.h"
#include <cstdint>

Controls::Controls(int index)
: index(index)
, up("up" + sp::string(index))
, down("down" + sp::string(index))
, left("left" + sp::string(index))
, right("right" + sp::string(index))
, primary_action("primary_action" + sp::string(index))
, secondary_action("secondary_action" + sp::string(index))
, unknown2("unknown2_" + sp::string(index))
, self_destruct("self_destruct_" + sp::string(index))
, replay_time("replay_time_" + sp::string(index))
, replay_fuel("replay_fuel_" + sp::string(index))
, start("start" + sp::string(index))
{
    if (index == 0)
    {
        up.setKeys({"up", "keypad 8"});
        down.setKeys({"down", "keypad 2"});
        left.setKeys({"left", "keypad 4"});
        right.setKeys({"right", "keypad 6"});
        
        primary_action.setKey("space");
        secondary_action.setKeys({"z", "Menu"});
        
        unknown2.setKey("x");
        self_destruct.setKeys({"c", "AC Back"}); //"AC Back" is the back key in android
        replay_time.setKey("v");
        replay_fuel.setKey("b");
        
        start.setKey("1");
    }

    if (index == 1)
    {
        up.setKey("w");
        down.setKey("s");
        left.setKey("a");
        right.setKey("d");
        
        primary_action.setKey("q");
        secondary_action.setKey("e");
        
        unknown2.setKey("r");
        self_destruct.setKey("f");
        replay_time.setKey("t");
        replay_fuel.setKey("g");
        
        start.setKey("2");
    }
}
