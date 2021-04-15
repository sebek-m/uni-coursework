#include <exception>
#include <csignal>
#include <memory>

#include "err.h"
#include "proxy-initializer.h"
#include "exceptions.h"
#include "proxy-class.h"
#include "utils.h"

std::unique_ptr<Proxy> proxy;

/* Obsługa sygnału kończenia */
static void catch_int (int sig) {
  if (sig == SIGINT) {
    if (proxy)
      (*proxy).stop();
  }
}

int main(int argc, char* argv[]) {
  struct sigaction action;
  sigset_t block_mask;
  int exit_code;

  exit_code = exit_code::SUCCESS;
  
  sigemptyset(&block_mask);
  action.sa_handler = catch_int;
  action.sa_mask = block_mask;
  action.sa_flags = 0;
  
  if (sigaction(SIGINT, &action, 0) == -1) {
    syserr("sigaction");
    return exit_code::ERROR;
  }

  try {
    ProxyInitializer initializer{argc, argv};

    proxy = std::unique_ptr<Proxy>(initializer.initialize_proxy());
    (*proxy).play();
  }
  catch(const SystemError& e) {
    syserr(e.what());
    exit_code = exit_code::ERROR;
  }
  catch (const std::exception& e) {
    fatal(e.what());
    exit_code = exit_code::ERROR;
  }

  return exit_code;
}