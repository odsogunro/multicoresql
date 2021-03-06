/* 3sqls.c 
   Copyright 2015 Paul Brewer <drpaulbrewer@eaftc.com> Economic and Financial Technology Consulting LLC
   License:  MIT
   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 documentation files (the "Software"), to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and 
to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/



#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "multicoresql.h"

int main(int argc, char **argv){
  char *dbname = NULL;  /* -d */
  char *tablename = NULL;  /* -t */
  char *mapsql = NULL; /* -m */
  char *reducesql = NULL; /* -r */
  int verbose = 0; /* -v */

  const char *getopt_options = ":d:t:m:r:v";
  int c;

  opterr = 1;
  
  while ((c = getopt(argc, argv, getopt_options)) != -1)
    switch(c)
      {
      case 'd':
	dbname = optarg;
	break;
      case 't':
	tablename = optarg;
	break;
      case 'm':
	mapsql = optarg;
	break;
      case 'r':
	reducesql = optarg;
	break;
      case 'v':
	verbose = 1;
	break;	
      case ':':
	fprintf(stderr,"Error: 3sqls is confused by your command.  Option -%c needs a setting. If the setting is more than one word, remember to put quotes around it. Please fix this and try again.  \n", optopt);
	exit(EXIT_FAILURE);
	break;
      case '?':
	if (isprint(optopt))
	  fprintf(stderr,"Error:  3sqls is confused by your command. I don't know what to do with '-%c' . Please fix your command and try again.  \n",optopt);
	else
	  fprintf(stderr, "%s\n", "Error: 3sqls is confused by your command because it contains unexpected non-printing characters.");
	return 1;
      default:
	abort();
      }

    if (dbname==NULL){
    fprintf(stderr,"%s\n","Error: 3sqls needs -d dbdir . Please tell 3sqls what database directory to use. Add -d dbdir to your command and try again");
    exit(EXIT_FAILURE);
  }
  
    struct mu_DBCONF * conf = mu_opendb(dbname);

  if (!conf){
    fprintf(stderr, "%s\n", "Could not set up database config for use");
    exit(EXIT_FAILURE);
  }
  
  conf->ncores = 3;

  if (verbose){
    fprintf(stdout,"x3 \n");
    fprintf(stdout,"number of cores (-c): %d\n",conf->ncores); 
    if (dbname) fprintf(stdout,"dbname              : %s \n",dbname);
    if (tablename) fprintf(stdout,"tablename           : %s \n",tablename);
    if (mapsql) fprintf(stdout,"mapsql:\n%s\n",mapsql);
    if (reducesql) fprintf(stdout,"reducesql:\n%s\n",reducesql);
  }

  if (mapsql==NULL){
    fprintf(stderr,"%s\n", "Error: 3sqls needs -m mapsql .  Please tell 3sqls what sql statments to execute (map) across the databases in the database directory.  Add -m \"statement1; statement2; statementN;\" or -m \"$(cat m.sql)\" to your command and try again. ");
    exit(EXIT_FAILURE);
  }
  
  char * result = mu_run_query(conf,
				mu_create_query(mapsql, NULL, reducesql)
				);
  if (result)
    fputs(result, stdout);
  const char *errors = mu_error_string();
  if (errors)
    fputs(errors, stderr);
}
