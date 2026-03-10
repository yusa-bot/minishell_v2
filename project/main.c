
int result_fork;
if (result_fork = fork() == -1)

do_cmd()
{
	if (access(const char *pathname, X_OK)) // X_OK : 実行可能か
	{
		// 失敗
	}
}

int main ()
{
	char *shell_prompt;

	// $ 待機
	while (shell_prompt = readline("yusa_minishell $") != NULL)
	{
		if (shell_prompt[0] != '\0')
		{
			add_history(shell_prompt);
			rl_on_new_line(); // readlineに次の入力へと伝える
			rl_replace_line("", 0); // 現在の行を空に
			rl_redisplay(); // 空にした後、$ で入力待機
		}
		free(shell_prompt); // readline返り値
	}
	rl_clear_history(); // history削除
    return 0;
}