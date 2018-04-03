// Data Science Assignment Program
//
#include <cstdio>
#include <ctime>
#include <vector>
#include <cstring>

#pragma warning (disable : 4996)

// Assumptions: much faster if assume session is less than one day
// Could use strptime, but not portable.  Not available on Windows and some other systems.
// keep sessions data structure small in order to be faster on very large data sets
// calculation of diff time is wrong.  Should subtract one second.
// there is a counter "line_no" in the code that is never used.  It's purpose is solely for debugging.
// assume EDGAR replace for last three characters of IP is three characters.
// fopen is only for 32-bit files.  Need to use file64 for very large files.
// two sessions loops could be combined into one for some performance gain.
// my tests are not in testsuite because they are too large for github
// github has a maximum size of 100 MB.

typedef struct {
	char   ip [16];       // ip address of session
	time_t start_time;    // session start time
	time_t last_time;     // time of last access
	int    sessions;      // number of sessions
} session_entry;

// parse calender string into a tm struct
static time_t calender_to_timet (const char* date, const char* time) {
	time_t stime;
	struct tm cal_time;
	int status;

	memset(&cal_time, 0, sizeof(cal_time));

	status = sscanf(date, "%4d-%2d-%2d", &(cal_time.tm_year), &(cal_time.tm_mon), &(cal_time.tm_yday));
	status = sscanf(time, "%2d:%2d:%2d", &(cal_time.tm_hour), &(cal_time.tm_min), &(cal_time.tm_sec));

	cal_time.tm_isdst = -1;
	cal_time.tm_year -= 1900;

	stime = mktime(&cal_time);

	return stime;
}

// output session entry to output file
static void print_entry(session_entry session_entry, FILE* out_file) {
	struct tm* temp_tm_ptr;
	char session_start[256], session_end[256];
	time_t diff_time;
	size_t length;

	temp_tm_ptr = localtime(&session_entry.start_time);
	length = strftime(session_start, 255, "%Y-%m-%d %H:%M:%S", temp_tm_ptr);

	if (session_entry.start_time == session_entry.last_time) {
		strcpy(session_end, session_start);
	} else {
		temp_tm_ptr = localtime(&session_entry.last_time);
		length = strftime(session_end, 255, "%Y-%m-%d %H:%M:%S", temp_tm_ptr);
	}

	diff_time = session_entry.last_time - session_entry.start_time + 1;

	fprintf(out_file, "%s,%s,%s,%d,%d\n", session_entry.ip, session_start, session_end, (int)diff_time, session_entry.sessions);
}

int main(int argc, char* argv[]) {
	FILE* log_file;                  // input edgar log file
	FILE* inactivity_file;           // file for inactivity counter
 	FILE* output_file;               // output sessions file
	time_t stime;
	std::vector<session_entry> session;
	session_entry temp_entry;
	char temp_line[256], temp_ip[256], temp_date[256], temp_time[256];
	char* temp;
	int ip_index = -1, date_index = -1, time_index = -1, token_no, found, inactivity_time, line_no, status;
	unsigned int i, max_index;
	char log_filename [256], inactivity_filename [256], sessionization_filename[256];

	if (argc < 4) {
		printf("Usage: process log.csv inactivity_period.txt sessionization.txt\n");
		return 1;
	}

	strcpy (log_filename,            argv[1]);
	strcpy (inactivity_filename,     argv[2]);
	strcpy (sessionization_filename, argv[3]);

	inactivity_file = fopen(inactivity_filename, "r");
	if (inactivity_file == 0) {
		printf("Unable to open inactivity file %s\n", inactivity_filename);
		return 1;
	}
	status = fscanf (inactivity_file, "%d", &inactivity_time);
	fclose (inactivity_file);

	log_file = fopen(log_filename, "r");
	if (log_file == 0) {
		printf("Unable to open log file %s\n", log_filename);
		return 2;
	}

	output_file = fopen(sessionization_filename, "w");
	if (output_file == 0) {
		printf("Unable to open output file %s\n", sessionization_filename);
		return 3;
	}

	ip_index   = -1;
	date_index = -1;
	time_index = -1;

	// read log file header
	if (fgets (temp_line, 255, log_file) == NULL) return 5; // empty file

	token_no = 0;
	temp = strtok(temp_line, ",");
	do {
		if (strcmp(temp, "ip") == 0) {  // look for matches
			ip_index = token_no;
		} else if (strcmp(temp, "date") == 0) {
			date_index = token_no;
		} else if (strcmp (temp, "time") == 0) {
			time_index = token_no;
		}

		temp = strtok(NULL, ",");
		token_no += 1;
	} while (temp != NULL);

	if ((ip_index == -1) || (date_index == -1) || (time_index == -1)) {
		printf("header missing from log file\n");
		return 3;
	}

	// find maximum required index
	max_index = ip_index;
	if (date_index > max_index) max_index = date_index;
	if (time_index > max_index) max_index = time_index;

	line_no = 0;
	while (fgets(temp_line, 255, log_file) != NULL) {  // run until end of file

		// guard against premature termination of an input line
		strcpy(temp_ip,   "");
		strcpy(temp_date, "");
		strcpy(temp_time, "");

		token_no = 0;
		temp = strtok(temp_line, ",");
		for (i = 0; i <= max_index; i = i + 1) {
			if (token_no == ip_index) { // look for matches
				strcpy(temp_ip, temp);
			} else if (token_no == date_index) {
				strcpy(temp_date, temp);
			} else if (token_no == time_index) {
				strcpy(temp_time, temp);
			}

			temp = strtok(NULL, ",");
			if (temp == NULL) break;  // early line termination

			token_no += 1;
		};

		// we now have a new entry stored in: temp_ip, temp_date, temp_time

		// convert calender time in log file into seconds since 1970
		stime = calender_to_timet (temp_date, temp_time);

		// check for sessions that have expired using the new time
		i = 0;
		found = false;
		while (i < session.size()) {
			if (stime - session[i].last_time > inactivity_time) { // expired
				print_entry(session[i], output_file);
				session.erase(session.begin() + i);
			} else {
				i = i + 1;
			}
		}

		// find whether or not a session with this ip address already exists
		found = false; 
		for (i = 0; i < session.size(); i += 1) {
			if (strcmp(session[i].ip, temp_ip) == 0) {
				found = true;
				break;
			}
		}

		if (!found) { // add this new entry into the sessions vector
			strcpy(temp_entry.ip, temp_ip);
			temp_entry.start_time = stime;
			temp_entry.last_time  = stime;
			temp_entry.sessions   = 1;

			session.push_back(temp_entry);
		} else { // an entry already exists
			session[i].sessions += 1;
			session[i].last_time = stime;
		}

		// Keep track of the current input line.  For debugging purposes only.
		line_no += 1; 
	}

	// Output whatever is left in the session vector
	for (i = 0; i < session.size(); i += 1) {
		print_entry(session [i], output_file);
	}

	fclose(log_file);
	fclose(output_file);

    return 0;
}

