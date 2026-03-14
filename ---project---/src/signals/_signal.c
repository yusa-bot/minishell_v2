A: プロンプトで入力を待っている
	Ctrl-C (SIGINT): 「新しい行に新しいプロンプトを表示」
		ハンドラの実装: g_sig に SIGINT をセットし、readline の内部状態をリセットしてプロンプトを再描画
	Ctrl-\ (SIGQUIT): 「何も行いません」。
		実装: SIG_IGN（無視）を設定します。

B：コマンド実行中 (子プロセス)
	Ctrl-C や Ctrl-\: 実行中のコマンド（子プロセス）だけが終了し、Minishell本体（親）は終了せずに次のプロンプトに
		Ctrl-\ の場合は Quit: 3
		実装（親プロセス）: シグナルを無視
		実装（子プロセス）: fork() の直後、execve() を呼ぶ前に、シグナルの設定をOSのデフォルト（SIG_DFL）に戻

C：ヒアドキュメント (<<) 入力中
	Ctrl-C (SIGINT): ヒアドキュメント入力を即座にキャンセルし、AST全体の実行を中止してプロンプトに戻
		実装: 子プロセス側の SIGINT をデフォルト（SIG_DFL）に -> Ctrl-C を押すと子プロセスが SIGINT で即死
			親プロセスは waitpid で WIFSIGNALED を検知し、実行をキャンセル
	Ctrl-\ (SIGQUIT): 何も起こりません。
		実装: 子プロセス側の SIGQUIT を SIG_IGN（無視）にします


* Ctrl-D: EOFを送る操作であるため、シグナルハンドラではなく readline の戻り値で処理
	対話モード:
		実装: readline() が NULL を返した場合、ユーザーが Ctrl-D を押したことを意味
			-> ループを抜けてシェルを終了（exit）させます。終了前に exit\n と画面に出力
	ヒアドキュメント中:
		同様に readline() が NULL を返したら、EOFが来たことを意味します。エラーにはせず、ループを抜けてそこまでに読み込んだ内容で処理を継続

全体の流れ
	メインループの開始時に set_signal_interactive() を呼ぶ。
	コマンドをパースし、実行フェーズに入る直前で set_signal_executing() を呼び、親がシグナルで死なないように防御する。
	fork() して生まれた子プロセスの中で、set_signal_child() を呼んでデフォルトの挙動に戻してから execve する。
	コマンドの実行が終わり、親の waitpid ループを抜けたら、次の入力を待つ前に再び set_signal_interactive() に戻す。