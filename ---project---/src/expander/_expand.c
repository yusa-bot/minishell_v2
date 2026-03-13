// *

> echo '*'
*
> echo "*"
*

> VAR="*"
> echo $VAR
Applications Desktop Documents Downloads Library Movies Music Pictures Public
> echo "$VAR"
*

> echo "$VAR"HOME
*HOME
> $"VAR"HOME
VARHOME


// リダイレクト

* が含まれているか（かつクォートされていないか）を確認
redir->filename を実行直前にワイルドカード展開にかける
展開結果の個数を数える。

一致するファイルが1つ: リダイレクト先として採用
一致するファイルが複数: エラーメッセージ: bash: *.c: ambiguous redirect
一致するファイルが0個: ワイルドカード展開が行われず、*.c という名前のファイルそのものを作る
	(>, >>) の場合: *.cという名前のファイルを作成
	(<) の場合: openが失敗
