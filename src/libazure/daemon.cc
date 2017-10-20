/**
 ******************************************************************************
 * Azure : Open Source iOS Memory Editor                                      *
 * File : daemon.cc                                                            *
 ******************************************************************************
 * Copyright 2017 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include <gflags/gflags.h>
#include "logging.h"
#include "daemon.h"

DEFINE_string(ip, "127.0.0.1", "IP address for TCP server to connect to");
DEFINE_int32(port, 1248, "Port for TCP server to connect to");

namespace azure {

Daemon::Daemon(int argc, char **argv)
        : argc_(argc), argv_(argv), server_(FLAGS_ip, FLAGS_port) {

}

Daemon::Daemon(int argc, char **argv, std::string ip, short port)
        : argc_(argc), argv_(argv), server_(ip, port) {

}


Daemon::~Daemon() {

}

int Daemon::Run() {
    if (!server_.Setup()) {
        AZLogW("Could not set up TCP server");
        return 255;
    }
    return server_.Run();
}

}