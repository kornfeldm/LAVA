#pragma once
#ifndef FE_H
#define FE_H
struct nk_command_buffer* canvas;
struct nk_rect total_space;
std::set<std::string> advancedScanPaths;
//#define NK_INCLUDE_FIXED_TYPES
//#define NK_INCLUDE_STANDARD_IO
//#define NK_INCLUDE_STANDARD_VARARGS
//#define NK_INCLUDE_DEFAULT_ALLOCATOR
//#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
//#define NK_INCLUDE_FONT_BAKING
//#define NK_INCLUDE_DEFAULT_FONT
//#define NK_IMPLEMENTATION
//#define NK_SFML_GL2_IMPLEMENTATION
//
//#include "nuklear.h"
//#include "nuklear_sfml_gl2.h"
//
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
//
//#define WINDOW_WIDTH 1200
//#define WINDOW_HEIGHT 800
//#define IMG_COLOR nk_rgba(255, 255, 255, 255)

/* Use this function to allocate space for text below an image. see how the back arrow is displayed in the DisplayScansPage function is used
*/
struct nk_rect SubRectTextBelow(struct nk_rect *big, struct nk_rect *sub ) {
	return nk_rect(sub->x, sub->h, sub->w, big->h-sub->h+sub->x);
}

/* opening file dialog stuff */
std::string ToNarrow(const wchar_t* s, char dfault = '?',
	const std::locale& loc = std::locale())
{
	std::ostringstream stm;

	while (*s != L'\0') {
		stm << std::use_facet< std::ctype<wchar_t> >(loc).narrow(*s++, dfault);
	}
	return stm.str();
}

// global struct. prob shouldnt do this but who will see this, lets be honest
struct
{
	WNDPROC oldWndProc;
	TCHAR   szLastSelection[MAX_PATH];
	UINT    cFolders;
} g_Multi;


/*
 * Subclassed window procedure for dialog created by SHBrowseForFolder.
 */
static LRESULT CALLBACK SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_COMMAND && HIWORD(wParam) == BN_CLICKED)
	{
		TCHAR szText[50];

		if (GetWindowText((HWND)lParam, szText, 50) &&
			0 == lstrcmp(szText, TEXT("Add File/Dir")))
		{
			if (0 != lstrcmp(g_Multi.szLastSelection, TEXT("")))
			{
				g_Multi.cFolders++;

				/* User has clicked the "Add Folder" button.
				 * Add the contents of szLastSelection to your list. */
				//printf("%s\n", g_Multi.szLastSelection);
				advancedScanPaths.insert(ToNarrow(g_Multi.szLastSelection));
			}
			else
			{
				/* A non-file system folder is selected. eg. Network Neighbourhood. */
				printf("Please select a valid file folder and try again.\n");
			}

			/* Eat the BN_CLICKED message so the dialog is not closed. */
			return 0;
		}
	}

	return CallWindowProc(g_Multi.oldWndProc, hwnd, uMsg, wParam, lParam);
}


/*
 * Callback function for SHBrowseForFolder.
 */
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED:
	{
		/* Change the text for the OK button and subclass the dialog. */
		LPCTSTR startFolder = reinterpret_cast<LPCTSTR>(lpData);
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, reinterpret_cast<LPARAM>(startFolder));
		SendMessage(hwnd, BFFM_SETOKTEXT, 0, (LPARAM)L"Add File/Dir");
		g_Multi.oldWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)SubclassProc);
		break;
	}

	case BFFM_SELCHANGED:
	{
		/* Record the selection, so we can record it if the user clicks "Add Folder". */
		if (!SHGetPathFromIDList((LPCITEMIDLIST)lParam, g_Multi.szLastSelection))
			g_Multi.szLastSelection[0] = TEXT('\0');
		break;
	}
	}

	return 0;
}


/*
 * Displays a directory selection dialog that allows the user to select
 * multiple folders. CoInitialize must be called before calling this function.
 * hWnd may be NULL. Returns the number of folders that were selected.
 * This function is not thread safe. It must not be called from more than
 * one thread at a time.
 */
UINT MultiFolderSelect(HWND hWnd, LPCTSTR szTitle, CString folder=L"C:\\")
{
	LPITEMIDLIST pidl = NULL;
	BROWSEINFO   bi = { 0 };
	TCHAR        buf[MAX_PATH];

	bi.hwndOwner = hWnd;
	bi.pszDisplayName = buf;
	bi.pidlRoot = 0;
	bi.lpszTitle = szTitle;
	bi.ulFlags = BIF_BROWSEFORPRINTER | BIF_BROWSEINCLUDEFILES | BIF_BROWSEINCLUDEURLS | 
		BIF_NEWDIALOGSTYLE | BIF_SHAREABLE | 
		BIF_NONEWFOLDERBUTTON | BIF_USENEWUI;
	bi.lParam = reinterpret_cast<LPARAM>(static_cast<LPCTSTR>(folder));
	bi.lpfn = BrowseCallbackProc;
	//bi.lParam = (LPARAM)L"C:\\";

	g_Multi.cFolders = 0;
	g_Multi.szLastSelection[0] = TEXT('\0');

	if ((pidl = SHBrowseForFolder(&bi)) != NULL)
		CoTaskMemFree(pidl);

	return g_Multi.cFolders;
}

struct pics {
	const char* squareLogo;
	const char* rectLogo;
	const char* trapLogo;
	const char* scan;
	const char* history;
	const char* backArrow;
	const char* chooseScan;
	const char* triangleButton;
};

/* FE CLASS */
class FE : public LavaScan
{
private:
public:
	/* CONSTRUCTORS */
	bool init(sf::Window *win);
	FE();

	/* MEMBER VARS */
	struct nk_context* ctx;
	struct nk_command_buffer* canvas;
	// font stuff
	struct nk_font_atlas* atlas;
	struct nk_font* font;
	const char* font_path = "../Assets/font.ttf";
	//struct nk_font_config* fontCFG;// can be null so commenting out for mem
	struct nk_colorf bg;
	struct nk_colorf whiteFont;
	struct nk_rect total_space;
	// pics to be loaded (pngs)
	pics pp;
	struct nk_image scanImage;
	struct nk_image rectImage; 
	struct nk_image trapImage; 
	struct nk_image squareImage; 
	struct nk_image historyImage;
	struct nk_image backArrow;
	struct nk_image chooseScan;
	struct nk_image triangleLogo;
	// view screen switch. view ScanViews scanview member for furthger info
	unsigned int view;
	/*
		0 : logo screen
		1 : scans screen
		2 : history screen
	*/
	unsigned int m_scanViews;
	/*
		0 : plz maam pls choose a scan
		1 : coimplete scan
		2 : quick scan
		3 : advanced scan
	*/
	
	/* MEMBER FUNCTIONS */
	static struct nk_image icon_load(const char* filename, bool flip = false);
	bool dateInput();
	bool drawImage(struct nk_image *img);
	bool drawImageSubRect(struct nk_image* img, struct nk_rect* r);
	bool DrawMainPage();
	bool DrawScansPage();
	bool Display();
	bool NoScanView();
	bool CompleteScanView();
	bool viewSwap();
	bool QuickScansView();
	bool AdvancedScanView();
	bool printset(std::set<std::string> s);
	/*static LRESULT CALLBACK SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
	UINT MultiFolderSelect(HWND hWnd, LPCTSTR szTitle);*/
	void UIPrintSet(std::set<std::string> s);
};
inline struct nk_image FE::icon_load(const char* filename, bool flip)
{
	int x, y, n;
	GLuint tex;
	if (flip) {
		stbi_set_flip_vertically_on_load(true);
	}
	unsigned char* data = stbi_load(filename, &x, &y, &n, STBI_rgb_alpha);
	if (!data) std::cerr << "[SDL]: failed to load image\n";
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	return nk_image_id((int)tex);
}

inline bool FE::drawImage(struct nk_image *img)
{
	canvas = nk_window_get_canvas(this->ctx);
	total_space = nk_window_get_content_region(this->ctx);
	nk_draw_image(canvas, total_space, img, IMG_COLOR);
	return true;
}

inline bool FE::dateInput() {
	/*
	char buf[256] = {0};
	// in window
	nk_edit_string_zero_terminated (ctx, NK_EDIT_FIELD, buf, sizeof(buf) - 1, nk_filter_default);
	if (nk_button_label (ctx, "Done"))
		printf ("%s\n", buf);
	*/
	int d;
	int m;
	int y;
	std::cin >> d; // read the day
	if (std::cin.get() != '/') // make sure there is a slash between DD and MM
	{
		std::cout << "expected /\n";
		return 1;
	}
	std::cin >> m; // read the month
	if (std::cin.get() != '/') // make sure there is a slash between MM and YYYY
	{
		std::cout << "expected /\n";
		return 1;
	}
	std::cin >> y; // read the year
	std::cout << "input date: " << d << "/" << m << "/" << y << "\n";

	return true;
}

inline bool FE::drawImageSubRect(struct nk_image* img, struct nk_rect *r)
{
	canvas = nk_window_get_canvas(this->ctx);
	nk_draw_image(canvas, *r, img, IMG_COLOR);
	return true;
}

inline bool FE::Display() {
	if (this->view == 0) {
		this->DrawMainPage();
		advancedScanPaths.clear();
	}
	else if (this->view == 1)
		this->DrawScansPage();

	else {
		std::cout << "ERRRRRRORRRRR\n";
	}
	return true;
}

inline bool FE::DrawMainPage()
{
	/* LOGO */
	if (nk_begin(this->ctx, "lavalogo", nk_rect(25, 25, WINDOW_HEIGHT * .33, WINDOW_HEIGHT * .33),
		NK_WINDOW_NO_SCROLLBAR)) {
		this->drawImage(&this->squareImage);
	}
	nk_end(this->ctx);

	/* SCAN ICON */
	if (nk_begin(this->ctx, "scan", nk_rect(WINDOW_WIDTH * .2, WINDOW_HEIGHT * .49, WINDOW_HEIGHT * .3, WINDOW_HEIGHT * .3),
		NK_WINDOW_NO_SCROLLBAR)) {
		// hidden button to press behind icon
		nk_layout_row_static(ctx, WINDOW_HEIGHT * .3, WINDOW_HEIGHT * .3, 1);
		if (nk_button_label(ctx, "")) {
			//fprintf(stdout, "scans pressed\n");
			this->view = 1; // scans page
			this->m_scanViews = 0; // choose a scan sub view
			nk_clear(this->ctx);
		}
		this->drawImage(&this->scanImage);
	}
	nk_end(this->ctx);

	/* history ICON */
	if (nk_begin(this->ctx, "history", nk_rect(WINDOW_WIDTH * .6, WINDOW_HEIGHT * .49, WINDOW_HEIGHT * .3, WINDOW_HEIGHT * .3),
		NK_WINDOW_NO_SCROLLBAR)) {
		// hidden button behind icon to press
		nk_layout_row_static(ctx, WINDOW_HEIGHT * .3, WINDOW_HEIGHT * .3, 1);
		if (nk_button_label(ctx, "")) {
			fprintf(stdout, "history pressed\n");
			this->view = 2;
			nk_clear(this->ctx);
		}
		this->drawImage(&this->historyImage);
	}
	nk_end(this->ctx);
	
	return true;
}

bool FE::NoScanView() {
	int filled_width = WINDOW_WIDTH * .08 * 2 + WINDOW_WIDTH * .075; // remaining width ofscreen after side menu
	int delta = WINDOW_WIDTH - filled_width;
	/* CHOOSE A SCAN */
	struct nk_rect center = nk_rect(filled_width + delta * .2, WINDOW_HEIGHT * .20-36-50, delta * .6-50, WINDOW_HEIGHT * .6 -36+50);
	struct nk_rect centerAndText = nk_rect(center.x, center.h+90, center.w+50, 80 + 80+30); //36 for font size!
	if (nk_begin(this->ctx, "ChooseScan", center, NK_WINDOW_NO_SCROLLBAR)) {
		this->drawImageSubRect(&this->chooseScan, &center);
	}
	nk_end(this->ctx);
	/* TEXT TO CHOOSE A SCAN*/
	if (nk_begin(this->ctx, "ChooseScantxt", centerAndText, NK_WINDOW_NO_SCROLLBAR))
	{
		/*nk_layout_row_dynamic(this->ctx, 40, 1);
		nk_label_wrap(this->ctx, "                                                        ");
		nk_layout_row_dynamic(this->ctx, 80, 1);
		nk_label_wrap(this->ctx, "Chose a Scan, Please!");*/
		// old way of drawing txt...low level api
		nk_draw_text(nk_window_get_canvas(this->ctx), centerAndText, "  Choose A Scan, Please!", 24, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	return true;
}

bool FE::CompleteScanView() {
	int filled_width = WINDOW_WIDTH * .08 * 2 + WINDOW_WIDTH * .075; // remaining width ofscreen after side menu
	int delta = WINDOW_WIDTH - filled_width;

	/* quick scan header and text */
	if (nk_begin(this->ctx, "Complete Scan", nk_rect(300, WINDOW_HEIGHT * .06, 600, 300),
		NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR))
	{
		nk_layout_row_dynamic(this->ctx, 40, 1);
		nk_label_wrap(this->ctx, "                                                        ");
		nk_layout_row_dynamic(this->ctx, 80, 1);
		nk_label_wrap(this->ctx, "This scan will traverse all files on the current file-system!");
	}
	nk_end(this->ctx);

	/* SCAN NOW BUTTON AND TEXT */
	struct nk_rect scanButton = nk_rect(delta * .5, WINDOW_HEIGHT * .5, 350, 50);
	if (nk_begin(this->ctx, "Complete Scan Button", scanButton, NK_WINDOW_NO_SCROLLBAR))
	{
		nk_layout_row_static(ctx, 65, 400, 1);
		if (nk_button_label(ctx, "")) {
			//fprintf(stdout, "do complete scan pressed\n");
			this->CompleteScan();
			// scan in progress ... blah blah
			nk_clear(this->ctx);
		}
	    // draw txt 
		struct nk_rect textArea = nk_rect(scanButton.x, scanButton.y, scanButton.w, scanButton.h);
		nk_draw_text(nk_window_get_canvas(this->ctx), textArea, " Complete Scan! ", 16, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
		// draw triangle
		this->drawImageSubRect(&this->triangleLogo, &nk_rect(textArea.x+300, textArea.y, textArea.w-300, textArea.h));
	}
	nk_end(this->ctx);

	return true;
}

bool FE::QuickScansView() {
	int filled_width = WINDOW_WIDTH * .08 * 2 + WINDOW_WIDTH * .075; // remaining width ofscreen after side menu
	int delta = WINDOW_WIDTH - filled_width;

	/* quick scan header and text */
	if (nk_begin(this->ctx, "Quick Scan", nk_rect(300, WINDOW_HEIGHT * .06, 600, 300),
		NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR))
	{
		nk_layout_row_dynamic(this->ctx, 40, 1);
		nk_label_wrap(this->ctx, "                                                        ");
		nk_layout_row_dynamic(this->ctx, 120, 1);
		nk_label_wrap(this->ctx, "This scan will traverse the most commonly affected areas on your system.");
	}
	nk_end(this->ctx);

	/* SCAN NOW BUTTON AND TEXT */
	struct nk_rect scanButton = nk_rect(delta * .5, WINDOW_HEIGHT * .5, 270, 50);
	if (nk_begin(this->ctx, "Quick Scan Button", scanButton, NK_WINDOW_NO_SCROLLBAR))
	{
		nk_layout_row_static(ctx, 65, 400, 1);
		if (nk_button_label(ctx, "")) {
			//fprintf(stdout, "do quick scan pressed\n");
			this->QuickScan();
			// scan in progress ... blah blah
			nk_clear(this->ctx);
		}
		// draw txt 
		struct nk_rect textArea = nk_rect(scanButton.x, scanButton.y, scanButton.w, scanButton.h);
		nk_draw_text(nk_window_get_canvas(this->ctx), textArea, " Quick Scan! ", 13, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
		// draw triangle
		this->drawImageSubRect(&this->triangleLogo, &nk_rect(textArea.x + 220, textArea.y, textArea.w - 220, textArea.h));
	}
	nk_end(this->ctx);

	return true;

}
inline bool FE::AdvancedScanView() {
	int filled_width = WINDOW_WIDTH * .08 * 2 + WINDOW_WIDTH * .075; // remaining width ofscreen after side menu
	int delta = WINDOW_WIDTH - filled_width;

	/* advanced scan header and text */
	if (nk_begin(this->ctx, "Advanced Scan", nk_rect(300, WINDOW_HEIGHT * .06, 600, 300),
		NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR))
	{
		nk_layout_row_dynamic(this->ctx, 120, 1);
		nk_label_wrap(this->ctx, "This scan will allow you to choose where and when to scan!");
	}
	nk_end(this->ctx);

	///* checkbox date switch */
	//if (nk_begin(this->ctx, "Overview", nk_rect(600, 600, 400, 600), NK_WINDOW_DYNAMIC))
	//{
	//	static const float ratio[] = { 120, 150 };
	//	static char field_buffer[64];
	//	static char text[9][64];
	//	static int text_len[9];
	//	static char box_buffer[512];
	//	static int field_len;
	//	static int box_len;
	//	nk_flags active;

	//	//text box and enter
	//	nk_layout_row(ctx, NK_STATIC, 100, 2, ratio);
	//	//nk_label(ctx, "test:", NK_TEXT_LEFT);
	//	nk_edit_string(ctx, NK_EDIT_SIMPLE, text[0], &text_len[0], 64, nk_filter_ascii);
	//	if (nk_button_label(ctx, "Done")) {
	//		printf("%s\n", text[0]);
	//		this->printset(advancedScanPaths);
	//	}
	//		
	//}
	//nk_end(this->ctx);
	
	/* displaying shit */
	struct nk_list_view view; view.count = 2; 
	if (nk_begin(this->ctx, "Selected Files/Folders...", nk_rect(WINDOW_WIDTH - delta + 20, WINDOW_HEIGHT * .235, delta-40, 400), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_SCROLL_AUTO_HIDE | NK_WINDOW_DYNAMIC ))
	{
		int h = advancedScanPaths.size() * 28 + 28;
		if (h < 400) { h = 400; }
		nk_layout_row_dynamic(ctx, h, 1);
		if (nk_list_view_begin(ctx, &view, "test", NK_WINDOW_BORDER, 25, 2)) {
			nk_layout_row_dynamic(ctx, 25, 1);
			UIPrintSet(advancedScanPaths);
			//printset(advancedScanPaths);
			/*nk_label(this->ctx, list[0], NK_TEXT_ALIGN_LEFT);
			nk_label(this->ctx, list[1], NK_TEXT_ALIGN_LEFT);*/
			nk_list_view_end(&view);
		}
	}
	nk_end(this->ctx);

	/* add folders/files button */
	struct nk_rect AddFoF = nk_rect(WINDOW_WIDTH - delta+10, WINDOW_HEIGHT * .235+405, 330, 50);
	if (nk_begin(this->ctx, "add folders", AddFoF, NK_WINDOW_NO_SCROLLBAR))
	{
		nk_layout_row_static(ctx, 65, AddFoF.w, 1);
		if (nk_button_label(ctx, "")) {
			// openfolderdiag
			MultiFolderSelect(GetActiveWindow(), TEXT("SELECT SOME FILES/FOLDERS"));
			nk_clear(this->ctx);
		}
		// draw txt 
		struct nk_rect textArea = nk_rect(AddFoF.x, AddFoF.y, AddFoF.w, AddFoF.h);
		nk_draw_text(nk_window_get_canvas(this->ctx), textArea, "  Add Files/Dirs ", 17, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
		// draw triangle
		//this->drawImageSubRect(&this->triangleLogo, &nk_rect(textArea.x + 240, textArea.y, textArea.w - 220, textArea.h));
	}
	nk_end(this->ctx);

	/* adv scan now */
	struct nk_rect scanButton = nk_rect(AddFoF.x+125, AddFoF.y+75, 300, 50);
	if (nk_begin(this->ctx, "advscannow", scanButton, NK_WINDOW_NO_SCROLLBAR))
	{
		nk_layout_row_static(ctx, 65, scanButton.w, 1);
		if (nk_button_label(ctx, "")) {
			// run adv scan now
			if (advancedScanPaths.size() > 0)
				AdvanceScanNow(advancedScanPaths);
			nk_clear(this->ctx);
		}
		// draw txt 
		struct nk_rect textArea2 = nk_rect(scanButton.x, scanButton.y, scanButton.w, scanButton.h);
		nk_draw_text(nk_window_get_canvas(this->ctx), textArea2, "  Adv. Scan Now ", 16, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
		// draw triangle
		//this->drawImageSubRect(&this->triangleLogo, &nk_rect(textArea.x + 240, textArea.y, textArea.w - 220, textArea.h));
	}
	nk_end(this->ctx);

	/* adv scan LATER */
	struct nk_rect scanButtonLater = nk_rect(scanButton.x+scanButton.w+50, scanButton.y, scanButton.w, 50);
	if (nk_begin(this->ctx, "advscannl8r", scanButtonLater, NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_MOVABLE))
	{
		nk_layout_row_static(ctx, 65, scanButtonLater.w, 1);
		if (nk_button_label(ctx, "")) {
			// run adv scan l8r
			nk_clear(this->ctx);
		}
		// draw txt 
		struct nk_rect textArea3 = nk_rect(scanButtonLater.x, scanButtonLater.y, scanButtonLater.w, scanButtonLater.h);
		nk_draw_text(nk_window_get_canvas(this->ctx), textArea3, "  Schedule Scan ", 16, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
		// draw triangle
		//this->drawImageSubRect(&this->triangleLogo, &nk_rect(textArea.x + 240, textArea.y, textArea.w - 220, textArea.h));
	}
	nk_end(this->ctx);

	return true;
}

inline bool FE::printset(std::set<std::string> s)
{
	std::cout << "\ncontents: \n";
	for (std::string const& str : s)
	{
		std::cout << "\t" << str << "\n";
	}
	return true;
}

inline bool FE::viewSwap() {
	if (this->m_scanViews == 0) {
		//fprintf(stdout, "we is in default scan view bruh\n");
		NoScanView();
	}
	else if (this->m_scanViews == 1) {
		//fprintf(stdout, "we is in compelte scan view bruh\n");
		CompleteScanView();
	}
	else if (this->m_scanViews == 2) {
		//fprintf(stdout, "we is in quick scan view bruh\n");
		QuickScansView();
	}
	else if (this->m_scanViews == 3) {
		//fprintf(stdout, "we is in advanced scan view bruh\n");
		//advancedScanPaths.clear();
		AdvancedScanView();
	}
	else {
		fprintf(stderr, "this shouldnt of happened, mr. freeman\n");
	}
	return true;
}

inline bool FE::DrawScansPage()
{
	/* BACK ARROW ICON */
	struct nk_rect bar = nk_rect(0, 0, WINDOW_WIDTH * .08, WINDOW_HEIGHT * .08);
	struct nk_rect backArrowAndText = nk_rect(bar.x, bar.y, bar.w, bar.h + 36); //36 for font size!
	if (nk_begin(this->ctx, "barrow", backArrowAndText,
		NK_WINDOW_NO_SCROLLBAR)) {

		/* hidden button behind icon to press */
		nk_layout_row_static(ctx, bar.y + bar.h + 36, bar.x + bar.w, 2);
		if (nk_button_label(ctx, "")) {
			//fprintf(stdout, "back arrow\n");
			this->view = 0;
			advancedScanPaths.clear();
			nk_clear(this->ctx);
		}
		this->drawImageSubRect(&this->backArrow, &bar);
		nk_draw_text(nk_window_get_canvas(this->ctx), SubRectTextBelow(&backArrowAndText, &bar), " BACK ", 6, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	// vertical line
	nk_stroke_line(this->canvas, bar.w * 2 + WINDOW_WIDTH * .075, WINDOW_HEIGHT - WINDOW_HEIGHT * .06,
		bar.w * 2 + WINDOW_WIDTH * .075, 0+WINDOW_HEIGHT * .06, 2, nk_rgb(255, 255, 255));

	int w_space = bar.w * 2 + WINDOW_WIDTH * .075;
	/* COMPLETE SCAN */
	struct nk_rect completeScanTextRect = nk_rect(.025*w_space, WINDOW_HEIGHT*.30, w_space*.95, 36);
	if (nk_begin(this->ctx, "CompleteScan", completeScanTextRect, NK_WINDOW_NO_SCROLLBAR)) {
		nk_layout_row_static(this->ctx, completeScanTextRect.y + completeScanTextRect.h, completeScanTextRect.x + completeScanTextRect.w, 2);
		if (nk_button_label(this->ctx, "")) {
			//fprintf(stdout, "Complete Scan\n");
			this->m_scanViews = 1;
			nk_clear(this->ctx);
		}
		nk_draw_text(nk_window_get_canvas(this->ctx), completeScanTextRect, " Complete Scan", 14, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	/* QUICK SCAN */
	struct nk_rect quickScanTextRect = nk_rect(.025 * w_space, WINDOW_HEIGHT * .45, w_space * .95, 36);
	if (nk_begin(this->ctx, "QuickScan", quickScanTextRect, NK_WINDOW_NO_SCROLLBAR)) {
		nk_layout_row_static(this->ctx, quickScanTextRect.y + quickScanTextRect.h, quickScanTextRect.x + quickScanTextRect.w, 2);
		if (nk_button_label(this->ctx, "")) {
			//fprintf(stdout, "Quick Scan\n");
			this->m_scanViews = 2;
			nk_clear(this->ctx);
		}
		nk_draw_text(nk_window_get_canvas(this->ctx), quickScanTextRect, " Quick Scan   ", 14, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	/* ADVANCED SCAN */
	struct nk_rect advScanTextRect = nk_rect(.025 * w_space, WINDOW_HEIGHT * .60, w_space * .95, 36);
	if (nk_begin(this->ctx, "AdvancedScan", advScanTextRect, NK_WINDOW_NO_SCROLLBAR)) {
		nk_layout_row_static(this->ctx, advScanTextRect.y + advScanTextRect.h, advScanTextRect.x + advScanTextRect.w, 2);
		if (nk_button_label(this->ctx, "")) {
			//fprintf(stdout, "Advanced Scan\n");
			this->m_scanViews = 3;
			nk_clear(this->ctx);
		}
		nk_draw_text(nk_window_get_canvas(this->ctx), advScanTextRect, " Advanced Scan", 14, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	this->viewSwap();

	return true;
}

inline FE::FE() {
	this->view = 0;
	this->m_scanViews = 0;
	/* INIT IMAGES */
	this->pp.scan = "../Assets/scan2.png";
	this->pp.rectLogo = "../Assets/rectLogo.png";
	this->pp.trapLogo = "../Assets/trapLogo.png";
	this->pp.squareLogo = "../Assets/squareLogo.png";
	this->pp.history = "../Assets/historylarger.png";
	this->pp.backArrow = "../Assets/back_arrow.png";
	this->pp.chooseScan = "../Assets/chooseScan.png";
	this->pp.triangleButton = "../Assets/triangle.png";

	//OPENFILENAME ofn = { sizeof ofn };
	//wchar_t file[1024];
	//file[0] = '\0';
	//ofn.lpstrFile = file;
	//ofn.nMaxFile = 1024;
	//ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_READONLY | OFN_NOVALIDATE;
	//GetOpenFileName(&ofn);
	//wchar_t* ptr = ofn.lpstrFile;
	//ptr[ofn.nFileOffset - 1] = 0;
	//std::wcout << L"Directory: " << ptr << std::endl;
	//ptr += ofn.nFileOffset;
	//while (*ptr)
	//{
	//	std::wcout << L"File: " << ptr << std::endl;
	//	ptr += (lstrlenW(ptr) + 1);
	//}
	//std::cout << "test" << std::flush; std::fflush;
}

inline void FE::UIPrintSet(std::set<std::string> s)
{
	for (auto f : s) {
		nk_label(this->ctx, f.c_str(), NK_TEXT_ALIGN_LEFT);
	}

}

inline bool FE::init(sf::Window *win) {
	glewExperimental = 1;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to setup GLEW\n");
		std::cout << glewGetErrorString(glewInit());
		exit(1);
	}
	/* GUI */
	//struct nk_context* ctx;
	this->ctx = nk_sfml_init(win);
	/* set font shit--kinda confused by nuklear api on this one chiefton */
	{//struct nk_font_atlas* atlas;
		nk_sfml_font_stash_begin(&this->atlas);
		//this->font = nk_font_atlas_add_from_file(this->atlas, this->font_path, 18, NULL);
		struct nk_font* droid = nk_font_atlas_add_from_file(atlas, "../Assets/font.ttf", 28, 0);
		nk_sfml_font_stash_end();
		nk_style_set_font(this->ctx, &droid->handle);
		//nk_init_default(this->ctx, &font->handle);
	}

	// load some image stuff
	//struct nk_buffer cmds;
	////nk_buffer_init_default(&cmds);
	glEnable(GL_TEXTURE_2D);

	// bg color
	this->bg.r = 45 / 255.0f; this->bg.g = 45 / 255.0f; this->bg.b = 45 / 255.0f; this->bg.a = 1.0f;
	// white txt color
	this->whiteFont.r = 0; this->whiteFont.g =0; this->whiteFont.b = 0; this->whiteFont.a = 1;
	
	// load pngs 
	this->scanImage = this->icon_load(pp.scan);
	this->squareImage = this->icon_load(pp.squareLogo);
	this->historyImage = this->icon_load(pp.history);
	this->backArrow = this->icon_load(pp.backArrow);
	this->chooseScan = this->icon_load(pp.chooseScan);
	this->triangleLogo = this->icon_load(pp.triangleButton);

	return true;
}

#endif