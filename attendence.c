/* =========================================================
   ATTENDANCE MANAGEMENT SYSTEM  (CRUD + REPORT TABLE)
   Language : C
   ========================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STUDENTS 100
#define MAX_DATE_LEN 20

/* -------------------- STRUCTURES -------------------- */

struct Student {
    int roll;
    char name[50];
    int total_present_hours;
};

struct AttendanceRecord {
    int roll;
    char date[MAX_DATE_LEN];
    int hour_num;
    char status;   // P or A
};

/* -------------------- GLOBAL DATA -------------------- */

struct Student students[MAX_STUDENTS];
int student_count = 0;
int total_hours_conducted = 0;

const char *STUDENTS_FILE = "students.dat";
const char *ATTENDANCE_FILE = "attendance.dat";

/* -------------------- FUNCTION DECLARATIONS -------------------- */

void load_all_data();
void save_students();
void clear_input_buffer();
void read_line(char *buf, int size);
void calculate_total_conducted_hours();

/* CRUD */
void add_student();
void view_students();
void search_student();
void update_student();
void delete_student();

/* Attendance */
void mark_attendance();
char get_attendance_input(const char *prompt);

/* Reports */
void generate_summary_report();

/* -------------------- MAIN -------------------- */

int main() {
    load_all_data();
    calculate_total_conducted_hours();

    int choice;

    while (1) {
        printf("\n=============== ATTENDANCE SYSTEM ===============\n");
        printf("1. Add Student\n");
        printf("2. View Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. Mark Attendance\n");
        printf("7. Generate Report\n");
        printf("8. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("Invalid input!\n");
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1: add_student(); break;
            case 2: view_students(); break;
            case 3: search_student(); break;
            case 4: update_student(); break;
            case 5: delete_student(); break;
            case 6: mark_attendance(); break;
            case 7: generate_summary_report(); break;
            case 8: printf("Saving and exiting...\n"); exit(0);
            default: printf("Invalid choice!\n");
        }
    }
}

/* -------------------- UTILITIES -------------------- */

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void read_line(char *buf, int size) {
    fgets(buf, size, stdin);
    buf[strcspn(buf, "\n")] = '\0';
}

void load_all_data() {
    FILE *fp = fopen(STUDENTS_FILE, "rb");
    if (fp) {
        while (fread(&students[student_count], sizeof(struct Student), 1, fp) == 1) {
            student_count++;
        }
        fclose(fp);
    }
}

void save_students() {
    FILE *fp = fopen(STUDENTS_FILE, "wb");
    fwrite(students, sizeof(struct Student), student_count, fp);
    fclose(fp);
}

void calculate_total_conducted_hours() {
    total_hours_conducted = 0;

    FILE *fp = fopen(ATTENDANCE_FILE, "rb");
    if (!fp || student_count == 0) return;

    struct AttendanceRecord rec;
    int check_roll = students[0].roll;

    while (fread(&rec, sizeof(rec), 1, fp) == 1) {
        if (rec.roll == check_roll)
            total_hours_conducted++;
    }
    fclose(fp);
}

/* -------------------- CRUD -------------------- */

void add_student() {
    if (student_count >= MAX_STUDENTS) {
        printf("Student limit reached!\n");
        return;
    }

    struct Student s;

    printf("\nEnter student name: ");
    read_line(s.name, sizeof(s.name));

    s.roll = student_count + 1;
    s.total_present_hours = 0;

    students[student_count++] = s;
    save_students();

    printf("Student added successfully. Roll = %d\n", s.roll);
}

void view_students() {
    if (student_count == 0) {
        printf("No students found.\n");
        return;
    }

    printf("\n---------------- STUDENT LIST ----------------\n");
    printf("+------+--------------------------+\n");
    printf("| Roll | Name                     |\n");
    printf("+------+--------------------------+\n");

    for (int i = 0; i < student_count; i++) {
        printf("| %-4d | %-24s |\n",
               students[i].roll,
               students[i].name);
    }

    printf("+------+--------------------------+\n");
}

void search_student() {
    int roll;
    printf("\nEnter roll number to search: ");
    scanf("%d", &roll);
    clear_input_buffer();

    for (int i = 0; i < student_count; i++) {
        if (students[i].roll == roll) {

            printf("\n----------- STUDENT DETAILS -----------\n");
            printf("+------+--------------------------+---------------+\n");
            printf("| Roll | Name                     | Present Hours |\n");
            printf("+------+--------------------------+---------------+\n");

            printf("| %-4d | %-24s | %-13d |\n",
                   students[i].roll,
                   students[i].name,
                   students[i].total_present_hours);

            printf("+------+--------------------------+---------------+\n");
            return;
        }
    }
    printf("Student not found.\n");
}

void update_student() {
    int roll;
    printf("\nEnter roll number to update: ");
    scanf("%d", &roll);
    clear_input_buffer();

    for (int i = 0; i < student_count; i++) {
        if (students[i].roll == roll) {
            printf("Enter new name: ");
            read_line(students[i].name, sizeof(students[i].name));
            save_students();
            printf("Student updated successfully.\n");
            return;
        }
    }
    printf("Student not found.\n");
}

void delete_student() {
    int roll;
    printf("\nEnter roll number to delete: ");
    scanf("%d", &roll);
    clear_input_buffer();

    for (int i = 0; i < student_count; i++) {
        if (students[i].roll == roll) {

            for (int j = i; j < student_count - 1; j++) {
                students[j] = students[j + 1];
            }

            student_count--;
            save_students();

            printf("Student deleted successfully.\n");
            return;
        }
    }
    printf("Student not found.\n");
}

/* -------------------- ATTENDANCE -------------------- */

char get_attendance_input(const char *prompt) {
    char buf[10];
    while (1) {
        printf("%s (P/A): ", prompt);
        read_line(buf, sizeof(buf));
        char ch = toupper(buf[0]);
        if (ch == 'P' || ch == 'A') return ch;
        printf("Enter only P or A.\n");
    }
}

void mark_attendance() {
    if (student_count == 0) {
        printf("No students available.\n");
        return;
    }

    char date[MAX_DATE_LEN];
    int hour;

    printf("Enter date (DD-MM-YYYY): ");
    read_line(date, sizeof(date));

    printf("Enter hour (1-8): ");
    scanf("%d", &hour);
    clear_input_buffer();

    FILE *fp = fopen(ATTENDANCE_FILE, "ab");

    for (int i = 0; i < student_count; i++) {
        struct AttendanceRecord rec;

        rec.roll = students[i].roll;
        strcpy(rec.date, date);
        rec.hour_num = hour;
        rec.status = get_attendance_input(students[i].name);

        if (rec.status == 'P')
            students[i].total_present_hours++;

        fwrite(&rec, sizeof(rec), 1, fp);
    }

    fclose(fp);
    calculate_total_conducted_hours();
    save_students();

    printf("Attendance marked successfully.\n");
}

/* -------------------- REPORT -------------------- */

void generate_summary_report() {
    if (total_hours_conducted == 0) {
        printf("No attendance data available.\n");
        return;
    }

    printf("\n---------------- ATTENDANCE REPORT ----------------\n");
    printf("+------+--------------------------+----------+-------------+\n");
    printf("| Roll | Name                     | Present  | Percentage  |\n");
    printf("+------+--------------------------+----------+-------------+\n");

    for (int i = 0; i < student_count; i++) {
        float percent = ((float)students[i].total_present_hours / total_hours_conducted) * 100;

        printf("| %-4d | %-24s | %-8d | %-10.2f |\n",
               students[i].roll,
               students[i].name,
               students[i].total_present_hours,
               percent);
    }

    printf("+------+--------------------------+----------+-------------+\n");
    printf("Total Hours Conducted: %d\n", total_hours_conducted);
}
