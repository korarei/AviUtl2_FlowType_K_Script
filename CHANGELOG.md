# Changelog

## v0.3.0

> [!CAUTION]
> エコーのバグ修正に伴い，既存プロジェクトを再読み込みすると `Motion@FlowType_K` と `Echo@FlowType_K` でエコーが1つ余分に追加される問題が発生する．
> そのため，以下の操作を行うことを推奨する．
> - `Motion@FlowType_K` で `Echo::Count` を1つ小さく設定
> - `Echo@FlowType_K` で `Count` を1つ小さく設定

- `Repeat@FlowType_K` を追加
- プロパティメニューに`プロパティ名をコピー`を追加
- `Kerning@FlowType_K` に `Effect::Parameters` を追加
- `Island@FlowType_K` で `Time Offset::Order` によって個別オブジェクトのインデックス付与順が変わるように変更
- キャッシュ処理の修正
- 出力中はオーダー順序を表示しないように修正
- `Motion@FlowType_K` で点滅させるとき使用する時間値が全体時間であった問題の修正
- `Motion@FlowType_K` の点滅と `Blink@FlowType_K` の時間変化をFPS依存に修正
- `Motion@FlowType_K` ， `Echo@FlowType_K` ， `Island@FlowType_K` で出力時間が負になる問題を修正
- `Motion@FlowType_K` および `Echo@FlowType_K` でエコーカウントが1つ大きかった問題の修正 (破壊的)
- `Trim@FlowType_K` のフィルタプラグイン化

## v0.2.0

- 一部プロパティメニュー名の変更
- プロパティメニュー`値を揃える`に`全ての区間`を追加
- プロパティメニュー`値を反転`に`現在の区間`，`以前の区間`，`以降の区間`を追加
- プロパティメニューに`エフェクトをコピー`を追加
- `Motion@FlowType_K` の `Based On` が `Whole` 以外において，個別オブジェクト数が文字数の定数倍になる場合の挙動を追加
- `Motion@FlowType_K` の `Based On` に `Objects` を追加 (個別オブジェクト毎を `Characters` からこれに変更)
- `Kerning@FlowType_K` に影響度合いを指定する `Influence` を追加
- `Kerning@FlowType_K` で個別オブジェクト数が文字数の定数倍になる場合の挙動を追加
- `Motion@FlowType_K` の `Lyric` で退場時の伸縮が想定と逆になっていた問題を修正
- トランスフォームのターゲットがローカル空間でもワールド空間でもない場合は変換処理が行われないように修正
- `Align@FlowType_K` のフィルタプラグイン化
- `Deform@FlowType_K` のフィルタプラグイン化
- `Transform@FlowType_K` のフィルタプラグイン化
- プリセットの追加

## v0.1.0

- Release
