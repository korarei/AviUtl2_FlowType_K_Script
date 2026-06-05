# FlowType_K

![GitHub License](https://img.shields.io/github/license/korarei/AviUtl2_FlowType_K_Script)
![GitHub Last commit](https://img.shields.io/github/last-commit/korarei/AviUtl2_FlowType_K_Script)
![GitHub Downloads](https://img.shields.io/github/downloads/korarei/AviUtl2_FlowType_K_Script/total)
[![GitHub Release][releases-badge]][releases-url]
[![AviUtl2 Catalog][catalog-badge]][catalog-url]

AviUtl ExEdit2向けテキストアニメーション作成支援ツール．

以下の機能が追加される．

アニメーション効果

- Text\\Motion@FlowType_K: 全自動リリックモーション
- Text\\Island@FlowType_K: パーツ分解
- Text\\Kerning@FlowType_K: 全自動カーニング
- Text\\Trim@FlowType_K: 余白除去
- Text\\Deform@FlowType_K: 変形
- Text\\Align@FlowType_K: 整列
- Text\\Transform@FlowType_K: 座標変換
- Text\\Blink@FlowType_K: 点滅
- Text\\Echo@FlowType_K: 残像

オブジェクトメニュー (オブジェクトを右クリック)

- FlowType_K\\テキストを文字ごとに分解: テキスト分解

プロパティメニュー (オブジェクト設定項目を右クリック)

- FlowType_K\\値を揃える\\全ての区間: 現在値を全ての区間にコピー
- FlowType_K\\値を揃える\\以前の区間: 現在値を以前の区間にコピー
- FlowType_K\\値を揃える\\以降の区間: 現在値を以降の区間にコピー
- FlowType_K\\値を反転\\現在の区間: 現在値を反転 (符号，ON/Off)
- FlowType_K\\値を反転\\全ての区間: 全ての区間の値を反転 (符号，ON/Off)
- FlowType_K\\値を反転\\以前の区間: 以前の区間の値を反転 (符号，ON/Off)
- FlowType_K\\値を反転\\以降の区間: 以降の区間の値を反転 (符号，ON/Off)

- FlowType_K\\プロパティ名をコピー\\{プロパティ名}: プロパティ名をクリップボードへコピー
- FlowType_K\\プロパティ名をコピー\\{エフェクト名}.{プロパティ名}: プロパティ名をクリップボードへコピー
- FlowType_K\\プロパティ名をコピー\\{レイヤー名}.{エフェクト名}.{プロパティ名}: プロパティ名をクリップボードへコピー

- FlowType_K\\エフェクトをコピー\\エイリアス形式 (INI 形式): エフェクトをエイリアス形式でクリップボードへコピー
- FlowType_K\\エフェクトをコピー\\FlowType_K 形式 (TOML 形式): エフェクトを FlowType_K 形式でクリップボードへコピー

## 動作確認

- [AviUtl ExEdit2 beta47](https://spring-fragrance.mints.ne.jp/aviutl/)

> [!CAUTION]
> beta47以降必須．

## 導入・更新・削除

### パッケージファイルからインストール

#### 導入・更新

[こちら][releases-url]からダウンロードした `*.au2pkg.zip` をAviUtl2にD&D．

#### 削除

パッケージ情報からアンインストールする．

### [AviUtl2 カタログ](https://github.com/Neosku/aviutl2-catalog)からインストール

[こちら][catalog-url]から導入，更新，削除を行う．

## 使い方

### Motion@FlowType_K

初期ラベル: `Text`

テキストアニメーションを付与するスクリプト．

#### パラメータ

- Timing: 表示タイミングの設定方法を指定
  - Automatic: 自動
  - Manual: 手動 (中間点で表示タイミングを指定する)
- Duration: 文字列全体のアニメーションが完了するまでの時間を指定．(正: 登場, 負: 退場)
- Offset: 登場タイミングのズレを指定．

- <details>
  <summary>Motion</summary>

  動作の単位や順序、重なり具合、滑らかさを指定する．

  - Motion::Based On: モーションを適用する単位を指定
    - Whole: 全体
    - Objects: 個別オブジェクト単位
    - Characters: 文字単位
    - Characters Excluding Spaces: 空白を除いた文字単位
    - Words: 単語単位
    - Lines: 行単位
  - Motion::Order: モーションの適用順序を指定
    - Forward: 順方向
    - Reverse: 逆方向
    - Random: ランダム
  - Motion::Overlap: モーションの重なり具合を指定
  - Motion::Softness: モーションの滑らかさを指定
  - Motion::Curve: モーションの補間曲線を指定
  - Motion::Cutoff: カットオフ閾値を指定 (指定値以下の進行度でアルファ値を0にする)
  - Motion::Mask: 各要素の変形に伴うマスク処理を適用するか指定

  </details>

- <details>
  <summary>Lyric</summary>

  リリックビデオで使われるようなアニメーションを設定する．

  - Lyric::Distance: 要素を配置する距離を指定
  - Lyric::Stretch: 要素の拡縮具合を指定
  - Lyric::Jitter: 位置や拡縮のばらつき (ジッター) を指定
  - Lyric::Layout::Shape: 配置形状を指定
    - Circle: 円形
    - Arc: 弧状
  - Lyric::Layout::Angle: 配置の開始角度を指定
  - Lyric::Layout::Sweep: 配置の範囲・スイープ角を指定
  - Lyric::Layout::Divisions: 配置の分割数を指定
  - Lyric::Layout::Selection: 要素の配置方向を指定
    - Random: ランダム
    - Clockwise: 時計回り
    - Counter-Clockwise: 反時計回り
  - Lyric::Dynamics::Peak: 要素のピーク時のスケールを指定
  - Lyric::Dynamics::Overshoot: スケールのオーバーシュート表現を有効にするか指定

  </details>

- <details>
  <summary>Transform</summary>

  モーションの起点・終点における変形 (移動・回転・拡縮等) を設定する．

  - Transform::Pivot::X / Y / Z: 変形の基準点 (ピボット) を指定
  - Transform::Position::X / Y / Z: 位置の変位を指定
  - Transform::Rotation::W / X / Y / Z: 回転の変位を指定
  - Transform::Rotation::Mode: 回転モードを指定
    - Quaternion: クォータニオン
    - Axis Angle: 軸角度
    - Euler系各種 (外因性)
  - Transform::Scale::X / Y / Z: 拡大縮小の変位を指定
  - Transform::Compositing::Opacity: 不透明度の変位を指定
  - Transform::Target::Local Space: ローカル座標系での変形を有効にするか指定
  - Transform::Target::World Space: ワールド座標系での変形を有効にするか指定

  </details>

- <details>
  <summary>Blink</summary>

  要素の点滅 (ブリンク) 効果やエッジ抽出を設定する．

  - Blink::Duration: 点滅アニメーションの時間を指定
  - Blink::Steps: 点滅の段階数 (ステップ数) を指定
  - Blink::Opacity::Minimum / Maximum: 点滅時の不透明度の最小値と最大値を指定
  - Blink::Scale::Minimum / Maximum: 点滅時のスケールの最小値と最大値を指定
  - Blink::Edge Detection::Intensity / Threshold: エッジ抽出の強度としきい値を指定

  </details>

- <details>
  <summary>Tint</summary>

  要素に対する色付け (画像やレイヤーからの参照) を設定する．

  - Tint::Source: 色付けのソースを指定
    - Image: 画像ファイル
    - Layer: 特定のレイヤー
  - Tint::Image: (SourceがImageの場合) 適用する画像ファイルを指定
  - Tint::Layer: (SourceがLayerの場合) 適用するレイヤー番号を指定

  </details>

- <details>
  <summary>Effect</summary>

  各要素に対してToml形式で指定したエフェクトをかける．

  - Effect::Parameters: エフェクトのパラメータをToml形式で記述する

  ```toml
  ["モザイク"]
  "サイズ" = 10

  # lua:でluaスクリプトを記述可能 (一部機能は制限されている)
  ["ぼかし"]
  "lua:範囲" = """
  -- weight: [-1.0, 1.0] の移動幅
  return weight * 10
  """
  ```

  </details>

- <details>
  <summary>Echo</summary>

  残像 (エコー) 効果を設定する．

  - Echo::Interval: 残像が発生する時間間隔を指定
  - Echo::Count: 残像の数を指定
  - Echo::Decay: 残像の減衰率 (不透明度) を指定
  - Echo::Composite: 残像の合成順序を指定
    - Above: 元のオブジェクトの上に合成
    - Below: 元のオブジェクトの下に合成

  </details>

- <details>
  <summary>Additional Options</summary>

  その他の設定．

  - Unit: 時間や間隔の単位を指定
    - Frames: フレーム単位
    - Seconds: 秒単位
  - Layer Reference: レイヤー指定 (Tint等) の参照方法を指定
    - Absolute: 絶対指定 (指定したレイヤー番号)
    - Relative: 相対指定 (自身のレイヤーからの相対番号)
  - Seed: ランダムシード値を指定 (負の数でレイヤー毎に別シードが自動適用)
  - Highlight Overlap: モーションの重なり部分をハイライト表示するか指定 (デバッグ・調整用)

  </details>

### Island@FlowType_K

初期ラベル: `Text`

テキストや画像を要素 (島) ごとに分離し、個別に操作可能にするスクリプト． (単一オブジェクトでのみ動作)

#### パラメータ

- Threshold: 分離の閾値 (アルファ値) を指定
- Index: 操作する要素のインデックスを指定 (-1で全体)

- <details>
  <summary>Anchor</summary>

  - Anchor::Target: 適用対象を指定
    - Position: 位置
    - Pivot Point: 基準点
  - Anchor::Overwrite: 上書きするか指定

  </details>

- <details>
  <summary>Sort</summary>

  - Sort::Order::Primary Axis: ソートの優先軸を指定
    - X: X軸
    - Y: Y軸
  - Sort::Order::X: X方向のソート順
    - Left to Right: 左から右
    - Right to Left: 右から左
  - Sort::Order::Y: Y方向のソート順
    - Top to Bottom: 上から下
    - Bottom to Top: 下から上
  - Sort::Order::Custom Order: カスタム順序配列を指定
  - Sort::Blocks::X / Y: ブロック分割数を指定 (3行のテキストを使用する際はYを3に設定するといい感じになる)

  </details>

- <details>
  <summary>Tint</summary>

  - Tint::Source: 色付けのソースを指定
    - Image: 画像ファイル
    - Layer: 特定のレイヤー
  - Tint::Image: 画像ファイルを指定
  - Tint::Layer: レイヤー番号を指定
  - Tint::Order: 色付けの適用順序
    - Forward: 順方向
    - Reverse: 逆方向
    - Random: ランダム

  </details>

- <details>
  <summary>Time Offset</summary>

  - Time Offset::Interval: 時間オフセットの間隔を指定
  - Time Offset::Order: 時間オフセットの適用順序
    - Forward: 順方向
    - Reverse: 逆方向
    - Random: ランダム

  </details>

- <details>
  <summary>Additional Options</summary>

  - Connectivity: 連結判定
    - 4-Connected: 4近傍
    - 8-Connected: 8近傍
  - Unit: 時間や間隔の単位を指定
  - Layer Reference: レイヤー指定の参照方法を指定
  - Seed: ランダムシード値を指定
  - Highlight Order: ソート順をハイライト表示するか指定

  </details>

### Kerning@FlowType_K

初期ラベル: `Text`

正規表現等を用いて特定の文字ペアや文字列のカーニング・変形・色付けを行うスクリプト．

正規表現の構文は[こちら](https://github.com/google/re2/wiki/Syntax)

#### パラメータ

- Kerning Mode: カーニングモードを指定
  - None: なし
  - Metrics: メトリクス

- <details>
  <summary>Filter</summary>

  - Filter::Regex Pattern: 対象とする正規表現パターンを指定
  - Filter::Capture Group: 正規表現のキャプチャグループ番号を指定

  </details>

- <details>
  <summary>Transform</summary>

  - Transform::Pivot::X / Y / Z: 変形の基準点を指定
  - Transform::Position::X / Y / Z: 位置の変位を指定
  - Transform::Rotation::W / X / Y / Z: 回転の変位を指定
  - Transform::Rotation::Mode: 回転モードを指定
  - Transform::Scale::X / Y / Z: 拡大縮小の変位を指定
  - Transform::Compositing::Blend Mode: 合成モードを指定 (AviUtl2に準拠)
  - Transform::Compositing::Opacity: 不透明度を指定
  - Transform::Target::Local Space: ローカル座標系での変形を有効にするか指定
  - Transform::Target::World Space: ワールド座標系での変形を有効にするか指定

  </details>

- <details>
  <summary>Tint</summary>

  - Tint::Color: 色指定
  - Tint::Opacity: 不透明度を指定

  </details>

- <details>
  <summary>Effect</summary>

  各要素に対してToml形式で指定したエフェクトをかける．

  - Effect::Parameters: エフェクトのパラメータをToml形式で記述する

- <details>
  <summary>Additional Options</summary>

  - Influence: 影響度合いを指定 (0〜100)

  </details>

### Repeat@FlowType_K

初期ラベル: `Text`

各要素を繰り返し配置するスクリプト．

#### パラメータ

- <details>
  <summary>Layout</summary>

  - Layout::Count::X: X方向の繰り返し回数を指定
  - Layout::Count::Y: Y方向の繰り返し回数を指定
  - Layout::Padding::X: X方向の間隔を指定
  - Layout::Padding::Y: Y方向の間隔を指定

  </details>

- <details>
  <summary>Position Offset</summary>

  - Position Offset::Angle: ズレの角度 (ズレ量) を指定
  - Position Offset::Axis: ズレの軸方向を指定

  </details>

- <details>
  <summary>Time Offset</summary>

  - Time Offset::Interval: 時間オフセットの間隔を指定
  - Time Offset::Orientation: 時間オフセットの方向を指定
    - Column: 列順 (横方向)
    - Row: 行順 (縦方向)
  - Time Offset::Order: 時間オフセットの適用順序
    - Forward: 順方向
    - Reverse: 逆方向
    - Random: ランダム

  </details>

- <details>
  <summary>Additional Options</summary>

  - Unit: 時間や間隔の単位を指定
  - Seed: ランダムシード値を指定
  - Highlight Order: ソート順をハイライト表示するか指定

  </details>

### Trim@FlowType_K

初期ラベル: `Text`

各要素の余白をトリミングして画像サイズを調整するスクリプト．

#### パラメータ

- Threshold: トリミングの閾値 (アルファ値) を指定

- <details>
  <summary>Anchor</summary>

  - Anchor::Target: 余白除去後の基準点を自動修正する対象
    - None: なし
    - Position: 位置
    - Pivot Point: 基準点
  - Anchor::Overwrite: 上書きするか指定

  </details>

- <details>
  <summary>Padding</summary>

  - Padding::Left: 左側のパディング
  - Padding::Right: 右側のパディング
  - Padding::Top: 上側のパディング
  - Padding::Bottom: 下側のパディング

  </details>

### Deform@FlowType_K

初期ラベル: `Text`

変形・歪ませるスクリプト．

#### パラメータ

- <details>
  <summary>Pivot Point</summary>

  - Pivot::X / Y: 変形の基準点を指定

  </details>

- <details>
  <summary>Position</summary>

  - Position::X / Y: 位置の変位を指定

  </details>

- <details>
  <summary>Scale</summary>

  - Scale::X / Y: 拡大縮小を指定

  </details>

- <details>
  <summary>Skew</summary>

  - Skew::Angle: 歪み (スキュー) の角度を指定
  - Skew::Axis: 歪みの軸方向を指定

  </details>

- Rotation: 回転角を指定
- Opacity: 不透明度を指定
- Sampling: ピクセル補間方法を指定
  - Nearest Neighbor: ニアレストネイバー (補間なし)
  - Bilinear: バイリニア

### Align@FlowType_K

初期ラベル: `Text`

テキスト要素の配置 (アンカー位置) を揃えるスクリプト．

#### パラメータ

- Horizontal: 水平方向の配置 (ズレ) を指定
- Vertical: 垂直方向の配置 (ズレ) を指定
- Target: 変更を適用する対象を指定
  - Pivot Point: 基準点
  - Position: 位置
  - Both: 両方
- Overwrite: 現在の座標に加算するのではなく、値を上書きするか指定

### Transform@FlowType_K

初期ラベル: `Text`

座標変換処理 (移動・回転・拡縮等) を行うスクリプト．

#### パラメータ

- <details>
  <summary>Pivot Point</summary>

  - Pivot::X / Y / Z: 変形の基準点 (ピボット) を指定

  </details>

- <details>
  <summary>Position</summary>

  - Position::X / Y / Z: 位置の変位を指定

  </details>

- <details>
  <summary>Rotation</summary>

  - Rotation::W / X / Y / Z: 回転の変位を指定
  - Rotation::Mode: 回転モードを指定 (Quaternion, Axis Angle, Euler系各種)

  </details>

- <details>
  <summary>Scale</summary>

  - Scale::X / Y / Z: 拡大縮小の変位を指定

  </details>

- <details>
  <summary>Target</summary>

  - Target::Local Space: ローカル座標系での変形を有効にするか指定
  - Target::World Space: ワールド座標系での変形を有効にするか指定

  </details>

- <details>
  <summary>Additional Options</summary>

  - Influence: 影響度合いを指定 (0〜100)

  </details>

### Blink@FlowType_K

初期ラベル: `Text`

テキスト要素に点滅やフラッシュ効果を付与するスクリプト．

#### パラメータ

- Duration: アニメーションの時間を指定
- Steps: 点滅の段階数 (ステップ数) を指定

- <details>
  <summary>Opacity</summary>

  - Opacity::Minimum: 不透明度の最小値を指定
  - Opacity::Maximum: 不透明度の最大値を指定

  </details>

- <details>
  <summary>Scale</summary>

  - Scale::Minimum: スケールの最小値を指定
  - Scale::Maximum: スケールの最大値を指定

  </details>

- <details>
  <summary>Edge Detection</summary>

  - Edge Detection::Intensity: エッジ抽出の強度を指定
  - Edge Detection::Threshold: エッジ抽出のしきい値を指定

  </details>

- <details>
  <summary>Color</summary>

  - Color::Source: 色付けのソースを指定
    - Image: 画像ファイル
    - Layer: 特定のレイヤー
  - Color::Image: (SourceがImageの場合) 画像ファイルを指定
  - Color::Layer: (SourceがLayerの場合) レイヤー番号を指定

  </details>

- <details>
  <summary>Additional Options</summary>

  - Unit: 時間の単位を指定
    - Frames: フレーム単位
    - Seconds: 秒単位
  - Layer Reference: レイヤーの参照方式を指定
    - Absolute: 絶対指定 (指定したレイヤー番号)
    - Relative: 相対指定 (自身のレイヤーからの相対番号)
  - Seed: ランダムシード値を指定

  </details>

### Echo@FlowType_K

初期ラベル: `Text`

各要素に対して残像 (エコー) を生成するスクリプト．

#### パラメータ

- Interval: 残像が発生する時間間隔を指定
- Count: 残像の数を指定
- Decay: 残像の減衰率 (不透明度) を指定
- Composite: 残像の合成順序を指定
    - Above: 元のオブジェクトの上に合成
    - Below: 元のオブジェクトの下に合成

- <details>
  <summary>Tint</summary>

  - Tint::Source: 色付けのソースを指定
    - Image: 画像ファイル
    - Layer: 特定のレイヤー
  - Tint::Image: 画像ファイルを指定
  - Tint::Layer: レイヤー番号を指定

  </details>

- <details>
  <summary>Additional Options</summary>

  - Unit: 時間や間隔の単位を指定
  - Layer Reference: レイヤー指定の参照方法を指定

  </details>

### オブジェクトメニュー

#### FlowType_K\\テキストを文字ごとに分解

テキストオブジェクトを1文字ごとに別オブジェクトとして分ける．

### プロパティメニュー

#### FlowType_K\\値を揃える

現在区間の値を選択した区間の値へコピーする．

- 全ての区間
- 以前の区間
- 以降の区間

#### FlowType_K\\値を反転

選択した区間の値を反転する． (トラックバーは符号の反転，チェックボックスはON/OFF反転)

- 現在区間
- 全ての区間
- 以前の区間
- 以降の区間

#### FlowType_K\\プロパティ名をコピー

プロパティ名を以下の形式でクリップボードへコピーする．(参照式用)

- {プロパティ名}
- {エフェクト名}.{プロパティ名}
- {レイヤー名}.{エフェクト名}.{プロパティ名}

#### FlowType_K\\エフェクトをコピー

エフェクトのパラメータをクリップボードへコピーする．

- エイリアス形式 (INI 形式): `.effect` 形式
- FlowType_K 形式 (TOML 形式): `Motion@FlowType_K` の `Effect::Parameters` で使用できる形式

## ビルド方法

[リリース用ワークフロー](./.github/workflows/releaser.yml)を参照されたい．

[extern](./plugins/extern/) 内 `vcpkg` ディレクトリに [vcpkg](https://github.com/microsoft/vcpkg) 本体を配置する必要がある．

## ライセンス

本プログラムのライセンスは [LICENSE](./LICENSE) を参照されたい．

また，本プログラムが利用するサードパーティ製ライブラリ等のライセンス情報は [THIRD_PARTY_LICENSES](./THIRD_PARTY_LICENSES.md) に記載している．

## 更新履歴

[CHANGELOG](./CHANGELOG.md) を参照されたい．

<!-- links -->

[releases-url]: https://github.com/korarei/AviUtl2_FlowType_K_Script/releases
[releases-badge]: https://img.shields.io/github/v/release/korarei/AviUtl2_FlowType_K_Script
[catalog-url]: https://aviutl2-catalog-badge.sevenc7c.workers.dev/package/korarei.FlowType_K
[catalog-badge]: https://aviutl2-catalog-badge.sevenc7c.workers.dev/badge/v/korarei.FlowType_K
