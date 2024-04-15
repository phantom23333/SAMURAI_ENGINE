#pragma once
#include <unordered_map>
#include <string>

#include "input/input.h"


namespace samurai
{

	struct ShortcutManager
	{
		struct Shortcut
		{
			std::string shortcut = "";
			bool *toggle = 0;
			bool editable = true;
		};

		void update(const samurai::Input &input);

		std::unordered_map<std::string, Shortcut> registeredShortcuts;

		bool registerShortcut(const char *name, const char *s, bool *toggle, bool editable = 1);

		const char *getShortcut(const char *name);
	};

bool shortcut(const samurai::Input &input, const char *shortcut);

bool MenuItem(const samurai::Input &input, const char *label, const char *shortcut, bool *p_selected, bool enabled = true);

std::string normalizeShortcutName(const char *shortcut);

void initShortcutApi();

}
