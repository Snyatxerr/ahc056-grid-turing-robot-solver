# AHC056 Solver

AtCoder Heuristic Contest 056 **「Grid Turing Robot」** 用の C++17 ソルバです。

## 概要

グリッド上のロボットが指定された目的地を順番に通過できるように、各マスの色と状態遷移規則を構成する問題です。

このコードでは、目的地を順番に結ぶ経路を作成し、その経路をロボットが実行できる色・状態遷移規則へ変換します。

問題ページ: <https://atcoder.jp/contests/ahc056>

## 解法

各目的地間では最短経路を保ちながら、同じマスをできるだけ単純な挙動で再利用できるように経路を選びます。

経路が決まった後、直進や左右の旋回パターンを色と状態遷移へ変換します。短い旋回パターンは状態を共有し、それ以外は個別の状態で処理しています。

## ビルド

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

`g++` で直接ビルドする場合:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic src/main.cpp -o ahc056_solver
```

標準入力から1ケースを読み込み、標準出力へ解を出力します。

## ライセンス

MIT License。詳細は [`LICENSE`](LICENSE) を参照してください。
