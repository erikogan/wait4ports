/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "usage.h"

static char *usage_name;

int main(int argc, char **argv) {
  struct list_node *list;

  usage_name = argv[0];

  if (argc < 2) {
    usage();
  }

  list = build_list(argc, argv);
  process_list(list);
}

void usage() {
  fprintf(stderr, "Usage: %s [<name>=]<protocol>://<host>:<port> [...]\n", usage_name);
  exit(255);
}
