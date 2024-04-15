
#include "containersWindow.h"
#include <windowGraphic/gui.h>
#include "imguiComboSearch.h"
#include <imgui_spinner.h>

void samurai::ContainersWindow::init(samurai::samuraiImgui::ImGuiIdsManager &imguiIdsManager)
{
	imguiIds = imguiIdsManager.getImguiIds(10);
}

void samurai::ContainersWindow::update(samurai::LogManager &logManager, bool &open, samurai::LoadedDll &loadedDll,
	samurai::ContainerManager &containerManager, samurai::samuraiImgui::ImGuiIdsManager &imguiIdsManager)
{
	ImGui::PushID(imguiIds);


	if (!ImGui::Begin(ICON_NAME, &open))
	{
		ImGui::End();
		ImGui::PopID();
		return;
	}


	static int selected = 0;
	
	std::string selectedContainerToLaunch = "";

	{
		ImGui::BeginGroup();
		if (ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())))
		{
			if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_Reorderable))
			{
				if (ImGui::BeginTabItem(ICON_FK_PLUS_SQUARE_O " Create container"))
				{
					ImGui::Text("Available containers");
					ImGui::Separator();

					//left
					ImGui::PushID(imguiIds + 1);
					ImGui::BeginGroup();
					{

						static char filter[256] = {};

						std::vector<std::string> containerNames;
						containerNames.reserve(loadedDll.containerInfo.size());

						for (auto& i : loadedDll.containerInfo)
						{
							containerNames.push_back(i.containerName);
						}

						auto contentSize = ImGui::GetItemRectSize();
						contentSize.y -= ImGui::GetFrameHeightWithSpacing();
						contentSize.x /= 2;

						ImGui::ListWithFilter("##list box container info", &itemCurrentAvailableCOntainers, filter, sizeof(filter),
							containerNames, contentSize);


					}
					ImGui::EndGroup();
					ImGui::PopID();

					ImGui::SameLine();

					//right
					ImGui::PushID(imguiIds + 2);
					ImGui::BeginGroup();
					{
						if (itemCurrentAvailableCOntainers < loadedDll.containerInfo.size())
						{
							auto& c = loadedDll.containerInfo[itemCurrentAvailableCOntainers];

							ImGui::Text("Container info: %s", c.containerName.c_str());
							ImGui::Separator();

							selectedContainerToLaunch = c.containerName;

							ImGui::Text(ICON_FK_PIE_CHART " Memory");
							ImGui::Separator();

							ImGui::NewLine();

#pragma region total memory requirement
							size_t totalHeapMemory = c.containerStaticInfo.defaultHeapMemorySize;
							for (auto i : c.containerStaticInfo.bonusAllocators)
							{
								totalHeapMemory += i;
							}

							size_t totalMemory = totalHeapMemory + c.containerStructBaseSize;

							ImGui::Text("Total Memory requirement: ");
							ImGui::SameLine();
							samurai::samuraiImgui::displayMemorySizeValue(totalMemory);

							ImGui::Text("Total Heap requirement: ");
							ImGui::SameLine();
							samurai::samuraiImgui::displayMemorySizeValue(totalHeapMemory);
#pragma endregion

							ImGui::NewLine();

							ImGui::Text("Static Memory requirement: ");
							ImGui::SameLine();
							samurai::samuraiImgui::displayMemorySizeValue(c.containerStructBaseSize);

							ImGui::Text("Default Heap Memory requirement: ");
							ImGui::SameLine();
							samurai::samuraiImgui::displayMemorySizeValue(c.containerStaticInfo.defaultHeapMemorySize);

							ImGui::Text("Other Heap Memory Allocators count: ");
							ImGui::SameLine();
							samurai::samuraiImgui::displayMemorySizeValue(c.containerStaticInfo.bonusAllocators.size());


							if (!c.containerStaticInfo.bonusAllocators.empty())
							{
								if (ImGui::BeginChild("##heap allocators",
									{ 0, 100 }, true, ImGuiWindowFlags_AlwaysVerticalScrollbar))
								{

									for (auto i : c.containerStaticInfo.bonusAllocators)
									{
										samurai::samuraiImgui::displayMemorySizeValue(i);
									}


								}
								ImGui::EndChild();
							}



							ImGui::NewLine();
							ImGui::Text(ICON_FK_PLUS_SQUARE_O " Launch");
							ImGui::Separator();

							ImGui::NewLine();

							if (!selectedContainerToLaunch.empty()
								&& ImGui::Button(ICON_FK_PLAY " Launch a default configuration"))
							{
								if (createAtSpecificMemoryRegion)
								{
									containerManager.createContainer(selectedContainerToLaunch, loadedDll,
										logManager, imguiIdsManager, std::string(), samurai::TB(1));
								}
								else
								{
									containerManager.createContainer(selectedContainerToLaunch, loadedDll, logManager,
										imguiIdsManager, std::string());
								}
							}

							ImGui::Checkbox("allocate at specific memory region", &createAtSpecificMemoryRegion);


						}
						else
						{
							ImGui::Text("Container info:");
							ImGui::Separator();
						}

					}
					ImGui::EndGroup();
					ImGui::PopID();



					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem(ICON_FK_MICROCHIP " Running containers"))
				{


					ImGui::Text("Running containers");

					ImGui::SameLine();

					ImGui::Separator();

					//left
					std::vector<samurai::containerId_t> containerIds;
					std::vector<std::string> containerNames;

					ImGui::PushID(imguiIds + 3);
					ImGui::BeginGroup();
					{

						containerIds.reserve(containerManager.runningContainers.size());
						containerNames.reserve(containerManager.runningContainers.size());

						for (auto& i : containerManager.runningContainers)
						{
							containerIds.push_back(i.first);
							containerNames.push_back(
								std::string(i.second.baseContainerName) + ": " + std::to_string(i.first));
						}

						auto contentSize = ImGui::GetItemRectSize();
						contentSize.y -= ImGui::GetFrameHeightWithSpacing();
						contentSize.x /= 2;

						ImGui::ListWithFilter("##list box container info", &itemCurrentCreatedContainers,
							filterContainerInfo, sizeof(filterContainerInfo),
							containerNames, contentSize);


					}
					ImGui::EndGroup();
					ImGui::PopID();

					ImGui::SameLine();

					//right
					ImGui::PushID(imguiIds + 4);
					ImGui::BeginGroup();
					{
						if (itemCurrentCreatedContainers < containerIds.size())
						{
							auto& c = containerManager.runningContainers[containerIds[itemCurrentCreatedContainers]];


							ImGui::Text("Running container: %s #%u", c.baseContainerName, containerIds[itemCurrentCreatedContainers]);
							ImGui::Separator();

#pragma region buttons

							//calculate cursor pos for 3 buttons
							{
								ImGuiStyle& style = ImGui::GetStyle();
								float width = 0.0f;
								width += ImGui::CalcTextSize(ICON_FK_PAUSE).x;
								width += style.ItemSpacing.x;
								width += ImGui::CalcTextSize(ICON_FK_PAUSE).x;
								width += style.ItemSpacing.x;
								width += ImGui::CalcTextSize(ICON_FK_PAUSE).x;

								samurai::samuraiImgui::alignForWidth(width);
							}



							ImGui::SameLine();

							bool stopped = false;

							if (ImGui::Button(ICON_FK_STOP))
							{
								//todo mabe defer here when api is made
								containerManager.destroyContainer(containerIds[itemCurrentCreatedContainers], loadedDll, logManager);
								stopped = true;
							}
							if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
							{
								ImGui::SetTooltip("Stop container.");
							}

							ImGui::SameLine();

							if (ImGui::Button(ICON_FK_EJECT))
							{
								containerManager.forceTerminateContainer(containerIds[itemCurrentCreatedContainers], loadedDll, logManager);
								stopped = true;
							}

							if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
							{
								ImGui::SetTooltip("Force stop container (not recomended).\nThis won't call any destructors.");
							}
						}
					}
					ImGui::EndGroup();
					ImGui::PopID();

					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			
			ImGui::EndChild();
		}
		

		ImGui::EndGroup();
	}
		ImGui::End();
		ImGui::PopID();
	}
