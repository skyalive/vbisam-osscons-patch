---
title: "Spec Draft: 02_binary_compare_tool - Binary compare dump utility"
type: "spec-draft"
status: "draft"
summary: "IDCAMS PRINT 風のレコード単位ダンプと差分表示を行う /test 配下のユーティリティ。"
related_docs:
  - "spec/README.md"
related_materials:
  - "https://www.ibm.com/docs/en/zos/2.5.0?topic=statements-print-statement"
---
# Spec Draft: 02_binary_compare_tool - Binary compare dump utility

## Goal
- IDCAMS PRINT 風のレコード単位ダンプ（HEX + 文字表示）を出力する。
- 2ファイルを比較し、差分があるレコードの先頭にチェックマークを付ける。

## Scope
- `/test` 配下に配置するユーティリティ（実装言語は後決定）。
- DUMP モードのみ（比較以外の編集・修復は対象外）。

## CLI (案)
```
bincompare dump --record-length 256 fileA.bin fileB.bin
bincompare dump --mode line fileA.txt fileB.txt
```

### Options (案)
- `--record-length, -r <N>`: レコード長（必須）
- `--mode <fixed|line>`: レコード境界の判定方式（既定: fixed）
- `--width, -w <N>`: 1行のバイト数（既定: 16）
- `--limit <N>`: 出力レコード数上限
- `--offset <N>`: 先頭からのスキップレコード数

## Output Format (案)
- レコード単位で、HEX と文字表現（非ASCIIは `.`）を並記。
- 2ファイル比較時は並列表示（左右に並べる）。
- レコードに差分がある場合、レコード先頭にチェックマーク `*` を付ける。

### 単一ファイル例
```
REC 000001  0000:  01 02 03 04 05 06 07 08  09 0A 0B 0C 0D 0E 0F 10  |................|
           0010:  41 42 43 44 45 46 47 48  49 4A 4B 4C 4D 4E 4F 50  |ABCDEFGHIJKLMNOP|
```

### 2ファイル比較例
```
* REC 000001
L 0000:  01 02 03 04 05 06 07 08  09 0A 0B 0C 0D 0E 0F 10  |................|
R 0000:  01 02 03 04 05 06 07 08  09 0A 0B 0C 0D 0E FF 10  |...............?|
```

## Comparison Rules (案)
- `mode=fixed`: レコード境界は固定長 `record-length` を基準とする。
- `mode=line`: 改行コードでレコード境界を判定する（改行コードは表示せず、レコード末尾扱い）。
- 同じレコード番号でバイト列が一致しない場合、差分とみなす。
- ファイル長が短い場合は欠損レコードとして差分扱い（表示は `--` で埋める）。

## Line mode warning
- コンペア対象の改行コードが異なる場合はワーニングを出力する（LF/CRLF/CRの混在を検出）。
- ワーニングは表示のみで、処理は継続する。

## Compare Behavior (案)
- `fileA fileB` の場合は左右並列ダンプ。

## Non-goals
- テキスト行単位の差分表示（改行区切り）
- 高度な差分アルゴリズム（LCS等）
- バイナリ変換やエンコーディング変換

## Open Questions
- `/test` 配下の配置は `tests/` ではなく `test/` でよいか？
- チェックマーク記号は `*` 固定で良いか？

## Next Steps
- 仕様確定（Open Questions の回答）。
- CLI オプションの確定とフォーマットの最終化。
