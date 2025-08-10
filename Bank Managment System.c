#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "bankdata.dat"

typedef struct {
    int acc_no;
    char name[50];
    char acc_type[10];
    float balance;
} Account;

void createAccount();
void displayAccount(int);
void depositWithdraw(int, int);
void deleteAccount(int);
void displayAll();
int accountExists(int);

int main() {
    int choice, acc_no;

    while(1) {
        printf("\n\n===== Bank Management System =====\n");
        printf("1. Create New Account\n");
        printf("2. Display Account Details\n");
        printf("3. Deposit Money\n");
        printf("4. Withdraw Money\n");
        printf("5. Close Account\n");
        printf("6. Display All Accounts\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                createAccount();
                break;
            case 2:
                printf("Enter Account Number: ");
                scanf("%d", &acc_no);
                displayAccount(acc_no);
                break;
            case 3:
                printf("Enter Account Number: ");
                scanf("%d", &acc_no);
                depositWithdraw(acc_no, 1);  // 1 means deposit
                break;
            case 4:
                printf("Enter Account Number: ");
                scanf("%d", &acc_no);
                depositWithdraw(acc_no, 0);  // 0 means withdraw
                break;
            case 5:
                printf("Enter Account Number to close: ");
                scanf("%d", &acc_no);
                deleteAccount(acc_no);
                break;
            case 6:
                displayAll();
                break;
            case 7:
                printf("Exiting program...\n");
                exit(0);
            default:
                printf("Invalid choice! Try again.\n");
        }
    }

    return 0;
}

int accountExists(int acc_no) {
    FILE *fp = fopen(FILENAME, "rb");
    if(fp == NULL) return 0;
    Account acc;
    while(fread(&acc, sizeof(Account), 1, fp) == 1) {
        if(acc.acc_no == acc_no) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void createAccount() {
    Account acc;
    FILE *fp = fopen(FILENAME, "ab+");
    if(fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("Enter Account Number: ");
    scanf("%d", &acc.acc_no);

    if(accountExists(acc.acc_no)) {
        printf("Account number already exists! Try another.\n");
        fclose(fp);
        return;
    }

    printf("Enter Account Holder Name: ");
    fflush(stdin);
    fgets(acc.name, sizeof(acc.name), stdin);
    acc.name[strcspn(acc.name, "\n")] = 0;  // Remove trailing newline

    printf("Enter Account Type (Savings/Current): ");
    fgets(acc.acc_type, sizeof(acc.acc_type), stdin);
    acc.acc_type[strcspn(acc.acc_type, "\n")] = 0;

    printf("Enter Initial Deposit Amount: ");
    scanf("%f", &acc.balance);

    fwrite(&acc, sizeof(Account), 1, fp);
    fclose(fp);

    printf("Account created successfully!\n");
}

void displayAccount(int acc_no) {
    FILE *fp = fopen(FILENAME, "rb");
    if(fp == NULL) {
        printf("No accounts found!\n");
        return;
    }

    Account acc;
    int found = 0;
    while(fread(&acc, sizeof(Account), 1, fp) == 1) {
        if(acc.acc_no == acc_no) {
            printf("\nAccount Number: %d\n", acc.acc_no);
            printf("Account Holder: %s\n", acc.name);
            printf("Account Type: %s\n", acc.acc_type);
            printf("Balance: %.2f\n", acc.balance);
            found = 1;
            break;
        }
    }
    fclose(fp);

    if(!found) {
        printf("Account not found!\n");
    }
}

void depositWithdraw(int acc_no, int option) {
    FILE *fp = fopen(FILENAME, "rb+");
    if(fp == NULL) {
        printf("No accounts found!\n");
        return;
    }

    Account acc;
    int found = 0;
    while(fread(&acc, sizeof(Account), 1, fp) == 1) {
        if(acc.acc_no == acc_no) {
            found = 1;
            break;
        }
    }

    if(!found) {
        printf("Account not found!\n");
        fclose(fp);
        return;
    }

    float amount;
    if(option == 1) {
        printf("Enter amount to deposit: ");
        scanf("%f", &amount);
        if(amount <= 0) {
            printf("Invalid amount!\n");
            fclose(fp);
            return;
        }
        acc.balance += amount;
    } else {
        printf("Enter amount to withdraw: ");
        scanf("%f", &amount);
        if(amount <= 0) {
            printf("Invalid amount!\n");
            fclose(fp);
            return;
        }
        if(amount > acc.balance) {
            printf("Insufficient balance!\n");
            fclose(fp);
            return;
        }
        acc.balance -= amount;
    }

    // Move file pointer back to update record
    fseek(fp, -sizeof(Account), SEEK_CUR);
    fwrite(&acc, sizeof(Account), 1, fp);
    fclose(fp);

    printf("Transaction successful! Current Balance: %.2f\n", acc.balance);
}

void deleteAccount(int acc_no) {
    FILE *fp = fopen(FILENAME, "rb");
    if(fp == NULL) {
        printf("No accounts found!\n");
        return;
    }

    FILE *temp = fopen("temp.dat", "wb");
    Account acc;
    int found = 0;

    while(fread(&acc, sizeof(Account), 1, fp) == 1) {
        if(acc.acc_no == acc_no) {
            found = 1;
            continue;  // skip this account (delete)
        }
        fwrite(&acc, sizeof(Account), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    if(found) {
        remove(FILENAME);
        rename("temp.dat", FILENAME);
        printf("Account deleted successfully.\n");
    } else {
        printf("Account not found.\n");
        remove("temp.dat");
    }
}

void displayAll() {
    FILE *fp = fopen(FILENAME, "rb");
    if(fp == NULL) {
        printf("No accounts found!\n");
        return;
    }

    Account acc;
    printf("\nAll Accounts:\n");
    printf("Acc No.\tName\t\tType\tBalance\n");
    printf("------------------------------------------------\n");

    while(fread(&acc, sizeof(Account), 1, fp) == 1) {
        printf("%d\t%-15s %-10s %.2f\n", acc.acc_no, acc.name, acc.acc_type, acc.balance);
    }

    fclose(fp);


}
