/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:35 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/15 16:56:43 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

//open: modeを「ファイルがない場合、作成される」にする

// redirect処理
// t_redirect->nextがある間リストを回し、リダイレクトの種類によって処理を振り分け
// 		複数t_redirectでも、最後のFDで上書き
int apply_redirects(t_redirect *redirects); // t_node->t_redirect (1nodeづつ)
// 入力 < とヒアドキュメント << の処理. open -> dup2 -(heredocメモ確認)
static int handle_input(t_redirect *redir);
// 出力 > と追記 >> の処理. open -> dup2
static int handle_output(t_redirect *redir);

// バックアップ
// バックアップしておいた標準入出力のFDを dup2 で元に戻す
void restore_fds(int saved_stdin, int saved_stdout);


// redirect処理 ---------------------------------------------
// t_redirect->nextがある間リストを回し、リダイレクトの種類によって処理を振り分け
// 		複数t_redirectでも、最後のFDで上書き
int apply_redirects(t_redirect *redirects) // t_node->t_redirect (1nodeづつ)
{
	t_redirect	*current;

	current = redirects;

	// t_redirect->nextがある間
	while (current)
	{
		// input(<, <<)
		if (current->type == TK_REDIR_IN || current->type == TK_HEREDOC)
		{
			if (handle_input(current) < 0)
				return (1);
		}

		// output(>, >>)
		else if (current->type == TK_REDIR_OUT || current->type == TK_REDIR_APPEND)
		{
			if (handle_output(current) < 0)
				return (1);
		}

		current = current->next;
	}

	return (0);

	//while (redirects) でループし、
	//type に応じて open(O_RDONLY) や open(O_WRONLY | O_CREAT | O_TRUNC) を実行
	//成功したら dup2(fd, STDIN_FILENO) または dup2(fd, STDOUT_FILENO)
	//繋ぎ終わった fd は close(fd) してリソースリークを防ぐ
}

// 入力 < とヒアドキュメント << の処理. open -> dup2 -(heredocメモ確認)
static int handle_input(t_redirect *redir)
{
	int	fd;

	// open: ファイルとの接続口を作る
	fd = open(redir->filename, O_RDONLY); // O_RDONLY:読み込み専用

	// 失敗
	if (fd < 0)
	{
		perror(redir->filename);
		return (-1);
	}

	// heredoc -> unlink
	if (redir->type == TK_HEREDOC)
		unlink(redir->filename);

	// STDIN_FILENOをfdで上書き
	dup2(fd, STDIN_FILENO);
	close(fd); // heredoc -> STDIN_FILENOとして参照は残る

	return (0);
}

// 出力 > と追記 >> の処理. open -> dup2
static int handle_output(t_redirect *redir)
{
	int	fd;

	// open: ファイルとの接続口を作る
	if (redir->type == TK_REDIR_OUT) // >
		fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644); // O_TRUNC:既存の中身を空
	else // >>
		fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644); // O_APPEND:追記

	// 失敗
	if (fd < 0)
	{
		perror(redir->filename);
		return (-1);
	}

	// STDOUT_FILENOをfdで上書き
	dup2(fd, STDOUT_FILENO);
	close(fd);

	return (0);
}

// バックアップ ---------------------------------------------
// バックアップしておいた標準入出力のFDを dup2 で元に戻す
void restore_fds(int saved_stdin, int saved_stdout)
{
	// STDIN_FILENO(変更後の現在のSTDIN)をsaved_stdinで上書き
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdin);

	// STDOUT_FILENO(変更後の現在のSTDOUT)をsaved_stdoutで上書き
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdout);
}
