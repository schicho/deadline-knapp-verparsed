#ifndef CONFIGFILE_SERIALIZE_H
#define CONFIGFILE_SERIALIZE_H

#include <stdio.h>

#include "configfile.h"

int config_serialize(config_block* block, FILE* stream);

#endif /* CONFIGFILE_SERIALIZE_H */
