#include <fcntl.h>

int main()
{
#if defined(SYNC_FILE_RANGE_WRITE ) || defined(SYNC_FILE_RANGE_WAIT_BEFORE)

    int fd = open("file.txt", O_WRONLY | O_CREAT);
    sync_file_range(fd, 0, 0, SYNC_FILE_RANGE_WRITE);
    sync_file_range(fd, 0, 0, SYNC_FILE_RANGE_WAIT_BEFORE);

#endif

    return 0;
}
