#include "options.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 1024

const char *argp_program_version =
  "shelltrace 0.1";
const char *argp_program_bug_address =
  "isra17 <isra017@gmail.com>";

static char doc[] = "Shelltrace -- Utility to trace shellcode execution";

static char args_doc[] = "";

static struct argp_option options[] = {
  {"trace-code", 'c', 0, 0, "Trace instructions"},
  {"trace-sys", 's', 0, 0, "Trace syscalls"},
  {"shellcode", 'f', "FILE", 0, "File with shellcode"},
  {0}
};

static int read_file_content(char* filename, char** buf) {
  int shellcode_fd = open(filename, O_RDONLY);
  if(shellcode_fd < 0) {
    return -1;
  }

  *buf = 0;
  int buf_size = 0;
  int ndata = 0;
  int nread = 0;

  do {
    if(ndata >= buf_size) {
      buf_size += BUF_SIZE;
      *buf = reallocf(*buf, buf_size);
      if(!*buf) {
        ndata = -1;
        goto CLEANUP;
      }
    }

    nread = read(shellcode_fd, *buf + ndata, buf_size - ndata);
    if(nread < 0) {
      ndata = -1;
      goto CLEANUP;
    }

    ndata += nread;
  } while(nread);

CLEANUP:
  close(shellcode_fd);
  return ndata;
}

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  struct st_options* options = state->input;
  switch(key){
    case 'c':
      options->trace_code = 1;
      break;
    case 's':
      options->trace_syscall = 1;
      break;
    case 'f':
      {
        options->shellcode_size = read_file_content(
            arg,
            &options->shellcode);

        if(options->shellcode_size < 0) {
          perror("read_file_content(shellcode)");
        }
      }
      break;
  }
  return 0;
}

static struct argp st_argp = {options, parse_opt, args_doc, doc};

void st_options_init(struct st_options* options) {
  options->trace_code = 0;
  options->trace_syscall = 1;
  options->shellcode_size = 0;
  options->shellcode = 0;
}

int st_options_parse(
    int argc,
    char** argv,
    struct st_options* options)
{
  return argp_parse(&st_argp, argc, argv, 0, 0, options);
}