pipeは2つを子ロセスで実行 かつ fdを繋ぐ という2つの手法が入り組んでいる

pipe(fd):
	fd[0]:書(左), fd[1]:読(右) のプロセスを繋げる

fork(): pids[0](左)の子プロ で fd[1] を書き込みにする -> |の左のコマンド exec_ast(node->left, env_list) を実行
fork(): pids[1](右)の子プロ で fd[0] を読み込みにする -> |の右のコマンド exec_ast(node->right, env_list) を実行

