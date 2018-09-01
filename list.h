/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <sys/types.h>
#include <netdb.h>

/* This should probably be overridable via an env var */
#define SLEEP_SECONDS 1

struct list_node {
  char     *name;
  uint      allocated_name;

  struct addrinfo  *addresses;
  struct list_node *next;
};

struct list_node *build_list(int, char **);
void process_list(struct list_node *);
