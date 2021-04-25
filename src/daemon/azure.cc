#include <libazure/logging.h>
#include <libazure/daemon.h>

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  AZLogW("This is a development executable. Please do not use");

  auto& daemon = azure::Daemon::Instance();
  daemon.Run();

  return 0;
}
