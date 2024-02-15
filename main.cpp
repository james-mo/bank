#include "bank.hpp"
#include "util.hpp"
#include <fstream>
#include "sql.hpp"

int main() {
    // check if accounts db exists
    if (!fileExists("accounts.db")) {
        std::ofstream file("accounts.db");
        file.close();

        exec_sql("accounts.db", "CREATE TABLE accounts (id INTEGER PRIMARY KEY, balance REAL NOT NULL DEFAULT 0.0, account_type TEXT);");
    }

    Account a;
    printf("Account created with id: %lu\n", a.get_id());

    return 0;
}