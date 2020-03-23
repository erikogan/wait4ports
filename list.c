/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "list.h"
#include "util.h"

struct list_node *build_node(char *);
void extract_parts(char *, char **, char **, char **, char **);
struct addrinfo *configure_hints(struct addrinfo *);
void normalize_name(struct list_node *, char *, char *, char *);
int connect_to_address(struct addrinfo *, char *);
void remove_node(struct list_node **, struct list_node **, struct list_node **);
void verbose_connection(struct addrinfo *, char *);

struct addrinfo *configure_hints(struct addrinfo *hints) {
  memset(hints, 0, sizeof(*hints));
  hints->ai_family = PF_UNSPEC;
  hints->ai_socktype = SOCK_STREAM;
  hints->ai_protocol = IPPROTO_TCP;
  hints->ai_flags |= AI_CANONNAME | AI_ADDRCONFIG;
  return hints;
}

struct list_node *build_list(int argc, char **argv) {
  int i;
  struct list_node *head = NULL, *tail = NULL, *candidate;

  /* the program (and options) have been shaved off the front, start at 0 */
  for (i = 0 ; i < argc ; i++) {
    candidate = build_node(argv[i]);
    if (candidate) {
      if (!head) {
        head = tail = candidate;
      } else {
        tail->next = candidate;
        tail = tail->next;
      }
    }
  }

  if (tail) {
    tail->next = NULL;
  }

  return head;
}

struct list_node *build_node(char *composite) {
  char *name = NULL, *proto, *host, *port;
  struct addrinfo hints, *res;
  struct list_node *result;
  int err;

  extract_parts(composite, &name, &proto, &host, &port);

  configure_hints(&hints);

  err = getaddrinfo(host, port, &hints, &res);
  if (err != 0) {
    fprintf(stderr, "%s:%s ", host, port);
    perror("getaddrinfo");
    return NULL;
  }

  result = (struct list_node *) malloc(sizeof(struct list_node));
  memset(result, 0, sizeof(struct list_node));

  result->addresses = res;

  normalize_name(result, name, host, port);

  return result;
}

void process_list(struct list_node *list, int sleep_seconds, int timeout_seconds) {
  struct list_node *prev, *cur;
  struct addrinfo *address;
  int connected = -1;
  time_t cur_time, start_time = time(NULL);

  while (list) {
    cur = list;
    while(cur) {
      address = cur->addresses;
      while (address) {
        connected = connect_to_address(address, cur->name);
        if (connected < 0) {
          /* failed, continue trying */
          address = address->ai_next;
        } else {
          /* success, remove it and clean up */
          remove_node(&list, &cur, &prev);
          break;
        }
      }

      if (connected < 0) {
        prev = cur;
        cur = cur->next;
      }
    }

    if (list) {
      if (timeout_seconds) {
        cur_time = time(NULL);
        if (start_time + timeout_seconds <= cur_time) {
          fprintf(stderr, "Timeeout [%d] reached. Exiting.\n", timeout_seconds);
          exit(255);
        }

        int remaining = start_time + timeout_seconds - cur_time;
        if (sleep_seconds > remaining) sleep_seconds = remaining;
      }

      sleep(sleep_seconds);
    }
  }
}

void remove_node(struct list_node **list_p, struct list_node **current_p, struct list_node **previous_p) {
  struct list_node *to_free = *current_p;

  if (*current_p == *list_p) {
    *current_p = *list_p = (*list_p)->next;
  } else {
    *current_p = (*previous_p)->next = (*current_p)->next;
  }

  freeaddrinfo(to_free->addresses);
  free(to_free);
}

void extract_parts(char *composite, char **name_p, char **proto_p, char **host_p, char **port_p) {
  char *tok, *pristine;
  int pristine_len = strnlen(composite, 255);

  pristine = calloc(pristine_len + 1, sizeof(char));
  strncpy(pristine, composite, pristine_len);

  if ((tok = strchr(composite, '='))) {
    *tok = '\0';
    *name_p = composite;
    *proto_p = tok + 1;
  } else {
    *proto_p = composite;
  }

  if (strncmp("tcp", *proto_p, 3)) {
    fprintf(stderr, "Unsupported protocol (currently only tcp): %s\n", pristine);
    usage();
  }

  if ((tok = strstr(*proto_p, "://"))) {
    *tok = '\0';
    *host_p = tok + 3;
  } else {
    fprintf(stderr, "Invailid argument (missing ://): %s\n", pristine);
    usage();
  }

  if ((tok = strchr(*host_p, ':'))) {
    *tok = '\0';
    *port_p = tok + 1;
  } else {
    fprintf(stderr, "Invailid argument (missing port): %s\n", pristine);
    usage();
  }

  free(pristine);
}

void normalize_name(struct list_node *node, char *name, char *host, char *port) {
  size_t hlen, plen;
  char *normalized_host = node->addresses->ai_canonname;

  if (name) {
    node->name = name;
    return;
  }

  if(!normalized_host) {
    normalized_host = host;
  }

  hlen = strnlen(normalized_host, 255);
  plen = strnlen(port, 255);

  node->name = calloc(hlen + plen + 2, sizeof(char));
  strncpy(node->name, normalized_host, hlen);
  node->name[hlen] = ':';
  strncpy(node->name + hlen + 1, port, plen);
  node->name[hlen + plen + 1] = '\0';
  node->allocated_name = 1;
}

int connect_to_address(struct addrinfo *address, char *name) {
  int s, result;
  unsigned short vflag = verbose();

  if ((s = socket(address->ai_family, address->ai_socktype, address->ai_protocol)) < 0) {
    perror("socket");
    exit(127);
  }

  if (vflag) verbose_connection(address, name);

  if ((result = connect(s, address->ai_addr, address->ai_addrlen)) < 0) {
    if (vflag) printf("failed.\n");
  } else {
    if (vflag) {
      printf("SUCCESS!\n");
    } else {
      printf("PORT READY: %s\n", name);
    }

    close(s);
  }

  fflush(stdout);

  return result;
}

void verbose_connection(struct addrinfo *address, char *name) {
  u_int16_t port;
  char *ip = NULL;

  if (address->ai_family == AF_INET6) {
    port = ntohs(((struct sockaddr_in6 *) address->ai_addr)->sin6_port);
    ip = calloc(INET6_ADDRSTRLEN + 1, sizeof(char));

    inet_ntop(AF_INET6, &((struct sockaddr_in6 *) address->ai_addr)->sin6_addr, ip, INET6_ADDRSTRLEN);
  } else if (address->ai_family == AF_INET) {
    port = ntohs(((struct sockaddr_in *) address->ai_addr)->sin_port);
    ip = calloc(INET_ADDRSTRLEN + 1, sizeof(char));
    inet_ntop(AF_INET, &((struct sockaddr_in *) address->ai_addr)->sin_addr, ip, INET_ADDRSTRLEN);
  } else {
    port = -1;
    ip = calloc(8, sizeof(char));
    strncpy(ip, "Unknown", 8);
  }

  printf("Trying %s (%s:%d) ... ", name, ip, port);
  free(ip);
}
