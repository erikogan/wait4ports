/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* This should probably be overridable via an env var */
#define SLEEP_SECONDS 1

static char *usage_name;

struct list_node {
  char     *name;
  uint      allocated_name;
  struct sockaddr_in addr;
  struct list_node *next;
};

void usage();
struct list_node *build_list(int, char **);
struct list_node *build_node(char *);
void process_list(struct list_node *);

int main(int argc, char **argv) {
  struct list_node *list;

  usage_name = argv[0];

  if (argc < 2) {
    usage();
  }

  list = build_list(argc, argv);
  process_list(list);
}

void process_list(struct list_node *list) {
  struct list_node *prev, *cur;
  int s;

  while (list) {
    cur = list;
    while(cur) {
      if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(127);
      }
      if (connect(s, (const struct sockaddr *) &cur->addr, sizeof(cur->addr)) < 0) {
        printf("Waiting for %s ...\n", cur->name);
        fflush(stdout);
        prev = cur;
        cur = cur->next;
      } else {
        /* Success */
        printf("PORT READY: %s\n", cur->name);
        fflush(stdout);
        if (cur == list) {
          prev = cur;
          cur = list = list->next;
          free(prev);
        } else {
          prev->next = cur->next;
          free(cur);
          cur = prev->next;
        }
      }
      close(s);
    }
    if (list) sleep(SLEEP_SECONDS);
  }
}

struct list_node *build_list(int argc, char **argv) {
  int i;
  struct list_node *head, *tail;

  head = tail = build_node(argv[1]);

  for (i = 2 ; i < argc ; i++) {
    tail->next = build_node(argv[i]);
    tail = tail->next;
  }

  tail->next = NULL;
  return head;
}

struct list_node *build_node(char *composite) {
  char *name = NULL, *proto, *addr, *port, *tok;
  uint allocated_name = 0;
  struct list_node *result;

  if ((tok = strchr(composite, '='))) {
    *tok = '\0';
    name = composite;
    proto = tok + 1;
  } else {
    proto = composite;
  }

  if (strncmp("tcp", proto, 3)) {
    fprintf(stderr, "Unsupported protocol (currently only tcp): %s\n", proto);
    usage();
  }

  if ((tok = strstr(proto, "://"))) {
    *tok = '\0';
    addr = tok + 3;
  } else {
    fprintf(stderr, "Invailid argument: %s\n", proto);
    usage();
  }

  if ((tok = strchr(addr, ':'))) {
    *tok = '\0';
    port = tok + 1;
  } else {
    fprintf(stderr, "Invailid argument: %s\n", proto);
    usage();
  }

  if (!name) {
    /* bad C programmer. No biscuit! */
    int alen = strlen(addr),
        plen = strlen(port);

    name = calloc(alen + plen + 2, sizeof(char));
    strncpy(name, addr, alen);
    name[alen] = ':';
    strncpy(name + alen + 1, port, plen);
    name[alen + plen + 1] = '\0';
    allocated_name = 1;
  }

  result = (struct list_node *) malloc(sizeof(struct list_node));
  result->name = name;
  result->allocated_name = allocated_name;
  result->addr.sin_family = AF_INET;
  result->addr.sin_port = htons(strtol(port, NULL, 10));
  /* ERANGE should be checked here. Zero, too. */
  inet_aton(addr, &result->addr.sin_addr);

  memset(&result->addr.sin_zero, 0, sizeof(result->addr.sin_zero));

  return result;

}

void usage() {
  fprintf(stderr, "Usage: %s [<name>=]<protocol>://<host>:<port> [...]\n", usage_name);
  exit(255);
}
