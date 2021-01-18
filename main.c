/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "list.h"
#include "util.h"
#include "version.h"

#define SLEEP_SECONDS 1
#define SLEEP_ENV_VAR "WAIT4PORTS_SLEEP"

#define TIMEOUT_SECONDS 0
#define TIMEOUT_ENV_VAR "WAIT4PORTS_TIMEOUT"

#define VERBOSE_ENV_VAR "WAIT4PORTS_VERBOSE"

static char *usage_name;
static unsigned short verbose_flag = 1;

unsigned short verbosity_from_env(char *);
void print_version(void);
int opt_atoi(char *str, char opt, char *env, int from_option, int default_value);

int main(int argc, char **argv) {
  struct list_node *list;

  int sleep_from_option = 0;
  int timeout_from_option = 0;
  int sleep_seconds = SLEEP_SECONDS;
  int timeout_seconds = TIMEOUT_SECONDS;

  char c;
  char *sleep_string = getenv(SLEEP_ENV_VAR);
  char *timeout_string = getenv(TIMEOUT_ENV_VAR);

  verbose_flag = verbosity_from_env(VERBOSE_ENV_VAR);

  while ((c = getopt(argc, argv, "vqs:t:")) != -1) {
    switch(c) {
      case 'v':
        print_version();
        exit(0);
      case 'q':
        verbose_flag = 0;
        break;
      case 's':
        sleep_string = optarg;
        sleep_from_option = 1;
        break;
      case 't':
        timeout_string = optarg;
        timeout_from_option = 1;
        break;
      case '?':
        if (optopt == 's')
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint(optopt))
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        usage();
      default:
        usage();
    }
  }

  sleep_seconds = opt_atoi(sleep_string, 's', SLEEP_ENV_VAR, sleep_from_option, SLEEP_SECONDS);
  timeout_seconds = opt_atoi(timeout_string, 't', TIMEOUT_ENV_VAR, timeout_from_option, TIMEOUT_SECONDS);

  usage_name = argv[0];

  argc -= optind;
  argv += optind;

  if (argc < 1) {
    usage();
  }

  list = build_list(argc, argv);

  if (!list) {
    fprintf(stderr, "%s\n", "No valid peers found in arguments!");
    usage();
  }

  process_list(list, sleep_seconds, timeout_seconds);
}

int opt_atoi(char *str, char opt, char *env, int from_option, int default_value) {
  char *source;

  if (!str) return default_value;

  /* prevent atoi from setting the value to zero without a warning */
  if (!(*str >= '0' && *str <= '9')) {
    if (from_option) {
      source = calloc(14, sizeof(char));
      sprintf(source, "argument to -%c", opt);
    } else {
      source = calloc(21 + strlen(env), sizeof(char));
      sprintf(source, "environment variable %s", env);
    }

    fprintf(stderr, "Warning: %s [%s] is not a numeric value, set to 0\n", source, str);
    free(source);
    return 0;
  }

  return atoi(str);
}

void usage() {
  fprintf(stderr, "Usage: %s [-q] [-s <interval_seconds>] [-t <timeout_seconds>] [<name>=]<protocol>://<host>:<port> [...]\n", usage_name);
  exit(255);
}

void print_version() {
  printf("%s version %s\n", WAIT4PORTS_NAME, WAIT4PORTS_VERSION);
}

unsigned short verbose() {
  return verbose_flag;
}

unsigned short verbosity_from_env(char *name) {
  char *value = getenv(name);
  if (!value) return 1;

  switch(*value) {
    case '0':
    case 'n':
    case 'N':
    case 'f':
    case 'F':
      return(0);
      break;
    case 'o':
    case 'O':
      switch(*(value+1)) {
        case 'f':
        case 'F':
          return(0);
          break;
        default:
          return(1);
      }
      break;
    default:
      return 1;
  }
}
