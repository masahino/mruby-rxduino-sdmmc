#include "mruby.h"
#include "mruby/class.h"
#include "mruby/string.h"
#include "mruby/data.h"

#include "mruby-rxduino-sdmmc.h"

extern void mruby_rxduino_sdmmc_init(mrb_state *mrb);

void
mrb_mruby_rxduino_sdmmc_gem_init(mrb_state* mrb) 
{
	mruby_rxduino_sdmmc_init(mrb);
}

void
mrb_mruby_rxduino_sdmmc_gem_final(mrb_state* mrb) {

}


