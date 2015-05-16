#include "libmulticoreutils.h"

int main(int argc, char **argv){
  if (argc!=7){
    fprintf(stderr,
	    "%s\n%s\n",
	    "Usage: sqlsfromcsv csvfile skiplines schemafile tablename dbDir shardcount",
	    "Example: sqlsfromcsv example.csv 1 createmytable.sql mytable ./mytable 100");
    exit(EXIT_FAILURE);
  }
  int skiplines = 0;
  int shardcount = 0;
  const char *csvname = argv[1];
  errno = 0;
  skiplines = (int) mu_get_long_int_or_die(argv[2],
					   "sqlsfromcsv: parameter skiplines, expected number of lines to skip, got: %s \n");
  if (skiplines < 0)
    skiplines = -skiplines;
  const char *schemaname = argv[3];
  const char *schema = mu_read_small_file(schemaname);
  if (schema==NULL){
    fprintf(stderr,"Fatal: sqlsfromcsv While reading from schema file %s an error was encountered.\n",schemaname);
    if (errno>0)
      perror(NULL);
    exit(EXIT_FAILURE);
  }
  free((void *) schema);
  const char *tablename = argv[4];
  const char *dbDir = argv[5];
  shardcount = (int) mu_get_long_int_or_die(argv[6],
					    "sqlsfromscsv: expected positive number of shards, got: %s \n");

  if (shardcount<0)
    shardcount = -shardcount;
  if (shardcount<3)
    shardcount=3;
  
  mu_create_shards_from_csv(csvname,skiplines,schemaname,tablename,dbDir,shardcount);
  exit(EXIT_SUCCESS);
}