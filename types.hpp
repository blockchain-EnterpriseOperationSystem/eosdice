#include "utils.hpp"

#define EOS_SYMBOL symbol(symbol_code("EOS"), 4)
#define DICE_SYMBOL symbol(symbol_code("BOCAI"), 4)
#define LOG name("eosbocailogs")
//token
#define DICETOKEN name("eosbocai1111")
#define DEV name("eosbocaidevv")
#define PRIZEPOOL name("eosbocai1111")
#define DICESUPPLY 88000000000000

typedef uint32_t eostime;



TABLE st_bet
{
    uint64_t id;
    name player;
    name referrer;
    asset amount;
    uint8_t roll_under;
    uint64_t created_at;
    uint64_t primary_key() const { return id; }
};


TABLE st_user
{
    name owner;
    asset amount;
    uint32_t count;
    uint64_t primary_key() const { return (uint64_t) owner; }
};

struct st_result
{
    uint64_t bet_id;
    name player;
    name referrer;
    asset amount;
    uint8_t roll_under;
    uint8_t random_roll;
    asset payout;
};

// @abi table fundpool i64
struct st_fund_pool
{
    asset locked;
};

// @abi table global i64
struct st_global
{
    uint64_t current_id;
    double eosperdice;
    uint64_t nexthalve;
    uint64_t initStatu;
};

typedef multi_index<"users"_n, st_user> tb_uesrs;
typedef multi_index<"bets"_n, st_bet> tb_bets;
typedef singleton<"fundpool"_n, st_fund_pool> tb_fund_pool;
typedef singleton<"global"_n, st_global> tb_global;

