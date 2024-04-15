
#include "Config.h"

#include "editor.h"
#include <iostream>
#include "IconsForkAwesome.h"
#include <editShortcuts/editShortcuts.h>
#include <safeSave/safeSave.h>
#include "settings/shortcutApi.h"


#define DOCK_MAIN_WINDOW_SHORTCUT ICON_FK_EYE_SLASH " Hide main window"
#define LOGS_SHORTCUT ICON_FK_COMMENT_O " Logs window"
#define EDIT_SHORTCUTS ICON_FK_PENCIL_SQUARE " Edit shortcuts window"
#define CONTAINERS_SHORTCUTS ICON_FK_MICROCHIP " Containers window"
#define RELOAD_DLL_SHORTCUTS ICON_FK_REFRESH " Reload dll"
#define TRANSPARENT_EDITOR_WINDOW ICON_FK_EYE " Transparent Editor window"
#define ASSET_MANAGER_WINDOW ICON_FK_FILES_O " Asset manager"


void samurai::Editor::init(samurai::ShortcutManager &shortcutManager, samurai::samuraiImgui::ImGuiIdsManager &imguiIDManager)
{

	shortcutManager.registerShortcut(DOCK_MAIN_WINDOW_SHORTCUT, "Ctrl+Alt+D", &optionsFlags.hideMainWindow);
	shortcutManager.registerShortcut(LOGS_SHORTCUT, "Ctrl+L", &windowFlags.logsWindow);
	shortcutManager.registerShortcut(EDIT_SHORTCUTS, "", &windowFlags.editShortcutsWindow);
	shortcutManager.registerShortcut(CONTAINERS_SHORTCUTS, "Ctrl+M", &windowFlags.containerManager);
	shortcutManager.registerShortcut(RELOAD_DLL_SHORTCUTS, "Ctrl+Alt+R", &shouldReloadDll);
	shortcutManager.registerShortcut(TRANSPARENT_EDITOR_WINDOW, "Ctrl+Alt+T", &windowFlags.transparentWindow);
	shortcutManager.registerShortcut(ASSET_MANAGER_WINDOW, "Ctrl+Alt+A", &windowFlags.assetManagerWindow);

	imguiId = imguiIDManager.getImguiIds(1);

	logWindow.init(imguiIDManager);
	editShortcutsWindow.init(imguiIDManager);
	containersWindow.init(imguiIDManager);
	assetManagerWindow.init(imguiIDManager);

	if (sfs::safeLoad(&optionsFlags, sizeof(optionsFlags), SAMURAI_ENGINE_SAVES_PATH "options", false) != sfs::noError)
	{
		optionsFlags = {};
	}

	if (sfs::safeLoad(&windowFlags, sizeof(windowFlags), SAMURAI_ENGINE_SAVES_PATH "window", false) != sfs::noError)
	{
		windowFlags = {};
	}
	

}



void samurai::Editor::update(const samurai::Input &input,
                          samurai::ShortcutManager &shortcutManager, samurai::LogManager &logs,
                          samurai::LoadedDll &loadedDll
                          , samurai::samuraiImgui::ImGuiIdsManager &imguiIDsManager, samurai::ContainerManager& containerManager)
{

	if (!optionsFlags.hideMainWindow)
	{

#pragma region docking space init
		ImGuiWindowFlags mainWindowFlags = ImGuiWindowFlags_MenuBar;
		//if (optionsFlags.hideMainWindow)
		{
			mainWindowFlags = ImGuiWindowFlags_MenuBar |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_NoBackground |
				ImGuiWindowFlags_NoTitleBar;

			ImVec2 vWindowSize = ImGui::GetMainViewport()->Size;
			ImVec2 vPos0 = ImGui::GetMainViewport()->Pos;
			ImGui::SetNextWindowPos(ImVec2((float)vPos0.x, (float)vPos0.y), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2((float)vWindowSize.x, (float)vWindowSize.y), 0);
		}
#pragma endregion

#pragma region main editor window

		//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
	
	
		ImGui::PushID(imguiId);

		ImGui::SetNextWindowBgAlpha(0);

		if (ImGui::Begin(
			"Main window",
			/*p_open=*/nullptr,
			mainWindowFlags
			)
			)
		{


			//if (optionsFlags.dockMainWindow)
			{
				static const ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
				ImGuiID dockSpace = ImGui::GetID("MainWindowDockspace");
				ImGui::DockSpace(dockSpace, ImVec2(0.0f, 0.0f), dockspaceFlags);
			}

		#pragma region menu
			if (ImGui::BeginMenuBar())
			{

				if (ImGui::BeginMenu(ICON_FK_COGS " Engine"))
				{
					//todo submit tasks to the engine (usefull for this and also in gameplay)

					if (ImGui::MenuItem(ICON_FK_REFRESH " Reload dll",
						shortcutManager.getShortcut(RELOAD_DLL_SHORTCUTS), nullptr))
					{
						shouldReloadDll = true;
					}


					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Options"))
				{


					ImGui::MenuItem(DOCK_MAIN_WINDOW_SHORTCUT,
						shortcutManager.getShortcut(DOCK_MAIN_WINDOW_SHORTCUT), &optionsFlags.hideMainWindow);

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu(ICON_FK_WINDOW_MAXIMIZE " Windows"))
				{
					ImGui::MenuItem(samurai::LogWindow::ICON_NAME,
						shortcutManager.getShortcut(LOGS_SHORTCUT), &windowFlags.logsWindow);

					ImGui::MenuItem(samurai::ContainersWindow::ICON_NAME,
						shortcutManager.getShortcut(CONTAINERS_SHORTCUTS), &windowFlags.containerManager);

					ImGui::MenuItem(samurai::AssetManagerWindow::ICON_NAME,
						shortcutManager.getShortcut(ASSET_MANAGER_WINDOW), &windowFlags.assetManagerWindow);


					ImGui::EndMenu();

				}

				if (ImGui::BeginMenu(ICON_FK_COG " Settings"))
				{
					ImGui::MenuItem(samurai::ShortcutsWindow::ICON_NAME,
						shortcutManager.getShortcut(EDIT_SHORTCUTS), &windowFlags.editShortcutsWindow);

					samurai::samuraiImgui::displayMemorySizeToggle();

					ImGui::MenuItem(TRANSPARENT_EDITOR_WINDOW,
						shortcutManager.getShortcut(TRANSPARENT_EDITOR_WINDOW), 
						&windowFlags.transparentWindow);

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
		#pragma endregion

		}
		ImGui::End();

		ImGui::PopID();

	#pragma endregion

	}

	if (windowFlags.transparentWindow)
	{
		ImGuiStyle &style = ::ImGui::GetStyle();
		style.Colors[ImGuiCol_WindowBg].w = 0.f;
	}
	else
	{
		ImGuiStyle &style = ::ImGui::GetStyle();
		style.Colors[ImGuiCol_WindowBg].w = 1.f;
	}

#pragma region log window
	if (windowFlags.logsWindow)
	{
		logWindow.update(logs, windowFlags.logsWindow);
	}
#pragma endregion

#pragma region shortcuts window
	if (windowFlags.editShortcutsWindow)
	{
		editShortcutsWindow.update(shortcutManager, windowFlags.editShortcutsWindow);
	}
#pragma endregion

#pragma region containers window
	if (windowFlags.containerManager)
	{
		containersWindow.update(logs, windowFlags.containerManager, 
			loadedDll, containerManager, imguiIDsManager);
	}
#pragma endregion

#pragma region asset manager window

	if (windowFlags.assetManagerWindow)
	{
		assetManagerWindow.update(windowFlags.assetManagerWindow, containerManager, loadedDll, logs, imguiIDsManager);
	}

#pragma endregion



}

void samurai::Editor::saveFlagsData()
{

	sfs::safeSave(&optionsFlags, sizeof(optionsFlags), SAMURAI_ENGINE_SAVES_PATH "options", false);
	sfs::safeSave(&windowFlags, sizeof(windowFlags), SAMURAI_ENGINE_SAVES_PATH "window", false);


}


