/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:59 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/15 18:04:39 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

// 受信したシグナルのみを示す
volatile sig_atomic_t g_sig = 0;

int main(int argc, char **argv, char **envp)
{
	char    *line;
	t_env   *env_list;
	t_token *tokens;  //1token
	t_node  *node; //1node
	int     exit_status;

	(void)argc;
    (void)argv;

	env_list = env_init(envp);
	// (tcgetattr)

	while (1)
	{
		// プロンプト待機中のシグナルハンドラを設定
		set_signal_interactive();

		line = readline("minishell$ ");

		// Ctrl-D (EOF)
        if (line == NULL)
        {
            printf("exit\n");
            break;
        }

        if (*line != '\0')
            add_history(line);

		// lexer
		tokens = tokenize(line);
		if (tokens == NULL)
        {
            free(line);
            continue;
        }

		// parser
		node = parse(&tokens);
		if (node == NULL) // Syntax error
        {
            free_tokens(tokens);
            free(line);
            continue;
        }

		// heredoc: 実行を開始する前に、AST全体を走査して全てのヒアドキュメントの入力を完了
		if (process_heredoc(node, env_list))
		{ // 失敗
            free_tokens(tokens);
            free(line);
            continue; // Ctrl-Cでは、AST全体の実行をキャンセルし、新しいプロンプトに戻る
        }

		// コマンド実行中のシグナルハンドラに切り替え (親プロでは無視)
		set_signal_executing();

		// Executor
		exec_ast(node, &env_list); // 展開 -> 実行

		// いらない？
		// rl_on_new_line(); // readlineに次の入力へと伝える
		// rl_replace_line("", 0); // 現在の行を空に
		// rl_redisplay(); // 空にした後、$ で入力待機

		// cleanup
		free_ast(node);
        free_tokens(tokens);
        free(line);
	}

	rl_clear_history(); // history削除
	exit_status = ft_atoi(get_env_value(env_list, "?"));
	free_env(env_list);

	return (exit_status);
}
