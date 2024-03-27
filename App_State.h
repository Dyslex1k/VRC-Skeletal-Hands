#pragma once

struct controller_State {
	bool is_connected;
};

struct App_State
{
public:
	controller_State left;
	controller_State right;
};

