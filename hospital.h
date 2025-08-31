#ifndef HOSPITAL_H_
#define HOSPITAL_H_


typedef enum {
    cardiology,
    ortho,
    neurology,
    general
} Department;

typedef struct {
    int id;
    char name[50];
    int age;
    char disease[50];
    char admissionDate[15];
    int roomNumber;
    Department dept;
} Patient;

void add_patient();
void display_patients();
void search_patient();
void updatePatient();



#endif
