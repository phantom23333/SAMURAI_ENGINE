#include "gui.h"



void *samurai::samuraiImgui::imguiCustomAlloc(size_t sz, void *user_data)
{
	samurai::memory::CustomAllocator *allocator = (samurai::memory::CustomAllocator *)user_data;
	SAMURAI_DEVELOPMENT_ONLY_ASSERT(allocator, "no allocator for imgui");

	return allocator->allocate(sz);
}

void samurai::samuraiImgui::imguiCustomFree(void *ptr, void *user_data)
{
	samurai::memory::CustomAllocator *allocator = (samurai::memory::CustomAllocator *)user_data;
	SAMURAI_DEVELOPMENT_ONLY_ASSERT(allocator, "no allocator for imgui");

	allocator->free(ptr);
}

void samurai::samuraiImgui::setImguiAllocator(samurai::memory::CustomAllocator &allocator)
{
	::ImGui::SetAllocatorFunctions(imguiCustomAlloc, imguiCustomFree, &allocator);
}


void samurai::samuraiImgui::initImgui(samurai::Context &pikaContext)
{
	setImguiAllocator(pikaContext.imguiAllocator);

	auto context = ::ImGui::CreateContext();
	//ImGui::StyleColorsDark();
	imguiThemes::embraceTheDarkness();

	ImGuiIO &io = ::ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	ImGuiStyle &style = ::ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		//style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 0.f;
		style.Colors[ImGuiCol_DockingEmptyBg].w = 0.f;
	}

#if !SAMURAI_SDL	
	ImGui_ImplGlfw_InitForOpenGL(pikaContext.wind, true);
	ImGui_ImplOpenGL3_Init("#version 330");
#else
	ImGui_ImplSDL2_InitForSDLRenderer(pikaContext.wind, pikaContext.renderer );
	ImGui_ImplSDLRenderer2_Init(pikaContext.renderer);
#endif

	pikaContext.ImGuiContext = context;


	//https://pixtur.github.io/mkdocs-for-imgui/site/FONTS/
	//https://github.com/juliettef/IconFontCppHeaders
	//https://fontawesome.com/v4/icons/
	io.Fonts->AddFontFromFileTTF(SAMURAI_RESOURCES_PATH "arial.ttf", 16);


	ImFontConfig config;
	config.MergeMode = true;
	config.GlyphMinAdvanceX = 16.0f; // Use if you want to make the icon monospaced
	static const ImWchar icon_ranges[] = {ICON_MIN_FK, ICON_MAX_FK, 0};
	io.Fonts->AddFontFromFileTTF(SAMURAI_RESOURCES_PATH "fontawesome-webfont.ttf", 16.0f, &config, icon_ranges);

	{
		ImVector<ImWchar> ranges;
		ImFontGlyphRangesBuilder builder;
		builder.AddChar(0xf016);//ICON_FK_FILE_O
		builder.AddChar(0xf114);//ICON_FK_FOLDER_O
		builder.BuildRanges(&ranges);

		io.Fonts->AddFontFromFileTTF(SAMURAI_RESOURCES_PATH "fontawesome-webfont.ttf", 150, 0, ranges.Data);
	}
	io.Fonts->Build();


}

void samurai::samuraiImgui::setImguiContext(Context pikaContext)
{
	::ImGui::SetCurrentContext(pikaContext.ImGuiContext);
}

void samurai::samuraiImgui::imguiStartFrame(Context pikaContext)
{
	setImguiContext(pikaContext);
#if !SAMURAI_SDL
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(::ImGui::GetWindowViewport());
#else
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
#endif

}


void samurai::samuraiImgui::imguiEndFrame(Context pikaContext)
{
	setImguiContext(pikaContext);
	::ImGui::Render();
	int display_w = 0, display_h = 0;

#if !SAMURAI_SDL
	glfwGetFramebufferSize(pikaContext.wind, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
#else
	SDL_GetWindowSize(pikaContext.wind, &display_w, &display_h);
	SDL_Rect viewport{0,0,display_w,display_h};
	SDL_RenderSetViewport(pikaContext.renderer, &viewport);
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
#endif

	ImGuiIO &io = ::ImGui::GetIO();

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		//GLFWwindow *backup_current_context = glfwGetCurrentContext();
		//ImGui::UpdatePlatformWindows();
		//ImGui::RenderPlatformWindowsDefault();
		//glfwMakeContextCurrent(backup_current_context);

		::ImGui::UpdatePlatformWindows();
		::ImGui::RenderPlatformWindowsDefault();
#if !SAMURAI_SDL
		pikaContext.glfwMakeContextCurrentPtr(pikaContext.wind); //idea create a class with some functions
#endif
	}
}

void samurai::samuraiImgui::addErrorSymbol()
{
	::ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
	::ImGui::Text(ICON_FK_TIMES_CIRCLE " ");
	::ImGui::PopStyleColor();
}

void samurai::samuraiImgui::addWarningSymbol()
{
	::ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
	::ImGui::Text(ICON_FK_EXCLAMATION_TRIANGLE " ");
	::ImGui::PopStyleColor();
}

//https://github.com/ocornut/imgui/discussions/3862
void samurai::samuraiImgui::alignForWidth(float width, float alignment)
{
	ImGuiStyle &style = ::ImGui::GetStyle();
	float avail = ::ImGui::GetContentRegionAvail().x;
	float off = (avail - width) * alignment;
	if (off > 0.0f)
		::ImGui::SetCursorPosX(::ImGui::GetCursorPosX() + off);
}

static int sizesType = 0;

void samurai::samuraiImgui::displayMemorySizeValue(size_t value)
{

	switch (sizesType)
	{
	case 0:
		ImGui::Text("%" IM_PRIu64 " (bytes)", value);
		break;
	case 1:
		ImGui::Text("%f (KB)", samurai::BYTES_TO_KB(value));
		break;
	case 2:
		ImGui::Text("%f (MB)", samurai::BYTES_TO_MB(value));
		break;
	case 3:
		ImGui::Text("%f (GB)", samurai::BYTES_TO_GB(value));
		break;
	default:
		break;
	}

}

void samurai::samuraiImgui::displayMemorySizeToggle()
{
	ImGui::Combo("Sizes type##samurai", &sizesType, "Bytes\0KB\0MB\0GB\0");
}



