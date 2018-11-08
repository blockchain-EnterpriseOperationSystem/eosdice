#include "eosdice.hpp"


void eosdice::reveal(const st_bet &bet)
{
    require_auth(_self);
    uint8_t random_roll = random(bet.player, bet.id);
    asset payout(0, EOS_SYMBOL);
    if (random_roll < bet.roll_under)
    {
        payout = compute_payout(bet.roll_under, bet.amount);

        action(permission_level{_self, "active"_n},
               "eosio.token"_n,
               "transfer"_n,
               make_tuple(_self, bet.player, payout, winner_memo(bet)))
            .send();
    }
    eostime start = 1541419200; //UTC8 2018-11-5 20:00:00
    eostime end = 1541505600;   //UTC8 2018-11-6 20:00:00
    if (now() > start and now() < end)
    {
        if (random_roll == 8 or random_roll == 18 or random_roll == 28 or random_roll == 38 or random_roll == 48 or random_roll == 58 or random_roll == 68 or random_roll == 78 or random_roll == 88 or random_roll == 98)
        {
            issue_token(bet.player, bet.amount * 3, "mining! eosdice.vip");
        }
        else
        {
            issue_token(bet.player, bet.amount, "mining! eosdice.vip");
        }
    }
    else
    {
        issue_token(bet.player, bet.amount, "mining! eosdice.vip");
    }
    unlock(bet.amount);

    st_result result{.bet_id = bet.id,
                     .player = bet.player,
                     .referrer = bet.referrer,
                     .amount = bet.amount,
                     .roll_under = bet.roll_under,
                     .random_roll = random_roll,
                     .payout = payout};

    action(permission_level{_self, "active"_n},
           LOG,
           "result"_n,
           result)
        .send();

    action(permission_level{_self, "active"_n},
           "eosio.token"_n,
           "transfer"_n,
           std::make_tuple(_self, DEV, compute_dev_reward(bet), std::string("for dev")))
        .send();

    action(permission_level{_self, "active"_n},
           "eosio.token"_n,
           "transfer"_n,
           std::make_tuple(_self, PRIZEPOOL, compute_pool_reward(bet), std::string("for prize pool")))
        .send();
}
//void eosdice::reveal1(const st_bet &bet)
//{
//    require_auth(_self);
//    send_defer_action(permission_level{_self, "active"_n},
//                      _self,
//                      "reveal"_n,
//                      bet);
//}

void eosdice::transfer(const name &from,
                            const name &to,
                            const asset &quantity,
                            const string &memo)
{
    if (from == _self || to != _self)
    {
        return;
    }
    if (memo.substr(0, 4) != "dice")
    {
        return;
    }
    uint8_t roll_under;
    name referrer;
    parse_memo(memo, &roll_under, reinterpret_cast<char *>(&referrer));
    eosio_assert(is_account(referrer), "referrer account does not exist");

    // //check quantity
    assert_quantity(quantity);

    // //check roll_under
    assert_roll_under(roll_under, quantity);

    // //check referrer
    eosio_assert(referrer != from, "referrer can not be self");

    //count player
    // iplay(from, quantity);

    //vip check
    // vipcheck(from, quantity);

    const st_bet _bet{.id = next_id(),
                      .player = from,
                      .referrer = referrer,
                      .amount = quantity,
                      .roll_under = roll_under,
                      .created_at = now()};

    lock(quantity);

    action(permission_level{_self, "active"_n},
           "eosio.token"_n,
           "transfer"_n,
           make_tuple(_self,
                      _bet.referrer,
                      compute_referrer_reward(_bet),
                      referrer_memo(_bet)))
        .send();
    send_defer_action(permission_level{_self, "active"_n},
                      _self,
                      "reveal1"_n,
                      _bet);
}

void eosdice::init()
{
    require_auth(_self);
    st_global global = _global.get_or_default(
        st_global{.current_id = _bets.available_primary_key()});

    eosio_assert(global.initStatu != 1, "init ok");

    global.current_id += 1;
    global.nexthalve = 7524000000 * 1e4;
    global.eosperdice = 100;
    global.initStatu = 1;
    _global.set(global, _self);
}

extern "C"
{
void apply(uint64_t receiver, uint64_t code, uint64_t action)
{
    if (code == receiver)
    {
        switch (action)
        {
            EOSIO_DISPATCH_HELPER(eosdice, (reveal)(init))
        }
    }

     if ((code == "eosio.token"_n.value) && (action == "transfer"_n.value))
    {
        execute_action(name(receiver), name(code),&eosdice::transfer);
        return;
    }

    eosio_exit(0);
}
};