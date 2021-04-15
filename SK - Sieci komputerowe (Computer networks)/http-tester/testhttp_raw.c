#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "err.h"
#include "parser.h"
#include "msg_builder.h"
#include "connector.h"
#include "response_parser.h"

arg_info info;
char *message, *response, *output;

static void cleanup() {
  clear_info(&info);

  if (message != NULL) {
    free(message);
    message = NULL;
  }

  if (response != NULL) {
    free(response);
    response = NULL;
  }

  if (output != NULL) {
    free(output);
    output = NULL;
  }
}

static void catch_int (int sig) {
  cleanup();
  fatal("Caught SIGINT");
}

int main(int argc, char* argv[]) {
  int err;
  size_t message_len, response_len;
  struct sigaction action;
  sigset_t block_mask;

  sigemptyset (&block_mask);
  action.sa_handler = catch_int;
  action.sa_mask = block_mask;
  action.sa_flags = 0;
  
  if (sigaction (SIGINT, &action, 0) == -1)  
    syserr("sigaction");

  initialize(&info);
  message = NULL;
  response = NULL;
  output = NULL;

  if ((err = read_cmd(&info, argc, argv)) != SUCCESS)
    resolve_error(err);

  if ((err = build_message(&info, &message, &message_len)) != SUCCESS) {
    cleanup();
    resolve_error(err);
  }

  if ((err = send_and_receive_msg(&info, message_len, message, &response_len, &response)) != SUCCESS) {
    cleanup();
    resolve_error(err);
  }

  if ((err = parse_response(response, &output)) != SUCCESS) {
    cleanup();
    resolve_error(err);
  }

  printf("%s\n", output);

  cleanup();

  return EXIT_SUCCESS;
}