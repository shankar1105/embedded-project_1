
#include <stdio.h>
#include "hospital.h"

int main() {
    int choice;
    do {
        printf("\n Hospital Management Menu\n");

        printf("1. AddPatient\n 2. DisplayAll\n 3. SearchPatient\n 4. UpdatePatient\n  5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: add_patient();
            break;
            case 2: display_patients();
            break;
            case 3: search_patient();
            break;
            case 4: updatePatient();
             break;
            case 5: printf("Exiting...\n");
            break;
            default: printf("Invalid choice.\n");
        }
    } while (choice = 5);

    return 0;
}

