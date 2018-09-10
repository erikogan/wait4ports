/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <getopt.h>

#include "list.h"
#include "util.h"
#include "version.h"

#define SLEEP_ENV_VAR "WAIT4PORTS_SLEEP"
#define VERBOSE_ENV_VAR "WAIT4PORTS_VERBOSE"

static char *usage_name;
static unsigned short verbose_flag = 1;

unsigned short verbosity_from_env(char *);
void print_version(void);

int main(int argc, char **argv) {
  struct list_node *list;
  int sleep_from_option = 0, sleep_seconds = SLEEP_SECONDS;
  char c, *sleep_string = getenv(SLEEP_ENV_VAR);

  verbose_flag = verbosity_from_env(VERBOSE_ENV_VAR);

  while ((c = getopt (argc, argv, "vqs:")) != -1) {
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
      case '?':
        if (optopt == 's')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        usage();
      default:
        usage();
    }
  }

  if (sleep_string) {
    /* prevent atoi from setting the value to zero without a warning */
    if (!(*sleep_string >= '0' && *sleep_string <= '9')) {
      char *source = sleep_from_option ? "argument to -s" : "environment variable " SLEEP_ENV_VAR;
      fprintf (stderr, "Warning: %s [%s] is not a numeric value, sleep value set to 0\n", source, sleep_string);
      sleep_seconds = 0;
    } else {
      sleep_seconds = atoi(sleep_string);
    }
  }

  usage_name = argv[0];

  if (argc < 2) {
    usage();
  }

  list = build_list(argc - optind, argv + optind);

  if (!list) {
    fprintf(stderr, "%s\n", "No valid peers found in arguments!");
    usage();
  }

  process_list(list, sleep_seconds);
}

void usage() {
  fprintf(stderr, "Usage: %s [-q] [-s <seconds>] [<name>=]<protocol>://<host>:<port> [...]\n", usage_name);
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
      switch (*(value+1)) {
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
