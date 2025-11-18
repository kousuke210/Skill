#include "SkillTree.h"

// メイン関数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    // DxLibの初期化
    ChangeWindowMode(TRUE);               // ウィンドウモード
    SetGraphMode(VIEWPORT_W, VIEWPORT_H, 32); // 画面サイズ設定
    if (DxLib_Init() == -1) return -1;
    SetDrawScreen(DX_SCREEN_BACK);        // 描画先を裏画面に設定

    // スキルツリーのインスタンス作成 (初期ポイントを100とする)
    SkillTree skill_tree(100);

    // メインループ
    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
        // 画面のクリア
        ClearDrawScreen();

        // --- 更新処理 ---
        skill_tree.update();

        // --- 描画処理 ---
        skill_tree.draw();

        // 裏画面の内容を表画面に反映
        ScreenFlip();
    }

    // DxLibの終了処理
    DxLib_End();
    return 0;
}