#include "shortcutApi.h"
#include <stringManipulation/stringManipulation.h>
#include <iostream>
#include <set>
#include <imgui.h>

struct Mapping
{
	short normal = 0;
	short imgui = 0;
};

//todo remove global things that allocate memory
std::unordered_map<std::string, Mapping> buttonMapping;

void samurai::initShortcutApi()
{
#if !SAMURAI_SDL
	buttonMapping =
	{
		{"a", {Button::A, ImGuiKey_A}},
		{ "b", {Button::B, ImGuiKey_B} },
		{ "c", {Button::C, ImGuiKey_C} },
		{ "d", {Button::D, ImGuiKey_D} },
		{ "e", {Button::E, ImGuiKey_E} },
		{ "f", {Button::F, ImGuiKey_F} },
		{ "g", {Button::G, ImGuiKey_G} },
		{ "h", {Button::H, ImGuiKey_H} },
		{ "i", {Button::I, ImGuiKey_I} },
		{ "j", {Button::J, ImGuiKey_J} },
		{ "k", {Button::K, ImGuiKey_K} },
		{ "l", {Button::L, ImGuiKey_L} },
		{ "m", {Button::M, ImGuiKey_M} },
		{ "n", {Button::N, ImGuiKey_N} },
		{ "o", {Button::O, ImGuiKey_O} },
		{ "p", {Button::P, ImGuiKey_P} },
		{ "q", {Button::Q, ImGuiKey_Q} },
		{ "r", {Button::R, ImGuiKey_R} },
		{ "s", {Button::S, ImGuiKey_S} },
		{ "t", {Button::T, ImGuiKey_T} },
		{ "u", {Button::U, ImGuiKey_U} },
		{ "v", {Button::V, ImGuiKey_V} },
		{ "w", {Button::W, ImGuiKey_W} },
		{ "x", {Button::X, ImGuiKey_X} },
		{ "y", {Button::Y, ImGuiKey_Y} },
		{ "z", {Button::Z, ImGuiKey_Z} },
		{ "0", {Button::NR0, ImGuiKey_0}}, { "1", {Button::NR1, ImGuiKey_1} }, { "2", {Button::NR2, ImGuiKey_2} }, { "3", {Button::NR3, ImGuiKey_3} },
		{ "4", {Button::NR4, ImGuiKey_0}}, { "5", {Button::NR5, ImGuiKey_5} }, { "6", {Button::NR6, ImGuiKey_6} }, { "7", {Button::NR7, ImGuiKey_7} },
		{ "8", {Button::NR8, ImGuiKey_8}}, { "9", {Button::NR9, ImGuiKey_9} },
		{ "space", {Button::Space , ImGuiKey_Space}},
		{ "enter", {Button::Enter, ImGuiKey_Enter} },
		{ "escape", {Button::Escape, ImGuiKey_Escape} },
		{ "esc", {Button::Escape, ImGuiKey_Escape} },
		{ "up", {Button::Up, ImGuiKey_UpArrow} },
		{ "down", {Button::Down , ImGuiKey_DownArrow}},
		{ "left", {Button::Left , ImGuiKey_LeftArrow}},
		{ "right", {Button::Right , ImGuiKey_RightArrow}},
		{ "ctrl", {Button::LeftCtrl , ImGuiKey_LeftCtrl}},
		{ "tab", {Button::Tab , ImGuiKey_Tab}},
		{ "alt", {Button::LeftAlt , ImGuiKey_LeftAlt}},

	};
#else
	buttonMapping = {

	};
#endif
}



namespace samurai
{

std::vector<std::string> tokenizeShortcutSimple(const char *shortcut)
{
	char data[256] = {};
	samurai::removeCharacters(data, shortcut, "\n \t\r\v", sizeof(data));
	samurai::toLower(data, data, sizeof(data));

	auto token = samurai::split(data, '+');

	return token;
};


std::vector<std::string> tokenizeShortcutNormalized(const char *shortcut)
{
	
	auto token = tokenizeShortcutSimple(shortcut);

	std::set<std::string> tokenSet;

	for (auto &i : token)
	{

		auto it = buttonMapping.find(i);

		if (it != buttonMapping.end())
		{
			tokenSet.insert(it->first);
		}
	}

	std::vector<std::string> ret;
	ret.reserve(tokenSet.size());

	for (auto &i : tokenSet)
	{
		ret.push_back(std::move(i));
	}

	return ret;

}

std::string normalizeShortcutName(const char *shortcut)
{
	auto t = tokenizeShortcutNormalized(shortcut);

	std::string ret = "";
	for (int i = 0; i < t.size(); i++)
	{
		t[i][0] = std::toupper(t[i][0]);

		ret += t[i];

		if (i < t.size()-1)
		{
			ret += "+";
		}
	}
	
	return ret;
}



//todo shortcut should rely on glfw backend when imgui is disabeled in production build
bool shortcut(const samurai::Input &input, const char *shortcut)
{
	auto token = tokenizeShortcutSimple(shortcut);
	
	if (token.empty()) { return 0; }


	bool pressed = false;

	if (0)
	{	//noraml implementation
		for (auto &t : token)
		{
			auto it = buttonMapping.find(t);
			if (it != buttonMapping.end())
			{
				if (input.buttons[it->second.normal].pressed())
				{
					pressed = true;
				}
				else if (!input.buttons[it->second.normal].held())
				{
					return false;
				}
			}
		}
	}
	else
	{	//imgui backend
		for (auto &t : token)
		{
			auto it = buttonMapping.find(t);
			if (it != buttonMapping.end())
			{
				if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(it->second.imgui), false))
				{
					pressed = true;
				}
				else if (!ImGui::IsKeyDown(static_cast<ImGuiKey>(it->second.imgui)))
				{
					return false;
				}
			}
		}
	}



	return pressed;
}

bool MenuItem(const samurai::Input &input, const char *label, const char *shortcut, bool *p_selected, bool enabled)
{
	if (samurai::shortcut(input, shortcut))
	{
		*p_selected = !*p_selected;
	}

	return ImGui::MenuItem(label, shortcut, p_selected, enabled);
}



void ShortcutManager::update(const samurai::Input &input)
{
	for (auto &i : registeredShortcuts)
	{
		if (shortcut(input, i.second.shortcut.c_str()))
		{
			*i.second.toggle = !*i.second.toggle;
		}
	}

}

bool ShortcutManager::registerShortcut(const char *name, const char *s, bool *toggle, bool editable)
{

	if (registeredShortcuts.find(name) != registeredShortcuts.end())
	{
		//todo log error
		return 0;
	}
	else
	{
		registeredShortcuts[name] 
			= Shortcut{std::move(normalizeShortcutName(s)), toggle, editable};
		return 1;
	}

}

const char *ShortcutManager::getShortcut(const char *name)
{
	auto it = registeredShortcuts.find(name);

	if (it == registeredShortcuts.end()) { return ""; }
	else 
	{ return it->second.shortcut.c_str(); };

}

}
