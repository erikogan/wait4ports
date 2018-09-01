/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "list.h"
#include "usage.h"

struct list_node *build_node(char *);
void extract_parts(char *, char **, char **, char **, char **);
struct addrinfo *configure_hints(struct addrinfo *);
void normalize_name(struct list_node *, char *, char *, char *);
int connect_to_address(struct addrinfo *, char *);
void remove_node(struct list_node **, struct list_node **, struct list_node **);

struct addrinfo *configure_hints(struct addrinfo *hints) {
  memset (hints, 0, sizeof (*hints));
  hints->ai_family = PF_UNSPEC;
  hints->ai_socktype = SOCK_STREAM;
  hints->ai_protocol = IPPROTO_TCP;
  hints->ai_flags |= AI_CANONNAME | AI_ADDRCONFIG;
  return hints;
}


struct list_node *build_list(int argc, char **argv) {
  int i;
  struct list_node *head, *tail, *candidate;

  head = tail = build_node(argv[1]);

  for (i = 2 ; i < argc ; i++) {
    candidate = build_node(argv[i]);
    if (candidate) {
      tail->next = candidate;
      tail = tail->next;
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
    perror ("getaddrinfo");
    return NULL;
  }

  result = (struct list_node *) malloc(sizeof(struct list_node));
  memset(result, 0, sizeof(struct list_node));

  result->addresses = res;

  normalize_name(result, name, host, port);

  return result;
}

void process_list(struct list_node *list) {
  struct list_node *prev, *cur;
  struct addrinfo *address;
  int connected;

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

    if (list) sleep(SLEEP_SECONDS);
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
  if (name) {
    node->name = name;
    return;
  }

    int hlen = strnlen(node->addresses->ai_canonname, 255),
        plen = strnlen(port, 255);

    node->name = calloc(hlen + plen + 2, sizeof(char));
    strncpy(node->name, node->addresses->ai_canonname, hlen);
    node->name[hlen] = ':';
    strncpy(node->name + hlen + 1, port, plen);
    node->name[hlen + plen + 1] = '\0';
    node->allocated_name = 1;
}

int connect_to_address(struct addrinfo *address, char *name) {
  int s, result;
  u_int16_t port;
  char *ip = NULL;

  if ((s = socket(address->ai_family, address->ai_socktype, address->ai_protocol)) < 0) {
    perror("socket");
    exit(127);
  }

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
    strncpy(ip, "Unknown", 7);
  }

  printf("Trying %s (%s:%d) ... ", name, ip, port);
  free(ip);

  if ((result = connect(s, address->ai_addr, address->ai_addrlen)) < 0) {
    printf("failed.\n");
  } else {
    printf("SUCCESS!\n");
    close(s);
  }

  fflush(stdout);

  return result;
}
