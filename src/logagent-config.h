#ifndef __LOGAGENT_CONFIG_H__
#define __LOGAGENT_CONFIG_H__

void logagent_config_load_from_file(const char *filename, char *json);

void logagent_config_load_from_network(const char *url, char *json);

#endif
