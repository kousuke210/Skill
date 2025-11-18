#pragma once
#include <string>
#include <vector>
#include <DxLib.h>

// 定数定義
const int NODE_SIZE = 64;       // ノード（アイコン）の大きさ
const int NODE_SPACE_Y = 100;   // 縦方向のノード間隔
const int NODE_SPACE_X = 100;   // 横方向のノード間隔
const int VIEWPORT_W = 800;     // 表示領域の幅
const int VIEWPORT_H = 600;     // 表示領域の高さ

// スキルノードの情報を保持する構造体
struct SkillNode {
    int id;
    int parent_id;

    // 論理座標（ツリーマップ上の絶対位置）
    int x, y;

    std::string name;
    int icon_handle;      // アイコン画像のハンドル
    int cost;             // 解放に必要なコスト
    std::string description;

    bool is_unlocked;
};

// スキルツリー全体を管理するクラス
class SkillTree {
private:
    std::vector<SkillNode> nodes_;
    int scroll_offset_y_ = 0;         // 現在のスクロール位置（Y座標）
    int max_scroll_y_ = 0;            // 最大スクロール量

    int current_skill_points_;
    int selected_node_id_ = -1;       // 詳細表示のために選択されているノードID

    // マウスドラッグ用の変数
    bool is_dragging_ = false;
    int drag_start_y_ = 0;
    int prev_mouse_y_ = 0;

private:
    // プライベートヘルパー関数
    void handle_input();
    void clamp_scroll();
    bool is_parent_unlocked(int node_id) const;

    void draw_node(const SkillNode& node) const;
    void draw_connections() const;
    void draw_detail_panel() const;

    // スキル解放ロジック
    bool unlock_skill(int node_id);

public:
    SkillTree(int initial_points);
    ~SkillTree();
    void setup_nodes(); // 初期ノードデータの作成と画像読み込み
    void update();      // 更新処理
    void draw() const;  // 描画処理
};