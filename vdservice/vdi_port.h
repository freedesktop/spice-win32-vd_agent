/*
   Copyright (C) 2009 Red Hat, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _H_VDI_PORT
#define _H_VDI_PORT

#include <windows.h>
#include <stdint.h>

#define BUF_SIZE    (1024 * 1024)

#define BUF_READ    (1 << 0)
#define BUF_WRITE   (1 << 1)
#define BUF_ALL     (BUF_READ | BUF_WRITE)

#define VDI_PORT_BLOCKED    0
#define VDI_PORT_RESET      -1
#define VDI_PORT_ERROR      -2

typedef struct VDIPortBuffer {
    OVERLAPPED overlap;
    uint8_t* start;
    uint8_t* end;
    bool pending;
    int bytes;
    uint8_t ring[BUF_SIZE];
} VDIPortBuffer;

class VDIPort {
public:
    VDIPort();
    ~VDIPort();
    bool init();
    size_t ring_write(const void* buf, size_t size);
    size_t write_ring_free_space();
    size_t ring_read(void* buf, size_t size);
    size_t read_ring_size();
    size_t read_ring_continuous_remaining_size();
    HANDLE get_write_event() { return _write.overlap.hEvent; }
    HANDLE get_read_event() { return _read.overlap.hEvent; }
    int write();
    int read();
    void write_completion();
    void read_completion();

private:
    int handle_error();

private:
    static VDIPort* _singleton;
    HANDLE _handle;
    VDIPortBuffer _write;
    VDIPortBuffer _read;
};

// Ring notes:
// _end is one after the end of data
// _start==_end means empty ring
// _start-1==_end (modulo) means full ring
// _start-1 is never used
// ring_write & read on right side of the ring (update _end)
// ring_read & write from left (update _start)

#endif
