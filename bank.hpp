#ifndef BANK_HPP
#define BANK_HPP

#include <sqlite3.h>

enum AccountType {
    SAVINGS,
    CHECKING
};

class Account {
    private:
        long balance;
        AccountType account_type;
        unsigned long id;
        unsigned long generate_id();
        sqlite3* db;
    public:
        unsigned long get_id();
        Account();
        void set_balance(long amount);
        void set_account_type(AccountType type);
        long get_balance();
        void deposit(long amount);
        void withdraw(long amount);
        void transfer(Account* to, long amount);
        void close();
        void print_transactions();
        void print_balance();
        void print_id();
        void print_account_type();
        void print_all();

};

Account lookup_account(unsigned long id);

#endif // BANK_HPP