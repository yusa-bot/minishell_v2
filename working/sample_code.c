#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <term.h>

// 1
 int main()
 {
     pid_t pid = fork(); // fork: プロセスを複製

     if (pid == 0)
 	{
         // 子プロセス
         char *args[] = {"/bin/ls", "-l", NULL};
         char *envp[] = {NULL};
         // execve: 別のプログラムに置き換えて実行
 		// sleep(3);
         execve(args[0], args, envp);
     } else if (pid > 0)
 	{
         // 親プロセス
         int status;

         // waitpid: 特定の子プロセスの終了を待機
         waitpid(pid, &status, 0);

         // 代替手段として以下の関数もあります：
         // wait(&status); // 任意の子プロセスを待機
         // struct rusage usage;
         // wait3(&status, 0, &usage); // リソース使用量も取得 (旧式)
         // wait4(pid, &status, 0, &usage); // 特定プロセス＋リソース使用量
     }
     return 0;
 }

 // 2
 void handle_sigint(int sig) {
     printf("Caught signal %d\n", sig);
 }

 int main()
 {
     // signal
     signal(SIGUSR1, handle_sigint);

     struct sigaction sa;
     sa.sa_handler = handle_sigint;

     // sigemptyset, sigaddset: シグナルマスクの初期化と追加
     sigemptyset(&sa.sa_mask);
     sigaddset(&sa.sa_mask, SIGQUIT);
     sa.sa_flags = 0;

     // sigaction: より堅牢なシグナルハンドラ設定
     sigaction(SIGINT, &sa, NULL);

     // kill: 指定したプロセス（ここでは自分自身）にシグナルを送信
     // kill(getpid(), SIGINT);

     return 0;
 }

// 3
 int main() {
     char cwd[1024];
     // getcwd: 現在のディレクトリを取得
     if (getcwd(cwd, sizeof(cwd)) != NULL) {
         printf("Current dir: %s\n", cwd);
     }

     // chdir: ディレクトリの移動
     chdir("/tmp");

     // access: ファイルのアクセス権限（読み取り可能かなど）を確認
     if (access("test.txt", R_OK) == 0) {
         struct stat st;
         // stat: ファイルの詳細情報を取得 (シンボリックリンクは辿る)
         stat("test.txt", &st);
         // lstat: シンボリックリンク自体の情報を取得
         lstat("test.txt", &st);
         // fstat: ファイルディスクリプタから情報を取得
         // fstat(fd, &st);

         // unlink: ファイル（ハードリンク）の削除
         unlink("test.txt");
     }

     // opendir, readdir, closedir: ディレクトリ内のファイル一覧取得
     DIR *dir = opendir(".");
     if (dir) {
         struct dirent *entry;
         while ((entry = readdir(dir)) != NULL) {
             printf("%s\n", entry->d_name);
         }
         closedir(dir);
     }
     return 0;
 }

// 4
 int main()
 {
     int fd[2];
     pipe(fd);

     pid_t pid = fork();
     if (pid == 0) {
         close(fd[0]);
         dup2(fd[1], STDOUT_FILENO);
         close(fd[1]);
         printf("Hello from child\n");
     } else {
         close(fd[1]);
         char buffer[100];
         read(fd[0], buffer, sizeof(buffer));

         // dup: ファイルディスクリプタを空いている最小の番号に複製
         int new_fd = dup(fd[0]);
         close(fd[0]);
         close(new_fd);
     }
     return 0;
 }

 // 5
 int main() {
     FILE *f = fopen("non_existent_file.txt", "r");
     if (!f) {
         // perror: 自動的に "プレフィックス: エラー内容" を標準エラー出力に出す
         perror("Error opening file");

         // strerror: errno番号に対応するエラー文字列を返す
         printf("Detail: %s\n", strerror(errno));
     }
     return 0;
 }

// 6
int main() {
    // isatty: 標準入力が端末(TTY)かどうかを判定
    if (isatty(STDIN_FILENO)) {
        // ttyname: 端末のデバイス名を返す
        printf("TTY Name: %s\n", ttyname(STDIN_FILENO));
        // ttyslot: 現在の端末のインデックスを返す
        printf("TTY Slot: %d\n", ttyslot());
    }

    // getenv: 環境変数の取得
    char *termtype = getenv("TERM");

    struct termios term;
    // tcgetattr: 端末の現在の設定を取得
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ECHO); // エコーをオフにする設定例
    // tcsetattr: 端末の設定を適用
    // tcsetattr(STDIN_FILENO, TCSANOW, &term);

    struct winsize ws;
    // ioctl: 端末のウィンドウサイズなどを取得・制御する汎用インターフェース
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);

    // --- Termcap系 (旧式、現在はterminfoが主流ですが仕組みの例として) ---
    char term_buffer[2048];
    // tgetent: 端末データベースからエントリを読み込む
    if (tgetent(term_buffer, termtype) == 1) {
        // tgetflag: 真偽値の機能を取得
        int am = tgetflag("am");
        // tgetnum: 数値の機能を取得（例：列数）
        int cols = tgetnum("co");

        char area[1024];
        char *ap = area;
        // tgetstr: 文字列の機能（制御エスケープシーケンス）を取得（例：画面クリア）
        char *clear_str = tgetstr("cl", &ap);

        if (clear_str) {
            // tputs: 制御文字列をパディングなどを処理して出力
            // tputs(clear_str, 1, putchar);

            // tgoto: カーソル移動などのパラメータ付き文字列を生成
            // char *move_cursor = tgoto(tgetstr("cm", &ap), 10, 5);
        }
    }
    return 0;
}
