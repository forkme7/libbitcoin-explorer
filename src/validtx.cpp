/*
 * Copyright (c) 2011-2014 sx developers (see AUTHORS)
 *
 * This file is part of sx.
 *
 * sx is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <bitcoin/bitcoin.hpp>
#include <obelisk/obelisk.hpp>
#include <sx/utility/config.hpp>
#include <sx/command/validtx.hpp>
#include <sx/utility/client.hpp>
#include <sx/utility/coin.hpp>
#include <sx/utility/console.hpp>

using namespace bc;

// TODO: this should be a member of sx::extensions::validtx,
// otherwise concurrent test execution will collide on shared state.
bool validtx_stopped = false;

// TODO: validtx_stopped should be passed here via closure
// or by converting this to a member function.
void tx_validated(const std::error_code& ec, const index_list& unconfirmed)
{
    std::cout << "Status: " << ec.message() << std::endl;
    for (size_t unconfirmed_index: unconfirmed)
        std::cout << "  Unconfirmed: " << unconfirmed_index << std::endl;

    validtx_stopped = true;
}

bool sx::extensions::validtx::invoke(const int argc, const char* argv[])
{
    if (!validate_argument_range(argc, example(), 1, 2))
        return false;

    std::string filename(get_filename(argc, argv));

    transaction_type tx;
    if (!load_satoshi_item<transaction_type>(tx, filename))
        return false;

    OBELISK_FULLNODE(pool, fullnode);
    fullnode.transaction_pool.validate(tx, tx_validated);
    poll(fullnode, pool, validtx_stopped);

    return true;
}
