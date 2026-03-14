/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:09:47 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/14 15:45:59 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//				env       export (引数なし)
//フォーマット   KEY=VALUE    declare -x KEY="VALUE", （VALUE）が必ずダブルクォート（"）
//ソート   	  そのまま  	ASCIIコード順
//値なし変数	   表示なし	  	 表示(declare -x ONLY_KEY)
//_			 あり 		  なし

今後の課題：_ の更新
もし Bash と全く同じ挙動（実行したコマンドに応じて _ が変わる）を目指すなら、exec_external 内の execve を呼ぶ直前に、set_env_value(env_list, "_", path) を実行する必要があります。
Note:
env コマンド自体も実行ファイルなので、Bash で env と打つと、その瞬間 _ は /usr/bin/env に書き換わって表示されます。

int	builtin_env(t_env *env_list)
{
	while (env_list)
	{
		if (env_list->value)
		{
			ft_putstr_fd(env_list->key, STDOUT_FILENO);
			ft_putchar_fd('=', STDOUT_FILENO);
			ft_putendl_fd(env_list->value, STDOUT_FILENO);
		}
		env_list = env_list->next;
	}
	return (0);
}

