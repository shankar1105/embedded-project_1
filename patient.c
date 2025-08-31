// hospital.c
#include <stdio.h>
#include <stdlib.h>
#include "hospital.h"
#define FILE_NAME "patients.dat"

void add_patient() {

   Patient p;
 FILE *fp = fopen(FILE_NAME, "ab");
    printf("Enter ID: ");
     scanf("%d", &p.id);
    printf("Enter Name: ");
    scanf(" %s", p.name);
    printf("Enter Age: ");
    scanf("%d", &p.age);
    printf("Enter Disease: ");
    scanf(" %s", p.disease);
    printf("Enter Admission Date : ");
    scanf("%s", p.admissionDate);
    printf("Enter Room Number: ");
    scanf("%d", &p.roomNumber);
    printf("Enter Department: ");
    scanf("%d", (int*)&p.dept);
    printf(" Patient added successfully.\n");
}


void display_patients() {
    Patient p;

 FILE *fp = fopen(FILE_NAME, "rb");

    printf("\n display_patients :\n");

    while (fread(&p, sizeof(Patient), 1, fp)) {
        printf("ID: %d Name: %s  Age: %d  Disease: %s Date: %s Room: %d  Dept: %d\n",p.id, p.name, p.age, p.disease, p.admissionDate, p.roomNumber, p.dept);
    }

}


void search_patient(){
int searchID;
 Patient p;

FILE *fp = fopen(FILE_NAME, "rb");

printf("patient found :");
scanf("%d",&searchID);
if(p.id == searchID){
   printf("ID: %d Name: %s  Age: %d  Disease: %s Date: %s Room: %d  Dept: %d\n",p.id, p.name, p.age, p.disease, p.admissionDate, p.roomNumber, p.dept);
}
int found=1;
if(found){
    printf("patient not found");
}
else{
    printf("patient found");
}
}
void updatePatient() {
    int updateId;
    Patient p;
    FILE *fp = fopen(FILE_NAME, "rb");

    printf(" Enter Patient ID to update: ");
    scanf("%d", &updateId);

    while (fread(&p, sizeof(Patient), 1, fp)) {
          if (p.id == updateId) {
            printf("Enter new Name: ");
            scanf(" %s", p.name);
        }
    }
}
