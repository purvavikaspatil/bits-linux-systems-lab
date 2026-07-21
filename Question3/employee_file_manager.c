#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define FILENAME     "employees.dat"
#define NAME_LEN     30
#define ID_LEN       10

typedef struct {
    char emp_id[ID_LEN];
    char name[NAME_LEN];
    float salary;
    int  active;
} EmployeeRecord;

#define RECORD_SIZE sizeof(EmployeeRecord)

int create_or_open_file(const char *filename) {
    int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open (create) failed");
        exit(EXIT_FAILURE);
    }
    return fd;
}

void write_record(int fd, EmployeeRecord *rec) {
    if (lseek(fd, 0, SEEK_END) == -1) {
        perror("lseek (append) failed");
        return;
    }

    ssize_t written = write(fd, rec, RECORD_SIZE);
    if (written == -1) {
        perror("write failed");
    } else if ((size_t)written != RECORD_SIZE) {
        fprintf(stderr, "Partial write detected for record %s\n", rec->emp_id);
    }
}

int update_record(int fd, int index, EmployeeRecord *new_data) {
    off_t offset = (off_t)index * RECORD_SIZE;

    off_t pos = lseek(fd, offset, SEEK_SET);
    if (pos == -1) {
        perror("lseek (update) failed");
        return -1;
    }

    ssize_t written = write(fd, new_data, RECORD_SIZE);
    if (written == -1) {
        perror("write (update) failed");
        return -1;
    } else if ((size_t)written != RECORD_SIZE) {
        fprintf(stderr, "Partial update at record %d\n", index);
        return -1;
    }

    return 0;
}

int read_record(int fd, int index, EmployeeRecord *out) {
    off_t offset = (off_t)index * RECORD_SIZE;

    off_t pos = lseek(fd, offset, SEEK_SET);
    if (pos == -1) {
        perror("lseek (read) failed");
        return -1;
    }

    ssize_t n = read(fd, out, RECORD_SIZE);
    if (n == 0) {
        return 0;
    } else if (n == -1) {
        perror("read failed");
        return -1;
    } else if ((size_t)n != RECORD_SIZE) {
        fprintf(stderr, "Corrupted/partial record at index %d\n", index);
        return -1;
    }

    return 1;
}

void print_all_records(int fd) {
    EmployeeRecord rec;

    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek (rewind) failed");
        return;
    }

    int index = 0;
    ssize_t n;
    while ((n = read(fd, &rec, RECORD_SIZE)) > 0) {
        if ((size_t)n < RECORD_SIZE) {
            fprintf(stderr, "Truncated record at index %d, stopping.\n", index);
            break;
        }
        if (rec.active) {
            printf("[%d] ID: %-10s Name: %-30s Salary: %.2f\n",
                   index, rec.emp_id, rec.name, rec.salary);
        }
        index++;
    }
    if (n == -1) {
        perror("read (scan) failed");
    }
}

int main(void) {
    int fd = create_or_open_file(FILENAME);

    EmployeeRecord e1 = {"E001", "Alice Sharma", 55000.0f, 1};
    EmployeeRecord e2 = {"E002", "Rohan Verma",  48000.0f, 1};
    EmployeeRecord e3 = {"E003", "Meera Iyer",   62000.0f, 1};

    write_record(fd, &e1);
    write_record(fd, &e2);
    write_record(fd, &e3);

    printf("---- Initial Records ----\n");
    print_all_records(fd);

    EmployeeRecord updated = {"E002", "Rohan Verma", 53000.0f, 1};
    update_record(fd, 1, &updated);

    printf("\n---- After Updating Record 1 ----\n");
    print_all_records(fd);

    EmployeeRecord fetched;
    int status = read_record(fd, 2, &fetched);
    if (status == 1) {
        printf("\n---- Direct Fetch of Record 2 ----\n");
        printf("ID: %s, Name: %s, Salary: %.2f\n",
               fetched.emp_id, fetched.name, fetched.salary);
    }

    if (close(fd) == -1) {
        perror("close failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}
