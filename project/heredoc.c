/tmp/heredoc_tmp 等に書き込み

int fd = open("/tmp/heredoc_tmp", O_RDONLY);
unlink("/tmp/heredoc_tmp"); // fd が生きているため、データの実体はメモリ/ディスク上に残り

親プロセスが fork した後、子プロセス内
子プロセスには fd（例: 番号 3）と、標準入力 STDIN_FILENO（番号 0）があります

dup2(fd, STDIN_FILENO) // 「番号 0（標準入力）を、番号 fd が指しているのと同じファイル実体に繋ぎかえろ」
close(fd);

execve("/bin/cat", ...)