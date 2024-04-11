#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main() {

    pid_t volume_control_pid = fork();
    if(volume_control_pid == 0){
        if(execlp("./audio_control","./audio_control", NULL) == -1) {
            perror("execlp");
            exit(EXIT_FAILURE);
        }
    }
    else{
        printf("%d\n",getpid());
        char cpid[10];
        sprintf(cpid, "%d", volume_control_pid);
        if(execlp("/usr/bin/python3", "python3", "music_player.py",cpid, NULL) == -1) {
            perror("execlp"); // 오류 메시지 출력
            exit(EXIT_FAILURE); // 실패 시 자식 프로세스 종료
        }
    }
}