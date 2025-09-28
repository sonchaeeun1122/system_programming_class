#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

int is_txt_file(const char *file_name) {
    const char *dot = strrchr(file_name, '.');
    if (dot != NULL && dot != file_name) {
        return strcmp(dot + 1, "txt") == 0;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "사용법: %s <source_directory> <output_file>\n", argv[0]);
        exit(1);
    }

    char *source_directory = argv[1];
    char *output_file = argv[2];

    DIR *dir;
    struct dirent *dp;
    int input_fd, output_fd;
    char buffer;

    if ((dir = opendir(source_directory)) == NULL) {
        perror(source_directory);
        exit(2);
    }

    output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd == -1) {
        perror(output_file);
        closedir(dir);
        exit(3);
    }

    while ((dp = readdir(dir)) != NULL) {
        if (dp->d_type == DT_REG && is_txt_file(dp->d_name)) {
            printf("처리 중인 파일 : %s\n", dp->d_name);
            char fpath[1024];
            sprintf(fpath, "%s/%s", source_directory, dp->d_name);

            input_fd = open(fpath, O_RDONLY);
            if (input_fd == -1) {
                perror(fpath);
                continue;
            }

            while (read(input_fd, &buffer, 1) > 0) {
                if (buffer != ' ') {
                    if (write(output_fd, &buffer, 1) == -1) {
                        perror("출력 파일 쓰기 오류");

                        close(input_fd);
                        close(output_fd);
                        closedir(dir);
                        exit(4);
                    }
                }
            }
            close(input_fd);
        }
    }

    printf("병합이 완료되었습니다.");
    closedir(dir);
    close(output_fd);
    return 0;
}