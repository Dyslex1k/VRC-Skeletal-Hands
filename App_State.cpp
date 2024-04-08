#include "App_State.h"
App_State::App_State() :
	actionSet({}),
	actionSetHandle(0),
	left({}),
	right({})
{
	oscSender = std::make_unique<hekky::osc::UdpSender>("127.0.0.1", 9000, 9001);
}