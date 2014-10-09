/* Memo is a Unix-style note taking software. 
 *
 * Copyright (c) 2014 Niko Rosvall <niko@newsworm.net>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

/* Function declarations */
char *read_memo_line(FILE *fp);
char *get_memo_file_path();
char *get_temp_memo_path();
int   add_note(const char *content, int linkid);
int   get_next_id();
int   get_longest_line();
int   delete_note(int id);
int   show_notes();
int   count_notes(FILE *fp);
int   search_notes(const char *search);
char *export_html();
void  output_line(char *line);
void  show_latest(int count);
FILE *get_memo_file_ptr();
void usage();


/* Count the lines of the file as a note is always one liner,
 * lines == note count.
 *
 * File pointer is rewinded to the beginning of the file.
 *
 * Returns the count or -1 if the file pointer is null.
 * Caller must close fp after calling the function successfully.
 */
int count_notes(FILE *fp)
{
	int count = 0;
	int ch = 0;

	if(!fp)
		return -1;

	/* Count lines by new line characters */
	while(!feof(fp)) {
		ch = fgetc(fp);
		if(ch == '\n')
			count++;
	}
	/* Go to beginning of the file */
	rewind(fp);

	/* return the count, ignoring the last empty line */
	/* "empty line" is because of the sick way I'm using feof. */
	return count - 1;
}


/* Get open FILE* for ~./memo file.
 * Returns NULL of failure.
 * Caller must close the file pointer after calling the function
 * succesfully.
 */
FILE *get_memo_file_ptr(char *mode) 
{
	FILE *fp = NULL;
	char *path = get_memo_file_path();

	if(path == NULL)
		return NULL;
	
	fp = fopen(path, mode);

	if(fp == NULL)
		return NULL;

	free(path);

	return fp;
}


/* Read a line from the .memo file.
 * Return NULL on failure.
 * Caller is responsible for freeing the return value
 */
char *read_memo_line(FILE *fp)
{
	if(!fp)
		return NULL;

	int length = 128;
	char *buffer = (char*)malloc(sizeof(char) * length);
	
	if(buffer == NULL)
		return NULL;

	int count = 0;

	char ch = getc(fp);

       /* Read char by char until the end of the line.
        * and allocate memory as needed.
        */
	while((ch != '\n') && (ch != EOF)) {
		if(count == length){
			length += 128;
			buffer = realloc(buffer, length);
			if(buffer == NULL)
				return NULL;
		}
		buffer[count] = ch;
		count++;
		ch = getc(fp);
	}

	buffer[count] = '\0';
	buffer = realloc(buffer, count + 1);

	return buffer;
}


/* Simply read all the lines from the ~/.memo file
 * and return the id of the last line plus one.
 * If the file is missing or is empty, return 0
 * On error, returns -1
 */
int get_next_id() 
{
	int id = 0;
	FILE *fp = NULL;
	char *line = NULL;
	int lines = 0;
	int current = 0;

	fp = get_memo_file_ptr("r");

	lines = count_notes(fp);

	if(lines == -1)
		return -1;

	while(1) {
		line = read_memo_line(fp);
		/* Check if we're at the last line */
		if(line && current == lines) {
			char *endptr;
			id = strtol(line, &endptr, 10);
			free(line);
			break;
	        }
		current++;

		if(line)
			free(line);
	}

	fclose(fp);

	return id + 1;
}


/* Show all notes.
 * Returns the number of notes;
 * Returns -1 on failure
 */
int show_notes()
{
	FILE *fp = NULL;
	char *line;
	int count = 0;
	int lines = 0;

	fp = get_memo_file_ptr("r");

	lines = count_notes(fp);
	count = lines;

	if(lines == -1)
		return -1;

	while(lines >= 0) {
		line = read_memo_line(fp);
		if(line) {
			output_line(line);
			free(line);
		}
		lines--;
	}

	fclose(fp);

	return count;
}

/* Search if a note contains the search term.
 * Returns the count of found notes or -1 if function fails.
 */
int search_notes(const char *search)
{
	FILE *fp = NULL;
	int count = 0;
	char *line;
	int lines = 0;

	fp = get_memo_file_ptr("r");

	lines = count_notes(fp);

	if(lines == -1)
		return -1;

	while(lines >= 0) {
		line = read_memo_line(fp);
		if(line){
			/* Check if the search term matches */
			const char *tmp = line;
			if((strstr(tmp, search)) != NULL){
				output_line(line);
				count++;
			}

			free(line);
		}
		lines--;
	}

	fclose(fp);

	return count;
}


/* Get the longest line of the ~/.memo file. */
int get_longest_line()
{
	FILE *fp = NULL;
	int lines = 0;
	char *line = NULL;
	int longest = 0;

	fp = get_memo_file_ptr("r");
	lines = count_notes(fp);

	if(lines == -1)
		return -1;

	while(lines >= 0){
		line = read_memo_line(fp);
		if(line){
			int tmp = strlen(line);

			if(tmp > longest)
				longest = tmp;

			free(line);
		}
		lines--;
	}

	fclose(fp);

	return longest;
}


/* This functions handles the output of one line.
 * Just a simple wrapper for printf for now.
 * Preserved for the future.
 */
void output_line(char *line)
{
	printf("%s\n", line);
}

/* Export current ~/.memo file to a html file ~/memo.html
 * Return the path of the html file, or NULL on failure.
 */
char *export_html()
{

	return NULL;
}

int link_notes(int id_from, int id_to)
{

	return 0;
}

/* Show latest n notes */
void show_latest(int n)
{
	FILE *fp = NULL;
	char *line;
	int lines = 0;
	int start;
	int current = 0;

	fp = get_memo_file_ptr("r");
	
	lines = count_notes(fp);

	if(lines != -1) {
		/* If n is bigger than the count of lines or smaller
		 * than zero we will show all the lines.
		 */
		if(n > lines || n < 0)
			start = -1;
		else
			start = lines - n;

		while(lines >= 0) {
			line = read_memo_line(fp);
			if(line) {
				if(current > start)
					output_line(line);
				free(line);
			}
			lines--;
			current++;
		}

		fclose(fp);
	}	
}


/* Delete a note by id.
 * Returns 0 on success and -1 on failure.
 */
int delete_note(int id) 
{
	FILE *fp = NULL;
	FILE *tmpfp = NULL;
	char *line = NULL;
	char *tmp;
	int lines = 0;

	tmp = get_temp_memo_path();

	if(tmp == NULL)
		return -1;

	char *memofile = get_memo_file_path();

	if(memofile == NULL)
		return -1;

	tmpfp = fopen(tmp, "w");

	if(tmpfp == NULL) {
		free(memofile);
		return -1;
	}

	fp = get_memo_file_ptr("r");
	lines = count_notes(fp);

	if(lines == -1) {
		free(memofile);
		fclose(tmpfp);
		return -1;
	}

	while(lines >= 0) {
		line = read_memo_line(fp);
		if(line){
			char *endptr;
			int curr = strtol(line, &endptr, 10);
	
			if(curr != id)
				fprintf(tmpfp, "%s\n", line);

			free(line);
		}
		lines--;
	}
	
	fclose(fp);
	fclose(tmpfp);

	if(access(memofile, F_OK) == 0)
		remove(memofile);

	rename(tmp, memofile);
	remove(tmp);
	
	free(memofile);
	free(tmp);

	return 0;
}


/*
 * Returns the path to $HOME/.memo file.
 * Caller is responsible for freeing the return value.
 */
char *get_memo_file_path() 
{
	char *env = getenv("HOME");
	char *path = (char*)malloc(1024 * sizeof(char));

	if(path == NULL)
		return NULL;

	if(env == NULL)
		return NULL;
	
	strcpy(path, env);
	strcat(path, "/.memo");

	return path;
}


/* Returns temporary .memo.tmp file.
 * It will be in the same directory
 * as the original .memo file.
 *
 * Returns NULL on failure.
 */
char *get_temp_memo_path()
{
	char *orig = get_memo_file_path();

	if(orig == NULL)
		return NULL;

	char *tmp = (char*)malloc(sizeof(char) * (strlen(orig) + 5));

	if(tmp == NULL)
		return NULL;

	strcpy(tmp, orig);
	strcat(tmp, ".tmp");

	return tmp;
}


/*
 * .memo file format is following:
 *
 * id     date           content       
 * |      |              |             
 * |- id  |- yyy-MM-dd   |- actual note
 *
 * sections are separated by a tab
 *
 */
int add_note(const char *content, int linkid)
{
	FILE *fp = NULL;
	time_t t;
	struct tm *ti;
	int id = -1;
	char date[11];


	fp = get_memo_file_ptr("a");

	if(fp == NULL){
		printf("Error opening ~/.memo\n");
		return -1;
	}

	time(&t);
	ti = localtime(&t);
	
	id = get_next_id();

	if(id == -1){
		id = 1;
	} 
	
	strftime(date, 80, "%Y-%m-%d", ti);
	fprintf(fp, "%d\t%s\t%s\n", id, date, content);
	

	fclose(fp);

	return id;
}


void usage()
{
	printf("Memo Copyright (C) 2014 Niko Rosvall <niko@newsworm.net>\n");
	printf("Usage: memo [option]...\n\n");
	printf("-a <content> [id]\tadd a new note, optionally as a subnote\n");
	printf("-d <id>\t\t\tdelete note by id\n");
	printf("-f <search>\t\tfind note by search term\n");
	printf("-h\t\t\tshow this help\n");
	printf("-l <n>\t\t\tshow latest n notes\n");
	printf("-s \t\t\tshow all notes\n");

}


int main(int argc, char *argv[])
{
	/* Don't link to any note by default */
	int linkid = 0;
	int c;

	opterr = 0;

	while((c = getopt(argc, argv, "a:d:f:hl:s")) != -1){
		switch(c){
		case 'a':
			if(argv[optind]){
				if(!isdigit(*argv[optind])){
					printf("%s must be a digit\n",argv[optind]);
					break;
				}
				linkid = atoi(argv[optind]);
			}
			add_note(optarg, linkid);
			break;
		case 'd':
			delete_note(atoi(optarg));
			break;
		case 'f':
			search_notes(optarg);
			break;
		case 'h':
			usage();
			break;
		case 'l':
			show_latest(atoi(optarg));
			break;
		case 's':
			show_notes();
			break;
		case '?':
			if(optopt == 'a')
				printf("-%c missing an argument <content>\n",optopt);
			if(optopt == 'd')
				printf("-%c missing an argument <id>\n",optopt);
			if(optopt == 'f')
				printf("-%c missing an argument <search>\n",optopt);
			if(optopt == 'l')
				printf("-%c missing an argument <id>\n",optopt);
			break;
			
		default:
			/* No options available, so get data from stdin.
			 * Assumes that the data is content for a new note.
			 * Adds a new note with content from stdin.
			 */
			;//stdin here

		}
	}

	return 0;
}
