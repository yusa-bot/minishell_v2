/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:10 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/12 14:13:26 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// なぜ履歴でheredoc内容も出るのか?


///tmp/heredoc_tmp 等に書き込み

//int fd = open("/tmp/heredoc_tmp", O_RDONLY);
//unlink("/tmp/heredoc_tmp"); // fd が生きているため、データの実体はメモリ/ディスク上に残り

//親プロセスが fork した後、子プロセス内
//子プロセスには fd（例: 番号 3）と、標準入力 STDIN_FILENO（番号 0）があります

//dup2(fd, STDIN_FILENO) // 「番号 0（標準入力）を、番号 fd が指しているのと同じファイル実体に繋ぎかえろ」
//close(fd);

//execve("/bin/cat", ...)

----

process_heredoc でASTを巡回し、TK_HEREDOC を見つける
readline("> ") で入力を受け付け、専用の一時ファイル（例: /tmp/.heredoc_tmp1）に書き込む
t_redirect 構造体の filenameを、作成した一時ファイルのパスに書き換える

実行時（exec_cmd.c）には、ただの一時ファイルの入力リダイレクト（< /tmp/.heredoc_tmp1）
	として処理し、open した直後に unlink してファイルを削除


---

// AST全体を走査し、TK_HEREDOC があれば入力を処理する（再帰）
int	process_heredoc(t_node *node, t_env *env_list);
// 実際に readline を回して一時ファイルに書き込む。ここでは、一時ファイルに書き込むまで
static int	read_heredoc(t_redirect *redir, t_env *env_list);

// suffix取得し一意のfilename作成
static char	*generate_tmp_filename(void);
// /dev/urandom から乱数を読み取り、8文字の16進数文字列を生成する
static char	*get_random_suffix(void);


// AST全体からheredocがあれば、入力し、、一時ファイルに書き込むまでを行う ---------------------------------------------

// AST全体を走査し、TK_HEREDOC があれば入力を処理する（再帰）
int	process_heredoc(t_node *node, t_env *env_list)
{
	t_redirect	*redir;

	if (node == NULL)
		return (0);

	// t_node->t_redirectがあるのはNODE_CMDのみ
	if (node->type == NODE_CMD)
	{
		// t_redirectがある間、nextで辿る
		redir = node->redirects;
		while (redir != NULL)
		{
			if (redir->type == TK_HEREDOC)
			{
				if (read_heredoc(redir, env_list) != 0)
					return (1); // 失敗（Ctrl-Cなどで中断された場合）
			}
			redir = redir->next;
		}
		else // それ以外のノード（|, &&, ||, ()）は左右の子を再帰的に走査
		{
			if (process_heredoc(node->left, env_list) != 0)
				return (1);
			if (process_heredoc(node->right, env_list) != 0)
				return (1);
		}
	}

	return (0);
}

// 実際に readline を回して一時ファイルに書き込む。ここでは、一時ファイルに書き込むまで
static int	read_heredoc(t_redirect *redir, t_env *env_list)
{
	int		fd;
	char	*line;
	char	*tmp_filename;

	// 一意な一時ファイルを生成
	tmp_filename = generate_tmp_filename();

	fd = open(tmp_filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd < 0)
	{
		free(tmp_filename);
		return (1); // 失敗
	}

	// heredoc読み込み
	while (1)
	{
		line = readline("heredoc> ");

		// heredoc読み込み中の、Ctrl-D (EOF) または Ctrl-C による中断
		// 		-> Ctrl-C:perompt処理自体を中断, Ctrl-D:heredoc読み込みはそこで中断
		if (line == NULL)
			break ;

		// デリミタと一致したら終了 (parserで、<<の横の単語を登録済)
		if (ft_strcmp(line, redir->filename) == 0)
		{
			free(line);
			break ;
		}

		// 環境変数の展開 (デリミタにクオーとが無い場合、$は展開される)
		//expand_heredoc_vars(&line, env_list);

		// 一時ファイルに書き込む
		ft_putendl_fd(line, fd);

		free(line);
	}

	// ファイルへの書き込みは終わったので、書き込み口は不要
	close(fd);

	// >読み込み中Ctrl-Cでは、g_sigにSIGINTが設定される
	//		-> AST全体の実行をキャンセルし、新しいプロンプトに戻る
	if (g_sig == SIGINT)
	{
		unlink(tmp_filename);
		free(tmp_filename);
		return (1);
	}

	// heredocを読み終わったら、
	//		t_redirect->filenameを、デリミタ文字列から一時ファイル名にする。
	//		heredoc以外のリダイレクトも本来はfilenameはファイル名なので、動作が揃う
	free(redir->filename);
	redir->filename = tmp_filename;

	return (0);
}

// 一意な一時ファイル名を生成 ---------------------------------------------

// suffix取得し一意のfilename作成
static char	*generate_tmp_filename(void)
{
	char	*suffix;
	char	*filename;

	while (1)
	{
		// suffix取得
		suffix = get_random_suffix();
		if (!suffix)
			return (NULL);

		// 一意のfilename作成
		filename = ft_strjoin("/tmp/.minishell_heredoc_", suffix);
		free(suffix);
		if (!filename)
			return (NULL);

		if (access(filename, F_OK) == -1)
			break ; // -1=存在しないので、一意なファイル名として採用

		// 既に存在した場合、再生成ループ
		free(filename);
	}
	return (filename);
}

// /dev/urandom から乱数を読み取り、8文字の16進数文字列を生成する
static char	*get_random_suffix(void)
{
	int				fd;
	char			*hex = "0123456789abcdef";
	char			*suffix;
	unsigned char	buf[4]; // 4バイト = 8文字の16進数
	int				i;

	suffix = (char *)malloc(sizeof(char) * 9);
	if (!suffix)
		return (NULL);

	// 乱数を読み込み
	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
	{
		free(suffix);
		return (NULL);
	}
	read(fd, buf, 4);
	close(fd);

	// 乱数からsuffixを生成
	i = 0;
	while (i < 4)
	{
		suffix[i * 2] = hex[buf[i] / 16];
		suffix[i * 2 + 1] = hex[buf[i] % 16];
		i++;
	}
	suffix[8] = '\0';

	return (suffix);
}
