#include <new>
	
#include "mruby.h"
#include "mruby/class.h"
#include "mruby/string.h"
#include "mruby/data.h"

#include <sdmmc.h>

SDMMC MMC;

void mrb_sdmmc_free(mrb_state *mrb, void *ptr){
     SDMMC *sdmmc = (SDMMC *)ptr;
     sdmmc->~SDMMC();
     mrb_free(mrb, sdmmc);
}

struct mrb_data_type mrb_sdmmc_type = {"SDMMC", mrb_sdmmc_free};

void mrb_sdmmcfile_free(mrb_state *mrb, void *ptr){
     File *sdmmc_file = (File *)ptr;
     sdmmc_file->~File();
     mrb_free(mrb, sdmmc_file);
}

struct mrb_data_type mrb_sdmmcfile_type = {"SDMMCFile", mrb_sdmmcfile_free};

mrb_value mrb_sdmmc_get_version(mrb_state *mrb, mrb_value self)
{
     unsigned long ver_val;
     ver_val = SDMMC::getVersion(NULL);
     return mrb_fixnum_value(ver_val);
}

mrb_value mrb_sdmmc_begin(mrb_state *mrb, mrb_value self)
{
  mrb_int cspin;

  if (mrb_get_args(mrb, "|i", &cspin) == 0) {
       MMC.begin();
  } else {
       MMC.begin(cspin);
  }
  return mrb_nil_value();
}

mrb_value mrb_sdmmc_insert(mrb_state *mrb, mrb_value self)
{
  mrb_value ins;

  mrb_get_args(mrb, "o", &ins);
  if (mrb_bool(ins)) {
       MMC.insert(true);
  } else {
       MMC.insert(false);
  }
  
  return mrb_nil_value();
}

mrb_value mrb_sdmmc_exists(mrb_state *mrb, mrb_value self)
{
  mrb_value filename;
  bool ret;

  mrb_get_args(mrb, "S", &filename);
  ret = MMC.exists(mrb_string_value_ptr(mrb, filename));
  if (ret == true) {
       return mrb_true_value();
  } else {
       return mrb_false_value();
  }
}

mrb_value mrb_sdmmc_mkdir(mrb_state *mrb, mrb_value self)
{
  mrb_value pathname;
  bool ret;

  mrb_get_args(mrb, "S", &pathname);
  ret = MMC.mkdir(mrb_string_value_ptr(mrb, pathname));
  if (ret == true) {
       return mrb_true_value();
  } else {
       return mrb_false_value();
  }
}

mrb_value mrb_sdmmc_open(mrb_state *mrb, mrb_value self)
{
     mrb_value filename, file_obj;
     File *file = NULL;
     mrb_int mode;

     if (mrb_get_args(mrb, "S|i", &filename, &mode) == 1) {
	  mode = FILE_READ;
     }
     *file = MMC.open(mrb_string_value_ptr(mrb, filename), mode);
     DATA_PTR(file_obj) = file;  
     DATA_TYPE(file_obj) = &mrb_sdmmcfile_type;  
     return mrb_obj_value(mrb_data_object_alloc(mrb,
						mrb_class_get(mrb, "SDMMCFile"),
						file, &mrb_sdmmcfile_type));
}


mrb_value mrb_sdmmc_remove(mrb_state *mrb, mrb_value self)
{
     mrb_value filename;
     bool ret;

     mrb_get_args(mrb, "S", &filename);
     ret = MMC.remove(mrb_string_value_ptr(mrb, filename));
     if (ret == true) {
	  return mrb_true_value();
     } else {
	  return mrb_false_value();
     }
}

mrb_value mrb_sdmmc_rename(mrb_state *mrb, mrb_value self)
{
     mrb_value oldname, newname;
     bool ret;

     mrb_get_args(mrb, "SS", &oldname, &newname);
     ret = MMC.rename(mrb_string_value_ptr(mrb, oldname),
			 mrb_string_value_ptr(mrb, newname));
     if (ret == true) {
	  return mrb_true_value();
     } else {
	  return mrb_false_value();
     }
}

mrb_value mrb_sdmmc_rmdir(mrb_state *mrb, mrb_value self)
{
     mrb_value pathname;
     bool ret;
     
     mrb_get_args(mrb, "S", &pathname);

     ret = MMC.rmdir(mrb_string_value_ptr(mrb, pathname));
     
     if (ret == true) {
	  return mrb_true_value();
     } else {
	  return mrb_false_value();
     }
}

mrb_value mrb_sdmmcfile_close(mrb_state *mrb, mrb_value self)
{
     File *file = (File *)mrb_get_datatype(mrb, self, &mrb_sdmmcfile_type);
     file->close();
     return mrb_nil_value();
}

mrb_value mrb_sdmmcfile_available(mrb_state *mrb, mrb_value self)
{
     File *file = (File *)mrb_get_datatype(mrb, self, &mrb_sdmmcfile_type);
     bool ret;
     ret = file->available();
     if (ret == true) {
	  return mrb_true_value();
     } else {
	  return mrb_false_value();
     }
}

mrb_value mrb_sdmmcfile_read(mrb_state *mrb, mrb_value self)
{
     File *file = (File *)mrb_get_datatype(mrb, self, &mrb_sdmmcfile_type);

     return mrb_fixnum_value(file->read());
}

mrb_value mrb_sdmmcfile_write(mrb_state *mrb, mrb_value self)
{
     File *file = (File *)mrb_get_datatype(mrb, self, &mrb_sdmmcfile_type);
     mrb_int len, argc, ret;
     mrb_value buf;
     argc = mrb_get_args(mrb, "S|i", &buf, &len);
     if (argc == 2) {
	  ret = file->write((unsigned char*)mrb_string_value_ptr(mrb, buf), len);
     } else {
	  ret = file->write(mrb_string_value_ptr(mrb, buf));
     }
     return mrb_fixnum_value(ret);
//    size_t write(unsigned char c);
//    size_t write(const char *str);
//    size_t write(unsigned char *buf,int len);
     return self;
}

mrb_value mrb_sdmmcfile_flush(mrb_state *mrb, mrb_value self)
{
     File *file = (File *)mrb_get_datatype(mrb, self, &mrb_sdmmcfile_type);
     file->flush();
     return mrb_nil_value();
}

mrb_value mrb_sdmmcfile_peek(mrb_state *mrb, mrb_value self)
{
//     File *file = (File *)mrb_get_datatype(mrb, self, &mrb_sdmmcfile_type);
     return mrb_nil_value();
//     return mrb_fixnum_value(file->peek());
}

mrb_value mrb_sdmmcfile_position(mrb_state *mrb, mrb_value self)
{
     File *file = (File *)mrb_get_datatype(mrb, self, &mrb_sdmmcfile_type);
     return mrb_fixnum_value(file->position());
}

mrb_value mrb_sdmmcfile_seek(mrb_state *mrb, mrb_value self)
{
     File *file = (File *)mrb_get_datatype(mrb, self, &mrb_sdmmcfile_type);
     mrb_int pos;
     bool ret;
     mrb_get_args(mrb, "i", &pos);
     ret = file->seek(pos);
     if (ret == true) {
	  return mrb_true_value();
     } else {
	  return mrb_false_value();
     }
}

mrb_value mrb_sdmmcfile_print(mrb_state *mrb, mrb_value self)
{
     File *file = (File *)mrb_get_datatype(mrb, self, &mrb_sdmmcfile_type);
     mrb_value v;
     mrb_int size;

     mrb_get_args(mrb, "o", &v);
     switch(mrb_type(v)) {
     case MRB_TT_STRING:
	  size = file->print(mrb_string_value_ptr(mrb, v));
	  break;
     case MRB_TT_FIXNUM:
	  size = file->print(mrb_fixnum(v));
	  break;
     default:
	  size = 0;
     }
     return mrb_fixnum_value(size);
//    size_t print(const char str[]);
//     size_t print(const char c);
//    size_t print(unsigned char c, int print_type=DEC);
//     size_t print(int val, int print_type=DEC);

}

mrb_value mrb_sdmmcfile_println(mrb_state *mrb, mrb_value self)
{
     return self;
}

mrb_value mrb_sdmmcfile_size(mrb_state *mrb, mrb_value self)
{
     File *file = (File *)mrb_get_datatype(mrb, self, &mrb_sdmmcfile_type);
     return mrb_fixnum_value(file->size());
}

mrb_value mrb_sdmmcfile_name(mrb_state *mrb, mrb_value self)
{
     File *file = (File *)mrb_get_datatype(mrb, self, &mrb_sdmmcfile_type);
     return mrb_str_new_cstr(mrb, file->name());
}

mrb_value mrb_sdmmcfile_is_directory(mrb_state *mrb, mrb_value self)
{
     File *file = (File *)mrb_get_datatype(mrb, self, &mrb_sdmmcfile_type);
     bool ret;
     ret = file->isDirectory();
     if (ret == true) {
	  return mrb_true_value();
     } else {
	  return mrb_false_value();
     }
}

mrb_value mrb_sdmmcfile_open_next_file(mrb_state *mrb, mrb_value self)
{
     File *file = (File *)mrb_get_datatype(mrb, self, &mrb_sdmmcfile_type);
     File next_file = file->openNextFile();
     mrb_value file_obj;

     DATA_PTR(file_obj) = &next_file;  
     DATA_TYPE(file_obj) = &mrb_sdmmcfile_type;  
     return mrb_obj_value(mrb_data_object_alloc(mrb,
						mrb_class_get(mrb, "SDMMCFile"),
						&next_file, 
						&mrb_sdmmcfile_type));
}

mrb_value mrb_sdmmcfile_rewind_directory(mrb_state *mrb, mrb_value self)
{
     File *file = (File *)mrb_get_datatype(mrb, self, &mrb_sdmmcfile_type);
     file->rewindDirectory();
     return mrb_nil_value();
}


extern "C"
void
mruby_rxduino_sdmmc_init(mrb_state* mrb) 
{
RClass *sdmmcClass, *sdmmcfileClass;
sdmmcClass = mrb_define_class(mrb, "SDMMC", mrb->object_class);
MRB_SET_INSTANCE_TT(sdmmcClass, MRB_TT_DATA);
  mrb_define_class_method(mrb, sdmmcClass, "get_version", mrb_sdmmc_get_version, ARGS_NONE());
//  mrb_define_method(mrb, sdmmcClass, "initialize", mrb_sdmmc_initialize, ARGS_NONE());
  mrb_define_class_method(mrb, sdmmcClass, "begin", mrb_sdmmc_begin, ARGS_OPT(1));
  mrb_define_class_method(mrb, sdmmcClass, "insert", mrb_sdmmc_insert, ARGS_REQ(1));
  mrb_define_class_method(mrb, sdmmcClass, "exists", mrb_sdmmc_exists, ARGS_REQ(1));
  mrb_define_class_method(mrb, sdmmcClass, "mkdir", mrb_sdmmc_mkdir, ARGS_REQ(1));
  mrb_define_class_method(mrb, sdmmcClass, "open", mrb_sdmmc_open, ARGS_REQ(1));
  mrb_define_class_method(mrb, sdmmcClass, "remove", mrb_sdmmc_remove, ARGS_REQ(1));
  mrb_define_class_method(mrb, sdmmcClass, "rename", mrb_sdmmc_rename, ARGS_REQ(1));
  mrb_define_class_method(mrb, sdmmcClass, "rmdir", mrb_sdmmc_rename, ARGS_REQ(1));

sdmmcfileClass = mrb_define_class(mrb,  "SDMMCFile", mrb->object_class);
MRB_SET_INSTANCE_TT(sdmmcClass, MRB_TT_DATA);

  mrb_define_method(mrb, sdmmcfileClass, "close", mrb_sdmmcfile_close, ARGS_NONE());
  mrb_define_method(mrb, sdmmcfileClass, "available", mrb_sdmmcfile_available, ARGS_NONE());
  mrb_define_method(mrb, sdmmcfileClass, "read", mrb_sdmmcfile_read, ARGS_NONE());
  mrb_define_method(mrb, sdmmcfileClass, "write", mrb_sdmmcfile_write, ARGS_REQ(1) | ARGS_OPT(1));
  mrb_define_method(mrb, sdmmcfileClass, "flush", mrb_sdmmcfile_flush, ARGS_NONE());
  mrb_define_method(mrb, sdmmcfileClass, "peek", mrb_sdmmcfile_peek, ARGS_NONE());
  mrb_define_method(mrb, sdmmcfileClass, "position", mrb_sdmmcfile_position, ARGS_NONE());
  mrb_define_method(mrb, sdmmcfileClass, "seek", mrb_sdmmcfile_seek, ARGS_REQ(1));
  mrb_define_method(mrb, sdmmcfileClass, "print", mrb_sdmmcfile_print, ARGS_REQ(1) | ARGS_OPT(1));
  mrb_define_method(mrb, sdmmcfileClass, "println", mrb_sdmmcfile_println, ARGS_REQ(1) | ARGS_OPT(1));
  mrb_define_method(mrb, sdmmcfileClass, "size", mrb_sdmmcfile_size, ARGS_NONE());
  mrb_define_method(mrb, sdmmcfileClass, "name", mrb_sdmmcfile_name, ARGS_NONE());
  mrb_define_method(mrb, sdmmcfileClass, "is_directory", mrb_sdmmcfile_is_directory, ARGS_NONE());
  mrb_define_method(mrb, sdmmcfileClass, "open_next_file", mrb_sdmmcfile_open_next_file, ARGS_NONE());
  mrb_define_method(mrb, sdmmcfileClass, "rewind_directory", mrb_sdmmcfile_rewind_directory, ARGS_NONE());
  
  mrb_define_const(mrb, sdmmcClass, "FILE_BUFFER_SIZE", mrb_fixnum_value(FILE_BUFFER_SIZE));
  mrb_define_const(mrb, sdmmcClass, "FILENAME_SIZE", mrb_fixnum_value(FILENAME_SIZE));
  mrb_define_const(mrb, sdmmcClass, "FILE_READ", mrb_fixnum_value(FILE_READ));
  mrb_define_const(mrb, sdmmcClass, "FILE_WRITE", mrb_fixnum_value(FILE_WRITE));

}
