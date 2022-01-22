/*
 ***************************************************************************** 
 * Author: Yogender Solanki <yogendersolanki91@gmail.com> 
 *
 * Copyright (c) 2022 Yogender Solanki
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************
 */
#pragma once

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <queue>

template <typename Data>
class blocking_queue {
private:
    std::queue<Data> queue;
    mutable boost::mutex queue_mutex;
    const size_t queue_limit;

    bool is_closed = false;

    boost::condition_variable new_item_or_closed_event;
    boost::condition_variable item_removed_event;

#ifndef NDEBUG
    size_t pushes_in_progress = 0;
#endif

public:
    blocking_queue(size_t size_limit = 0)
        : queue_limit(size_limit)
    {
    }

    void push(const Data data)
    {
        boost::mutex::scoped_lock lock(queue_mutex);
#ifndef NDEBUG
        ++pushes_in_progress;
#endif
        if (queue_limit > 0) {
            while (queue.size() >= queue_limit) {
                item_removed_event.wait(lock);
            }
        }
        // assert(!is_closed);
        queue.push(data);
#ifndef NDEBUG
        --pushes_in_progress;
#endif

        new_item_or_closed_event.notify_one();
    }

    bool try_push(const Data data)
    {
        boost::mutex::scoped_lock lock(queue_mutex);
        if (queue_limit > 0) {
            if (queue.size() >= queue_limit) {
                return false;
            }
        }
        //assert(!is_closed);
        queue.push(data);

        new_item_or_closed_event.notify_one();
        return true;
    }

    void close()
    {
        boost::mutex::scoped_lock lock(queue_mutex);
        //  assert(!is_closed);
#ifndef NDEBUG
        // assert(pushes_in_progress == 0);
#endif
        is_closed = true;

        new_item_or_closed_event.notify_all();
    }

    bool pop(Data& popped_value)
    {
        boost::mutex::scoped_lock lock(queue_mutex);
        while (queue.empty()) {
            if (is_closed) {
                return false;
            }
            new_item_or_closed_event.wait(lock);
        }

        popped_value = queue.front();
        queue.pop();
        item_removed_event.notify_one();
        return true;
    }

    bool try_pop(Data& popped_value)
    {
        boost::mutex::scoped_lock lock(queue_mutex);
        if (queue.empty()) {
            return false;
        }

        popped_value = queue.front();
        queue.pop();
        item_removed_event.notify_one();
        return true;
    }

    bool empty() const
    {
        boost::mutex::scoped_lock lock(queue_mutex);
        return queue.empty();
    }

    bool closed() const
    {
        boost::mutex::scoped_lock lock(queue_mutex);
        return is_closed;
    }

    size_t limit() const
    {
        return queue_limit;
    }

    size_t size() const
    {
        boost::mutex::scoped_lock lock(queue_mutex);
        return queue.size();
    }
};
