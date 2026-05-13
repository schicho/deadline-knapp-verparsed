#ifndef CONFIGFILE_DESERIALIZE_H
#define CONFIGFILE_DESERIALIZE_H

#include <stdio.h>

#include "configfile.h"

config_block* config_deserialize(FILE* stream);

#endif /* CONFIGFILE_DESERIALIZE_H */