---
title: "Tech Memo: 01_extfh_recommendation - VBISAM変更とEXTFH推奨判断"
type: "tech-memo"
status: "approved"
summary: "VBISAMモダナイゼーション後のGnuCOBOL 3.3への影響確認とEXTFH推奨の判断メモ。"
related_docs:
  - "spec/README.md"
related_materials:
  - "/Users/uemonmac/dev/__claude/vbisam/gnucobol-3.3-dev/gnucobol-3.3-dev/libcob/fileio.c"
  - "/Users/uemonmac/dev/__claude/extfh_sample/grfh/specs/00_index.md"
---
# Tech Memo: 01_extfh_recommendation - VBISAM変更とEXTFH推奨判断

## 結論
- EXTFH経由の利用は「推奨」とするが「必須化」は不要。

## 理由
- VBISAM APIの変更は互換範囲内（`const char *`化、enum化、エラーパス改善）。
- GnuCOBOL側のVBISAM利用は `libcob/fileio.c` で `vbisam.h` を直接参照しているが、`char *` から `const char *` への受け渡しは問題なし。
- 故障注入フック（環境変数）は設定されない限り通常運用に影響なし。

## 影響範囲の確認結果
- GnuCOBOL 3.3 側のVBISAM呼び出し箇所は主に `libcob/fileio.c`。
- 呼び出し例: `isopen((void *)filename, ...)`, `iserase((void *)filename)`, `isbuild((void *)filename, ...)`

## 推奨方針
- 安全性・拡張性を優先する場合は EXTFH を推奨。
- ただし今回の変更を理由に EXTFH を必須とする必要はない。

## メモ
- 将来的にISAM実装を差し替える可能性がある場合、EXTFHは有効な緩衝層になる。
