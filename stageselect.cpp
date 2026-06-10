#include "stageselect.h"
#include "sprite2d.h"
#include "texture.h"
#include "keyboard.h"
#include "fade.h"
#include "debug_ostream.h"
#include "define.h"
#include "font.h"
#include "mouse.h"
#include "sound.h"
#include "ClickFont.h"
// test
using namespace DirectX;

// 1. Ảnh nền phòng/bàn máy hát
static Sprite2D* g_pBackground = nullptr;

// 2. Đĩa than chính ở giữa và Kim đọc đĩa (Tonearm)
static Sprite2D* g_pMainVinyl = nullptr;
static Sprite2D* g_pToneArm = nullptr;

// 3. Hàng đĩa bên trái (Dùng mảng để quản lý)
const int MAX_STAGES = 5;
static Sprite2D* g_pStageDisks[MAX_STAGES] = { nullptr };
static ClickFont* g_pStageButtons[MAX_STAGES] = { nullptr };

// 4. Biến logic quản lý
static int g_SelectedStage = 0;      // Màn chơi đang được chọn
static float g_VinylRotation = 0.0f; // Góc xoay của đĩa chính

void StageSelect_Initialize(void)
{
	// Khởi tạo nền (Độ phân giải 2560 x 1440)
	g_pBackground = new Sprite2D(
		{ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
		{ 740.0f, 556.0f },
		0.0f,
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		BLENDSTATE_NONE,
		L"asset\\texture\\vinylbg.png"
	);

	// Khởi tạo hàng đĩa bên trái và nút bấm tương ứng
	for (int i = 0; i < MAX_STAGES; i++) {
		float posX = 200.0f;
		float posY = 200.0f + (i * 150.0f); // Tọa độ Y dịch chuyển dần xuống dưới

		g_pStageDisks[i] = new Sprite2D(
			{ posX, posY },
			{ 300.0f, 300.0f },
			0.0f,
			{ 1.0f, 1.0f, 1.0f, 1.0f },
			BLENDSTATE_NONE,
			L"asset\\texture\\tex.png"
		);

		// Đặt chữ/nút đè đúng lên vị trí của chiếc đĩa đó
		g_pStageButtons[i] = new ClickFont(
			{ posX, posY },
			30.0f, // Giảm size chữ một chút cho vừa khít cái đĩa
			0.0f,
			{ 1.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 0.8f, 0.2f, 1.0f },
			"Stage" // Tạm thời đặt tên chung, bạn có thể custom tùy ý
		);
	}

	// Khởi tạo đĩa chính ở GIỮA màn hình
	g_pMainVinyl = new Sprite2D(
		{ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
		{ 500.0f, 500.0f },
		0.0f,
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		BLENDSTATE_ALFA, // ĐỔI TỪ BLENDSTATE_NONE THÀNH ALPHA
		L"asset\\texture\\vinmain.png"
	);

	// Tương tự cho g_pToneArm và g_pStageDisks[i]
	g_pToneArm = new Sprite2D(
		{ SCREEN_WIDTH / 2.0f + 350.0f, SCREEN_HEIGHT / 2.0f - 300.0f },
		{ 400.0f, 400.0f },
		0.0f,
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		BLENDSTATE_ALFA, // ĐỔI THÀNH ALPHA
		L"asset\\texture\\tonearm.png"
	);

	UnLockMouse(); // Mở khóa chuột
}

void StageSelect_Update(void)
{
	// 1. Cập nhật trạng thái cho toàn bộ các nút bên trái
	for (int i = 0; i < MAX_STAGES; i++)
	{
		g_pStageButtons[i]->Update();

		// Nếu người chơi click chọn đĩa thứ i
		if (g_pStageButtons[i]->IsClick())
		{
			g_SelectedStage = i; // Ghi nhận màn chơi được chọn

			// Bạn có thể phát âm thanh chuyển đĩa tại đây bằng thư viện sound.h
			// PlaySound(...);

			// Nếu muốn bấm phát ăn ngay vào game:
			SetSceneFade(SCENE_GAME);
		}
	}

	// 2. Làm cho đĩa chính ở giữa xoay tròn mượt mà
	g_VinylRotation += 0.5f; // Tốc độ xoay (tăng/giảm tùy ý)
	if (g_VinylRotation >= 360.0f) {
		g_VinylRotation -= 360.0f;
	}

	// Cập nhật góc quay vào Sprite đĩa chính
	if (g_pMainVinyl != nullptr) {
		g_pMainVinyl->SetRotation(g_VinylRotation);
	}
}

void StageSelect_Draw(void)
{
	// ④ Vẽ theo thứ tự từ dưới lên trên
	g_pBackground->Draw(); // Vẽ nền trước

	// Vẽ toàn bộ danh sách đĩa và chữ bên trái
	for (int i = 0; i < MAX_STAGES; i++) {
		g_pStageDisks[i]->Draw();
		g_pStageButtons[i]->Draw();
	}

	g_pMainVinyl->Draw(); // Vẽ đĩa xoay chính ở giữa
	g_pToneArm->Draw();   // Vẽ kim đĩa đè lên trên cùng đĩa chính
}

void StageSelect_Finalize(void)
{
	// ⑤ Giải phóng toàn bộ bộ nhớ sạch sẽ để tránh rò rỉ (Memory Leak)
	SAFE_DELETE(g_pBackground);
	SAFE_DELETE(g_pMainVinyl);
	SAFE_DELETE(g_pToneArm);

	for (int i = 0; i < MAX_STAGES; i++) {
		SAFE_DELETE(g_pStageDisks[i]);
		SAFE_DELETE(g_pStageButtons[i]);
	}
}