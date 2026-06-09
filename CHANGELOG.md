# Changelog

## v0.3.1

- `Font` フォルダ内に存在するフォントも認識するように修正
- フォントのファミリー検索時にボールド，イタリックの判別を正しく行うように修正
- 点滅でフォントを選択できるように変更

## v0.3.0

> [!CAUTION]
> エコーのバグ修正に伴い，既存プロジェクトを再読み込みすると `Motion@FlowType_K` と `Echo@FlowType_K` でエコーが1つ余分に追加される問題が発生する．
> そのため，以下の操作を行うことを推奨する．
> - `Motion@FlowType_K` で `Echo::Count` を1つ小さく設定
> - `Echo@FlowType_K` で `Count` を1つ小さく設定

> [!CAUTION]
> 点滅の仕様変更に伴い，既存プロジェクトを再読み込みすると `Motion@FlowType_K` と `Blink@FlowType_K` で 点滅ステップ数がリセットされる．
> そのため，以下の操作を行うことを推奨する．
> - `Motion@FlowType_K` でステップ数を再設定
> - `Blink@FlowType_K` でステップ数を再設定

- 一部プロパティメニュー名の変更
- `Repeat@FlowType_K` を追加
- プロパティメニューに`プロパティ名をコピー`と`参照式をコピー`を追加
- `Motion@FlowType_K` に `Offset` を追加
- `Motion@FlowType_K` と `Blink@FlowType_K` に文字置換機能を追加
- `Kerning@FlowType_K` に `Limit Effects Below` を追加
- `Blink@FlowType_K` に `Based On` を追加
- `Island@FlowType_K` で `Time Offset::Order` によって個別オブジェクトのインデックス付与順が変わるように変更
- `Motion@FlowType_K` と `Blink@FlowType_K` で 点滅ステップ数を個別に設定できるように変更 (破壊的)
- キャッシュ処理の修正
- テキストオブジェクトの情報を必要とするエフェクトをフィルタ効果やグループ制御からかけれるように修正
- 出力中はオーダー順序を表示しないように修正
- `Motion@FlowType_K` で `Timing` が `Manual` の場合，退場モーションがおかしかった問題の修正
- `Motion@FlowType_K` で リリックモーションの移動量と伸び量をリンクするように修正
- `Motion@FlowType_K` で点滅させるとき使用する時間値が全体時間であった問題の修正
- `Motion@FlowType_K` の点滅と `Blink@FlowType_K` の時間変化をFPS依存に修正
- `Motion@FlowType_K` で `Effect::Parameters` のパース結果が消されていた場合エラーで止まる問題の修正
- `Motion@FlowType_K` ， `Echo@FlowType_K` ， `Island@FlowType_K` で出力時間が範囲外になる問題を修正
- `Motion@FlowType_K` および `Echo@FlowType_K` でエコーカウントが1つ大きかった問題の修正 (破壊的)
- `Kerning@FlowType_K` でアラインメントを考慮するように修正
- テキスト分解で中心座標が正しくセットされない問題の修正
- 中間点無視のトラックバーに対して `値を揃える` を使用するとクラッシュする問題の修正
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
