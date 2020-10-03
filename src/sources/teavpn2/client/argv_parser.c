
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>

#include <teavpn2/client/common.h>

#ifndef ARGV_DEBUG
#define ARGV_DEBUG 1
#endif

#if ARGV_DEBUG
  #define PRINT_CONFIG(CONFIG_NAME, VAL_LT, VAL) \
    printf("  "#CONFIG_NAME" = "VAL_LT"\n", VAL)
#else
  #define PRINT_CONFIG(CONFIG_NAME, VAL_LT, VAL)
#endif

inline static void show_help(char *app);
inline static void set_default_config(client_cfg *config);
inline static bool getopt_handler(int argc, char **argv, client_cfg *config);

static const uint16_t default_mtu = 1500;
static const uint16_t default_server_port = 55555;
static char default_dev_name[] = "tun0";

/**
 * Return false if parse fails.
 */
bool tvpn_client_argv_parse(
  int argc,
  char *argv[],
  char *envp[],
  client_cfg *config
)
{
  (void)envp;

  if (argc == 1) {
    printf("Usage: %s [options]\n", argv[0]);
    return false;
  }

  set_default_config(config);
  return getopt_handler(argc, argv, config);
}


/**
 * Initialize default config values.
 */
inline static void set_default_config(client_cfg *config)
{
  config->config_file = NULL;
  config->data_dir    = NULL;

  /* Virtual network interface. */
  config->iface.dev          = default_dev_name;
  config->iface.mtu          = default_mtu;

  /* Socket. */
  config->sock.server_addr = NULL;
  config->sock.server_port   = default_server_port;
  config->sock.type        = sock_tcp;
}

static const struct option long_options[] = {

  {"help",         no_argument      , 0, 'h'},
  {"config",       required_argument, 0, 'c'},

  /* Interface options. */
  {"dev",          required_argument, 0, 'd'},
  {"mtu",          required_argument, 0, 'm'},

  /* Socket options. */
  {"server-addr",  required_argument, 0, 'H'},
  {"server-port",  required_argument, 0, 'P'},
  {"sock-type",    required_argument, 0, 's'},

  {0, 0, 0, 0}
};


/**
 * Parse the arguments and plug it to config.
 */
inline static bool getopt_handler(int argc, char **argv, client_cfg *config)
{
  int c;

  #if ARGV_DEBUG
  printf("Parsing arguments...\n");
  #endif

  while (1) {
    int option_index = 0;
    /*int this_option_optind = optind ? optind : 1;*/
    c = getopt_long(argc, argv, "hc:d:m:H:P:s:", long_options, &option_index);

    if (c == -1)
      break;

    switch (c) {

      case 'h':
        show_help(argv[0]);
        return false;
        break;
      case 'c':
        config->config_file = optarg;
        PRINT_CONFIG(config->config_file, "\"%s\"", optarg);
        break;

      /* [Config options] */
      /* Virtual network interface: */
      case 'd':
        config->iface.dev = optarg;
        PRINT_CONFIG(config->iface.dev, "\"%s\"", optarg);
        break;
      case 'm':
        config->iface.mtu = (uint16_t)atoi(optarg);
        PRINT_CONFIG(config->iface.mtu, "%d", config->iface.mtu);
        break;

      /* Socket. */
      case 'H':
        config->sock.server_addr = optarg;
        PRINT_CONFIG(config->sock.server_addr, "\"%s\"", optarg);
        break;
      case 'P':
        config->sock.server_port = (uint16_t)atoi(optarg);
        PRINT_CONFIG(config->sock.server_port, "%d", config->sock.server_port);
        break;
      case 's': {
        char targ[4];

        strncpy(targ, optarg, 3);

        targ[0] = (targ[0] >= 'A' && targ[0] <= 'Z') ? targ[0] + 32 : targ[0];
        targ[1] = (targ[1] >= 'A' && targ[1] <= 'Z') ? targ[1] + 32 : targ[1];
        targ[2] = (targ[2] >= 'A' && targ[2] <= 'Z') ? targ[2] + 32 : targ[2];
        targ[3] = '\0';

        if (!strcmp(targ, "tcp")) {
          config->sock.type = sock_tcp;
        } else
        if (!strcmp(targ, "udp")) {
          config->sock.type = sock_udp;
        } else {
          printf("Invalid socket type: \"%s\"\n", optarg);
          return false;
        }

        PRINT_CONFIG(
          config->sock.type,
          "%s",
          (config->sock.type == sock_tcp) ? "tcp" : "udp"
        );
      }
        break;

      case 'u':
        config->data_dir = optarg;
        break;

      case '?':
      default:
        return false;
        break;
    }
  }

  return true;
}

inline static void show_help(char *app)
{
  printf("Usage: %s [options]\n", app);

  printf("\n");
  printf("TeaVPN2 Client (An open source VPN software).\n");

  printf("\n");
  printf("Available options:\n");
  printf("  -h, --help\t\t\tShow this help message.\n");
  printf("  -c, --config=FILE\t\tSet config file.\n");

  printf("\n");
  printf("[Config options]\n");
  printf("Virtual network interface:\n");
  printf("  -d, --dev=DEV\t\t\tSet virtual network interface name (default: %s).\n", default_dev_name);
  printf("  -m, --mtu=MTU\t\t\tSet mtu value (default: %d).\n", default_mtu);

  printf("\n");
  printf("Socket:\n");
  printf("  -H, --server-addr=IP\t\tSet server address.\n");
  printf("  -P, --server-port=PORT\t\tSet server port (default: %d).\n", default_server_port);
  printf("  -s, --sock-type=TYPE\t\tSet socket type (must be tcp or udp) (default: tcp).\n");

  printf("\n");
  printf("Other:\n");
  printf("  -u, --data-dir\t\tSet data directory.\n");

  printf("\n");
  printf("\n");
  printf("For bug reporting, please open an issue on GitHub repository.\n");
  printf("GitHub repository: https://github.com/TeaInside/teavpn2\n");
  printf("\n");
  printf("This software is licensed under the MIT license.\n");
}