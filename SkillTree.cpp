#include "SkillTree.h"
#include <iostream>
#include <algorithm>

//--- [ 初期化と設定 ] --------------------------------------------------------

SkillTree::SkillTree(int initial_points) : current_skill_points_(initial_points) {
    // 仮のアイコン画像を読み込む（実際はユーザーが用意した画像に置き換えてください）
    // DxLibのInitGraph()と同じ処理
    int temp_icon = MakeScreen(NODE_SIZE, NODE_SIZE, TRUE);
    if (temp_icon != -1) {
        SetDrawScreen(temp_icon);
        ClearDrawScreen();
        DrawCircle(NODE_SIZE / 2, NODE_SIZE / 2, NODE_SIZE / 2 - 4, GetColor(100, 100, 255), TRUE);
        SetDrawScreen(DX_SCREEN_BACK);
        DeleteGraph(temp_icon); // 一旦削除して、setup_nodesで改めて読み込む
    }

    // デフォルトのアイコンを白丸として作成
    int icon_base = MakeScreen(NODE_SIZE, NODE_SIZE, TRUE);
    SetDrawScreen(icon_base);
    ClearDrawScreen();
    DrawCircle(NODE_SIZE / 2, NODE_SIZE / 2, NODE_SIZE / 2 - 2, GetColor(255, 255, 255), TRUE);
    SetDrawScreen(DX_SCREEN_BACK);

    // 実際に使用するアイコンハンドル
    int h_default = MakeScreen(NODE_SIZE, NODE_SIZE, TRUE);
    if (h_default != -1) {
        SetDrawScreen(h_default);
        ClearDrawScreen();
        DrawCircle(NODE_SIZE / 2, NODE_SIZE / 2, NODE_SIZE / 2 - 4, GetColor(100, 100, 255), TRUE);
        SetDrawScreen(DX_SCREEN_BACK);
    }

    // ノードのセットアップ
    setup_nodes();

    // スキルツリーの全体の高さを計算し、最大スクロール量を設定
    int max_y = 0;
    for (const auto& node : nodes_) {
        if (node.y > max_y) max_y = node.y;
    }
    // ノードのY座標の最大値 + ノードの高さ - 表示領域の高さ
    max_scroll_y_ = max(0, max_y + NODE_SIZE - VIEWPORT_H);
}

SkillTree::~SkillTree() {
    // 読み込んだアイコン画像の解放 (今回は一つのアイコンを使い回しているので不要な場合もある)
    // 実際に複数の画像を読み込んだ場合はここで解放処理を行う
}

// ダミーのスキルツリーデータを作成
void SkillTree::setup_nodes() {
    // アイコンハンドルをダミーで作成
    int h_icon = MakeScreen(NODE_SIZE, NODE_SIZE, TRUE);
    SetDrawScreen(h_icon);
    ClearDrawScreen();
    DrawCircle(NODE_SIZE / 2, NODE_SIZE / 2, NODE_SIZE / 2 - 4, GetColor(100, 200, 100), TRUE);
    SetDrawScreen(DX_SCREEN_BACK);

    // 

    [Image of Skill Tree Structure with Nodes and Lines]


        // ノードデータ
        nodes_ = {
        // ID 0: ルートスキル
        {0, -1, VIEWPORT_W / 2 - NODE_SIZE / 2, NODE_SPACE_Y * 1, "スタート", h_icon, 0, "ツリーの起点スキル。", true},

        // ティア 2
        {1, 0, VIEWPORT_W / 2 - NODE_SIZE / 2 - NODE_SPACE_X, NODE_SPACE_Y * 3, "攻撃力アップ I", h_icon, 10, "物理攻撃力が少し上昇します。", false},
        {2, 0, VIEWPORT_W / 2 - NODE_SIZE / 2 + NODE_SPACE_X, NODE_SPACE_Y * 3, "魔力アップ I", h_icon, 10, "魔法攻撃力が少し上昇します。", false},

        // ティア 3 (ID 1から派生)
        {3, 1, VIEWPORT_W / 2 - NODE_SIZE / 2 - NODE_SPACE_X * 1.5, NODE_SPACE_Y * 5, "攻撃力アップ II", h_icon, 30, "物理攻撃力が中程度上昇します。", false},
        {4, 1, VIEWPORT_W / 2 - NODE_SIZE / 2 - NODE_SPACE_X * 0.5, NODE_SPACE_Y * 5, "HPアップ I", h_icon, 20, "最大HPが少し上昇します。", false},

        // ティア 3 (ID 2から派生)
        {5, 2, VIEWPORT_W / 2 - NODE_SIZE / 2 + NODE_SPACE_X * 0.5, NODE_SPACE_Y * 5, "魔力アップ II", h_icon, 30, "魔法攻撃力が中程度上昇します。", false},
        {6, 2, VIEWPORT_W / 2 - NODE_SIZE / 2 + NODE_SPACE_X * 1.5, NODE_SPACE_Y * 5, "MPアップ I", h_icon, 20, "最大MPが少し上昇します。", false},

        // ティア 4 (深い位置のノード)
        {7, 3, VIEWPORT_W / 2 - NODE_SIZE / 2 - NODE_SPACE_X * 1.5, NODE_SPACE_Y * 7, "究極の一撃", h_icon, 100, "強力な一撃を繰り出す究極スキル。解放コストが非常に高い。", false},
        {8, 7, VIEWPORT_W / 2 - NODE_SIZE / 2 - NODE_SPACE_X * 1.5, NODE_SPACE_Y * 9, "マスタリー", h_icon, 200, "すべての能力を向上させる。ツリーの最終ノード。", false}
    };

    // スキルツリー全体の高さを再計算
    int max_y = 0;
    for (const auto& node : nodes_) {
        if (node.y > max_y) max_y = node.y;
    }
    // ノードのY座標の最大値 + ノードの高さ - 表示領域の高さ
    max_scroll_y_ = max(0, max_y + NODE_SIZE - VIEWPORT_H);
}

//--- [ 更新処理 ] --------------------------------------------------------------

void SkillTree::update() {
    handle_input();
    clamp_scroll();
}

//--- [ スキル解放ロジック ] ----------------------------------------------------

// 親スキルが解放済みかチェック
bool SkillTree::is_parent_unlocked(int node_id) const {
    const auto& node = nodes_[node_id];
    if (node.parent_id == -1) return true; // ルートノードは常に解放済みとみなす

    for (const auto& parent_node : nodes_) {
        if (parent_node.id == node.parent_id) {
            return parent_node.is_unlocked;
        }
    }
    return false; // 親ノードが見つからない場合は解放不可
}

// スキルの解放を試みる
bool SkillTree::unlock_skill(int node_id) {
    auto& node = nodes_[node_id];

    if (node.is_unlocked) return false; // 既に解放済み

    if (!is_parent_unlocked(node_id)) {
        // std::cout << node.name << "は親スキルが未解放です。\n";
        return false; // 親スキルが未解放
    }

    if (current_skill_points_ < node.cost) {
        // std::cout << node.name << "はポイントが不足しています。\n";
        return false; // ポイント不足
    }

    // 解放成功
    node.is_unlocked = true;
    current_skill_points_ -= node.cost;
    // std::cout << node.name << "を解放しました。残りポイント: " << current_skill_points_ << "\n";

    return true;
}


//--- [ 入力処理とスクロール ] -------------------------------------------------

void SkillTree::clamp_scroll() {
    // スクロール制限: 上限は0 (一番上)、下限は max_scroll_y_ (一番下)
    scroll_offset_y_ = min(0, scroll_offset_y_);
    scroll_offset_y_ = max(-max_scroll_y_, scroll_offset_y_);
}

void SkillTree::handle_input() {
    int mouse_x, mouse_y;
    GetMousePoint(&mouse_x, &mouse_y);
    int mouse_button = GetMouseInput();
    int wheel_rot = GetMouseWheelRotVol();

    // --- 1. スクロール処理 ---

    // マウスホイール
    if (wheel_rot != 0) {
        // ホイール一回転でNODE_SPACE_Yの約1/4程度スクロール
        scroll_offset_y_ += wheel_rot * (NODE_SPACE_Y / 4);
    }

    // マウスドラッグの開始
    if ((mouse_button & MOUSE_INPUT_LEFT) && !is_dragging_) {
        is_dragging_ = true;
        drag_start_y_ = mouse_y;
    }

    // マウスドラッグ中
    if (is_dragging_) {
        // 現在のマウス位置と前回の位置の差分でスクロール
        scroll_offset_y_ += (mouse_y - prev_mouse_y_);
    }

    // マウスボタンを離したらドラッグ終了
    if (!(mouse_button & MOUSE_INPUT_LEFT)) {
        is_dragging_ = false;
    }

    // --- 2. クリック処理 (ドラッグ中でないとき) ---
    if (!is_dragging_ && (mouse_button & MOUSE_INPUT_LEFT) && !(GetMouseInput() & MOUSE_INPUT_LEFT)) {
        // マウスのクリック座標とノードの画面座標を比較

        // 詳細パネルのクリック判定 (解放ボタンなど)
        if (selected_node_id_ != -1) {
            // 詳細パネルのボタン領域を判定 (今回は簡易的に画面右側に固定)
            int btn_x = VIEWPORT_W - 150;
            int btn_y = VIEWPORT_H - 100;
            int btn_w = 100;
            int btn_h = 40;

            if (mouse_x >= btn_x && mouse_x < btn_x + btn_w &&
                mouse_y >= btn_y && mouse_y < btn_y + btn_h)
            {
                // 解放ボタンがクリックされた
                unlock_skill(selected_node_id_);
                // クリックされたら、詳細パネルの選択を維持
                goto end_click_check;
            }
        }

        // ノードのクリック判定
        selected_node_id_ = -1; // ノードがクリックされなければ選択解除
        for (const auto& node : nodes_) {
            // ノードの画面座標を計算
            int screen_x = node.x;
            int screen_y = node.y + scroll_offset_y_;

            // マウス座標がノードの描画範囲内にあるか
            if (mouse_x >= screen_x && mouse_x < screen_x + NODE_SIZE &&
                mouse_y >= screen_y && mouse_y < screen_y + NODE_SIZE)
            {
                selected_node_id_ = node.id;
                // std::cout << "Node Clicked: " << node.name << "\n";
                break;
            }
        }
    }

end_click_check:
    prev_mouse_y_ = mouse_y;
}


//--- [ 描画処理 ] --------------------------------------------------------------

void SkillTree::draw() const {
    // 1. 接続ラインの描画
    draw_connections();

    // 2. ノードの描画
    for (const auto& node : nodes_) {
        draw_node(node);
    }

    // 3. 詳細パネルの描画
    draw_detail_panel();

    // 4. 所持ポイントの表示
    DrawFormatString(10, 10, GetColor(255, 255, 0), "スキルポイント: %d", current_skill_points_);
}

void SkillTree::draw_connections() const {
    // ラインの色（デフォルトは灰色）
    int default_color = GetColor(100, 100, 100);
    int unlocked_color = GetColor(0, 255, 0); // 解放済みのラインは緑

    for (const auto& node : nodes_) {
        if (node.parent_id != -1) {
            // 親ノードを検索
            const auto& parent_it = std::find_if(nodes_.begin(), nodes_.end(),
                [&](const SkillNode& n) { return n.id == node.parent_id; });

            if (parent_it != nodes_.end()) {
                const auto& parent_node = *parent_it;

                // 線の色を決定
                int line_color = default_color;
                if (node.is_unlocked) {
                    line_color = unlocked_color;
                }
                else if (is_parent_unlocked(node.id)) {
                    // 親が解放済みで、自分が未解放なら解放可能として黄色
                    line_color = GetColor(255, 255, 0);
                }

                // 画面座標に変換
                int p_x = parent_node.x + NODE_SIZE / 2;
                int p_y = parent_node.y + NODE_SIZE / 2 + scroll_offset_y_;
                int c_x = node.x + NODE_SIZE / 2;
                int c_y = node.y + NODE_SIZE / 2 + scroll_offset_y_;

                DrawLine(p_x, p_y, c_x, c_y, line_color);
            }
        }
    }
}

void SkillTree::draw_node(const SkillNode& node) const {
    // ノードの画面座標
    int screen_x = node.x;
    int screen_y = node.y + scroll_offset_y_;

    // 描画範囲外ならスキップ
    if (screen_y > VIEWPORT_H || screen_y + NODE_SIZE < 0) return;

    // 1. ノードアイコンの描画
    DrawGraph(screen_x, screen_y, node.icon_handle, TRUE);

    // 2. 枠線の色を決定
    int frame_color;
    int cost_color = GetColor(255, 255, 255); // コスト表示の色

    if (node.id == selected_node_id_) {
        frame_color = GetColor(255, 0, 255); // 選択中: マゼンタ
    }
    else if (node.is_unlocked) {
        frame_color = GetColor(0, 255, 0);   // 解放済み: 緑
        cost_color = GetColor(150, 150, 150); // 解放済みのコストは灰色
    }
    else if (is_parent_unlocked(node.id) && current_skill_points_ >= node.cost) {
        frame_color = GetColor(255, 255, 0); // 解放可能: 黄色
    }
    else if (is_parent_unlocked(node.id) && current_skill_points_ < node.cost) {
        frame_color = GetColor(255, 100, 0); // ポイント不足: オレンジ
    }
    else {
        frame_color = GetColor(150, 150, 150); // 未解放: 灰色
    }

    // 3. 枠線の描画
    DrawBox(screen_x - 2, screen_y - 2,
        screen_x + NODE_SIZE + 2, screen_y + NODE_SIZE + 2, frame_color, FALSE);

    // 4. コストの描画 (ノードの下)
    if (!node.is_unlocked && node.cost > 0) {
        DrawFormatString(screen_x, screen_y + NODE_SIZE + 5, cost_color, "C:%d", node.cost);
    }

    // 5. スキル名の描画 (ノードの下)
    // DrawFormatString(screen_x, screen_y + NODE_SIZE + 20, frame_color, "%s", node.name.c_str());
}

void SkillTree::draw_detail_panel() const {
    if (selected_node_id_ == -1) return;

    // 選択されているノードを検索
    const auto& it = std::find_if(nodes_.begin(), nodes_.end(),
        [&](const SkillNode& n) { return n.id == selected_node_id_; });
    if (it == nodes_.end()) return;

    const auto& node = *it;

    // パネルの描画領域 (画面右側に固定)
    int panel_w = 300;
    int panel_h = 200;
    int panel_x = VIEWPORT_W - panel_w - 10;
    int panel_y = VIEWPORT_H - panel_h - 10;

    // パネルの背景
    DrawBox(panel_x, panel_y, panel_x + panel_w, panel_y + panel_h, GetColor(0, 0, 50), TRUE);
    DrawBox(panel_x, panel_y, panel_x + panel_w, panel_y + panel_h, GetColor(50, 50, 150), FALSE);

    // スキル名
    DrawFormatString(panel_x + 10, panel_y + 10, GetColor(255, 255, 255), "スキル名: %s", node.name.c_str());

    // 解放状態
    int status_color = node.is_unlocked ? GetColor(0, 255, 0) : GetColor(255, 0, 0);
    DrawFormatString(panel_x + 10, panel_y + 40, GetColor(255, 255, 255), "状態: ");
    DrawFormatString(panel_x + 60, panel_y + 40, status_color, "%s", node.is_unlocked ? "解放済み" : "未解放");

    // 詳細説明
    DrawFormatString(panel_x + 10, panel_y + 70, GetColor(200, 200, 255), "説明:");
    // 説明は長い可能性があるので、折り返し処理が必要ですが、ここでは省略
    DrawFormatString(panel_x + 10, panel_y + 90, GetColor(200, 200, 255), "%s", node.description.c_str());


    // 解放ボタン
    if (!node.is_unlocked) {
        int btn_x = panel_x + panel_w / 2 - 50;
        int btn_y = panel_y + panel_h - 50;
        int btn_w = 100;
        int btn_h = 40;

        bool purchasable = is_parent_unlocked(node.id) && current_skill_points_ >= node.cost;
        int btn_color = purchasable ? GetColor(0, 150, 0) : GetColor(50, 50, 50); // 解放可能なら緑
        int text_color = purchasable ? GetColor(255, 255, 255) : GetColor(150, 150, 150);

        DrawBox(btn_x, btn_y, btn_x + btn_w, btn_y + btn_h, btn_color, TRUE);
        DrawBox(btn_x, btn_y, btn_x + btn_w, btn_y + btn_h, GetColor(255, 255, 255), FALSE);
        DrawFormatString(btn_x + 10, btn_y + 5, text_color, "解放 (C:%d)", node.cost);

        if (!is_parent_unlocked(node.id)) {
            DrawFormatString(btn_x - 50, btn_y - 20, GetColor(255, 100, 100), "親スキル未解放");
        }
        else if (current_skill_points_ < node.cost) {
            DrawFormatString(btn_x - 50, btn_y - 20, GetColor(255, 100, 100), "ポイント不足");
        }
    }
}