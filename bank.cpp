#include <iostream>
#include <random>
#include <sstream>
#include "sql.hpp"
#include "bank.hpp"

using std::string;
using std::stringstream;

unsigned long Account::generate_id() {
    std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<unsigned long> dis(10000000, 99999999);

            return dis(gen);
}

unsigned long Account::get_id() {
    return this->id;
}

Account::Account() {
    unsigned long new_id = generate_id();

    // while ID is already in db, generate new ID
    while (true) {
        bool exists = false;
        stringstream ss;
        ss << "SELECT * FROM accounts WHERE id = " << new_id << ";";
        string sql(ss.str());
        printf("sql: %s\n", sql.c_str());

        if (sqlite3_open("accounts.db", &db) != SQLITE_OK) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            exit(0);
        } else {
            std::cout << "Opened database successfully" << std::endl;
        }

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
            int res = sqlite3_step(stmt);
            if (res == SQLITE_ROW) {
                printf("id: %s\n", sqlite3_column_text(stmt, 0));
                printf("balance: %s\n", sqlite3_column_text(stmt, 1));
                printf("account_type: %s\n", sqlite3_column_text(stmt, 2));
                res = sqlite3_step(stmt);
                exists = true;
            }
        }

        if (!exists) {
            break;
        }

        new_id = generate_id();
    }
    this->id = new_id;
    string sql_query = "INSERT INTO accounts (id, balance, account_type) VALUES (" + std::to_string(new_id) + ", 0.0, 'checking');";
    exec_sql("accounts.db", sql_query.c_str());
    // verify transaction
    bool exists = false;
    stringstream ss;
    ss << "SELECT * FROM accounts WHERE id = " << new_id << ";";
    string sql(ss.str());
    printf("sql: %s\n", sql.c_str());

    if (sqlite3_open("accounts.db", &db) != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        exit(0);
    } else {
        std::cout << "Opened database successfully" << std::endl;
    }

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        int res = sqlite3_step(stmt);
        if (res == SQLITE_ROW) {
            printf("id: %s\n", sqlite3_column_text(stmt, 0));
            printf("balance: %s\n", sqlite3_column_text(stmt, 1));
            printf("account_type: %s\n", sqlite3_column_text(stmt, 2));
            res = sqlite3_step(stmt);
            exists = true;
        }
    }

    if (!exists) {
        std::cerr << "Account not created" << std::endl;
        exit(0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    printf("Account created successfully\n");   
}

long Account::get_balance() {
    return this->balance;
}

void Account::set_balance(long amount) {
    this->balance = amount;
}

void Account::set_account_type(AccountType type) {
    this->account_type = type;
}

void Account::deposit(long amount) {
    this->balance += amount;
    string sql = "UPDATE accounts SET balance = " + std::to_string(this->balance) + " WHERE id = " + std::to_string(this->id) + ";";
    exec_sql("accounts.db", sql.c_str());
}

void Account::withdraw(long amount) {
    this->balance -= amount;
    string sql = "UPDATE accounts SET balance = " + std::to_string(this->balance) + " WHERE id = " + std::to_string(this->id) + ";";
    exec_sql("accounts.db", sql.c_str());
}

void Account::transfer(Account* to, long amount) {
    this->balance -= amount;
    to->balance += amount;
    string sql = "UPDATE accounts SET balance = " + std::to_string(this->balance) + " WHERE id = " + std::to_string(this->id) + ";";
    exec_sql("accounts.db", sql.c_str());
    sql = "UPDATE accounts SET balance = " + std::to_string(to->balance) + " WHERE id = " + std::to_string(to->id) + ";";
    exec_sql("accounts.db", sql.c_str());
}

void Account::close() {
    string sql = "DELETE FROM accounts WHERE id = " + std::to_string(this->id) + ";";
    exec_sql("accounts.db", sql.c_str());
}

void Account::print_transactions() {
    printf("Transactions for account %lu\n", this->id);
    printf("Balance: %lu\n", this->balance);
}


Account lookup_account(unsigned long id) {
    Account a;
    sqlite3* db;
    if (sqlite3_open("accounts.db", &db) != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        exit(0);
    } else {
        std::cout << "Opened database successfully" << std::endl;
    }

    string sql = "SELECT * FROM accounts WHERE id = " + std::to_string(id) + ";";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        int res = sqlite3_step(stmt);
        if (res == SQLITE_ROW) {
            printf("id: %s\n", sqlite3_column_text(stmt, 0));
            unsigned long id = std::stoul((char*)sqlite3_column_text(stmt, 0));
            printf("balance: %s\n", sqlite3_column_text(stmt, 1));
            long balance = std::stol((char*)sqlite3_column_text(stmt, 1));
            printf("account_type: %s\n", sqlite3_column_text(stmt, 2));
            string account_type = (char*)sqlite3_column_text(stmt, 2);
            if (account_type == "checking") {
                a.set_account_type(CHECKING);
            } else {
                a.set_account_type(SAVINGS);
            }
            res = sqlite3_step(stmt);
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return a;
}