#include "assetManagerWindow.h"

#if SAMURAI_WINDOWS
#define NOMINMAX
#include <Windows.h>
#endif


namespace samurai
{

	void AssetManagerWindow::init(samurai::samuraiImgui::ImGuiIdsManager &idManager)
	{
		imguiId = idManager.getImguiIds();
	}
	
	void AssetManagerWindow::update(bool &open, ContainerManager &containerManager, LoadedDll &currentDll,
		samurai::LogManager &logManager, samurai::samuraiImgui::ImGuiIdsManager &imguiIDsManager)
	{
		ImGui::PushID(imguiId);


		if (!ImGui::Begin(ICON_NAME, &open))
		{
			ImGui::End();
			ImGui::PopID();
			return;
		}

		//todo: for all windows
		ImGui::SetWindowSize({300,100}, ImGuiCond_FirstUseEver);

		if(std::filesystem::equivalent(currentPath, SAMURAI_RESOURCES_PATH) || searchText[0] != '\0')
		{
			ImGui::BeginDisabled(1);
		}
		else
		{
			ImGui::BeginDisabled(0);
		}

		if (ImGui::Button(ICON_FK_ARROW_UP))
		{
			currentPath = currentPath.parent_path();
		}

		ImGui::EndDisabled();

		ImGui::SameLine();

		ImGui::InputText("Search file", searchText, sizeof(searchText));

		ImGui::SameLine();

		if (ImGui::Button("Open resources folder"))
		{
		#if SAMURAI_WINDOWS
			ShellExecuteA(NULL, "open", SAMURAI_RESOURCES_PATH, NULL, NULL, SW_RESTORE);
		#endif
		}

		std::string longPath = currentPath.string();
		std::string root = SAMURAI_RESOURCES_PATH;
		std::string enginePath = "SAMURAI_RESOURCES_PATH/";
		if (longPath.size() > root.size())
		{
			enginePath += (longPath.c_str() + root.size());
		}

		for (char &c : enginePath)
		{
			if (c == '\\')
			{
				c = '/';
			}
		}

		ImGui::Text(enginePath.c_str());

		ImGui::Separator();

		float contentW = ImGui::GetContentRegionAvail().x;
		const float size = 160;
		const float padding = 10;

		ImGui::Columns( std::max(1, (int)(contentW / (size + padding))), 0, false);

		//returns 1 if should break
		auto displayItem = [&](const std::filesystem::directory_entry &p) -> bool
		{
			if (ImGui::BeginChild(p.path().filename().string().c_str(), {size, size + 40}, false,
				ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
			{
				//ImGui::
				ImFontAtlas *atlas = ImGui::GetIO().Fonts;

				ImGui::PushFont(atlas->Fonts[1]);

				if (p.is_directory())
				{

					if (ImGui::Button(ICON_FK_FOLDER_O, {size ,size}))
					{
						currentPath = p;

						//todo deffer
						ImGui::PopFont();
						ImGui::EndChild();
						ImGui::Columns();
						return 1;
					}
				}
				else
				{
					if (ImGui::Button(ICON_FK_FILE_O, {size ,size}))
					{
						auto it = currentDll.containerExtensionsSupport.find(p.path().filename().extension().string());
						if (it != currentDll.containerExtensionsSupport.end())
						{
							//todo name						
							containerManager.createContainer(it->second, currentDll, logManager, imguiIDsManager, p.path().string());
						}
					}
				}

				ImGui::PopFont();

				ImGui::Text(p.path().filename().string().c_str());

				if (ImGui::BeginPopupContextWindow())
				{
					if (ImGui::Button("reveal in explorer"))
					{

					#if SAMURAI_WINDOWS
						if (p.is_directory())
						{
							ShellExecuteA(NULL, "open", p.path().string().c_str(), NULL, NULL, SW_RESTORE);
						}
						else
						{
							auto path = p.path().parent_path().string();
							ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_RESTORE);
						}
					#endif
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::Button("copy file location"))
					{
						ImGui::SetClipboardText(p.path().string().c_str());
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::Button("copy file location for engine"))
					{
						std::string s =  "SAMURAI_RESOURCES_PATH \"" + p.path().filename().string() + "\"";

						ImGui::SetClipboardText(s.c_str());
						ImGui::CloseCurrentPopup();
					}

					if (!p.is_directory())
					{
						if (ImGui::Button("open file"))
						{
						#if SAMURAI_WINDOWS
							ShellExecuteA(NULL, "open", p.path().string().c_str(), NULL, NULL, SW_RESTORE);
						#endif
							ImGui::CloseCurrentPopup();
						}
					}
					
				
					{
						auto it = currentDll.containerExtensionsSupport.find(p.path().filename().extension().string());
						if (it != currentDll.containerExtensionsSupport.end())
						{
							//todo name						
							if (ImGui::Button("Open In engine"))
							{
								containerManager.createContainer(it->second, currentDll, logManager, imguiIDsManager, p.path().string());
							}
						}
					}
					

					ImGui::EndPopup();
				}


			}

			ImGui::EndChild();

			return 0;
		};

		if (searchText[0] == '\0')
		{
			for (auto &p : std::filesystem::directory_iterator(currentPath))
			{
				if (displayItem(p))
				{
					break;
				}

				ImGui::NextColumn();

			}
		}
		else
		{
			//search filter
			for (auto &p : std::filesystem::recursive_directory_iterator(SAMURAI_RESOURCES_PATH))
			{
				if (p.is_regular_file())
				{
					auto rez = p.path().filename().string();

					if (rez.find(searchText) != std::string::npos)
					{
						if (displayItem(p))
						{
							break;
						}

						ImGui::NextColumn();
					}

				}
			}
		}

		

		ImGui::Columns(1);

		ImGui::End();
		ImGui::PopID();
	}


};

