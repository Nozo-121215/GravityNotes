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
#include "MultiLineFontRenderer.h"
#include "scoresummaryloader.h"
#include <cstdio>

using namespace DirectX;

// ①インスタンス、ポインタ用意
static Sprite2D* g_pStageSelectSprite = nullptr;
static ClickFont* g_pChangeSceneText = nullptr;
static MultiLineFontRenderer* g_pScoreInfoText = nullptr;
static int g_SelectedScoreIndex = 0;

// 現在選択中の楽曲情報を1曲分だけ整形して表示し、選択中json名をManagerへ同期する
static void RefreshSelectedScoreText()
{
	const auto& summaries = ScoreSummaryManager::GetInstance().GetSummaries();
	if (g_pScoreInfoText == nullptr)
	{
		return;
	}

	if (summaries.empty())
	{
		g_pScoreInfoText->SetText("No score json found");
		return;
	}

	if (g_SelectedScoreIndex < 0)
	{
		g_SelectedScoreIndex = 0;
	}
	if (g_SelectedScoreIndex >= static_cast<int>(summaries.size()))
	{
		g_SelectedScoreIndex = static_cast<int>(summaries.size()) - 1;
	}

	const ScoreSummary& summary = summaries[static_cast<size_t>(g_SelectedScoreIndex)];
	ScoreSummaryManager::GetInstance().SetSelectedJsonName(summary.jsonname);

	char buf[1024] = {};
	std::snprintf(
		buf,
		sizeof(buf),
		"[%d/%d]\nMusic: %s\nComposer: %s\nChart: %s\nDifficulty: %.1f\nBPM: %.1f\nJSON: %s",
		g_SelectedScoreIndex + 1,
		static_cast<int>(summaries.size()),
		summary.musicname.c_str(),
		summary.musicauthor.c_str(),
		summary.scoreauthor.c_str(),
		summary.difficulty,
		summary.bpm,
		summary.jsonname.c_str()
	);
	g_pScoreInfoText->SetText(buf);
}

// 左右入力で選択中インデックスを循環更新し、表示内容を更新する
static void ChangeSelectedScore(int delta)
{
	const auto& summaries = ScoreSummaryManager::GetInstance().GetSummaries();
	if (summaries.empty())
	{
		return;
	}

	const int count = static_cast<int>(summaries.size());
	g_SelectedScoreIndex = (g_SelectedScoreIndex + delta) % count;
	if (g_SelectedScoreIndex < 0)
	{
		g_SelectedScoreIndex += count;
	}

	RefreshSelectedScoreText();
}

// StageSelectシーンの生成と、譜面概要一覧の再読み込みを行う
void StageSelect_Initialize(void)
{
	// ②各種初期化
	g_pStageSelectSprite = new Sprite2D(
		{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3 },					//位置
		{ 300.0f, 300.0f },											//サイズ
		0.0f,														//回転（度）
		{ 1.0f, 1.0f, 1.0f, 1.0f },									//RGBA
		BLENDSTATE_NONE,											//BlendState
		L"asset\\texture\\tex.png"									//テクスチャパス
	);

	g_pChangeSceneText = new ClickFont(
		{ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 4.0f * 3 },			//位置
		50.0f,														//文字サイズ
		0.0f,														//回転（度）
		{ 1.0f, 1.0f, 1.0f, 1.0f },									//通常色
		{ 1.0f, 0.8f, 0.2f, 1.0f },									//ホバー色
		"[stageselect.cpp] ゲームへ"								//テキスト
	);

	g_pScoreInfoText = new MultiLineFontRenderer(
		{ SCREEN_WIDTH / 4.0f, SCREEN_HEIGHT / 2 },
		28.0f,
		0.0f,
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		"loading...",
		1.35f
	);

	const bool loaded = ScoreSummaryManager::GetInstance().ReloadSummaries();
	hal::dout << "[StageSelect] score summary reload: "
		<< (loaded ? "success" : "failed")
		<< " count=" << ScoreSummaryManager::GetInstance().GetSummaryCount()
		<< std::endl;

	g_SelectedScoreIndex = 0;
	RefreshSelectedScoreText();

	UnLockMouse();//マウスアンロック
}

// 入力処理（左右で選曲）と決定クリック時のシーン遷移を行う
void StageSelect_Update(void)
{
	//③処理
	g_pChangeSceneText->Update();

	if (Keyboard_IsKeyDownTrigger(KK_LEFT))
	{
		ChangeSelectedScore(-1);
	}

	if (Keyboard_IsKeyDownTrigger(KK_RIGHT))
	{
		ChangeSelectedScore(1);
	}

	//ClickFontがクリックされた
	if (g_pChangeSceneText->IsClick())
	{
		SetSceneFade(SCENE_GAME);
	}
}

// StageSelectシーンの描画を行う
void StageSelect_Draw(void)
{
	//④描画
	g_pStageSelectSprite->Draw();
	g_pScoreInfoText->Draw();
	g_pChangeSceneText->Draw();
}

// StageSelectシーンで確保したリソースを解放する
void StageSelect_Finalize(void)
{
	//⑤解放
	SAFE_DELETE(g_pStageSelectSprite);
	SAFE_DELETE(g_pScoreInfoText);
	SAFE_DELETE(g_pChangeSceneText);
}
