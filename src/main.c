#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define PLATFORM "linux"
#define TIME_BUFFER 20
#define URL_BUFFER 100

struct post_info {
	long long int	timestamp;
	char*		body;
	bool		isCalendar;
};

cJSON* fetch_posts(int amount){	
	char* output_string;
	size_t output_string_size; //We don't use this, but open_memstream segfaults without it

	CURL* curl;
	curl_global_init(
			CURL_GLOBAL_SSL |
			CURL_GLOBAL_WIN32
			);
	
	curl = curl_easy_init();

	if(!curl){
		fprintf(stderr, "Something went wrong in curl");
	}
	
	char* url_string = calloc(sizeof(char), URL_BUFFER);
	snprintf(url_string, URL_BUFFER, "https://www.reddit.com/r/IsEricWearingShorts/new.json?limit=%d", amount);

	curl_easy_setopt(curl, CURLOPT_URL, url_string);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, PLATFORM ":net.ultrabanana.eric:v0.0.1 (by /u/vkb123)");

	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);

	FILE* output_fd = open_memstream(&output_string, &output_string_size);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, output_fd);

	curl_easy_perform(curl);
	free(curl);
	fflush(output_fd);
	
	cJSON* json = cJSON_Parse(output_string);
	free(output_string);

	return json;
}

struct post_info extract_data(cJSON* json, int index){ //This is usually 0, but calendar whatever whatever
	cJSON* data1 = cJSON_GetObjectItem(json, "data");
	cJSON* children = cJSON_GetObjectItem(data1, "children");
	cJSON* firstThingFromChildrenFuckYouJason = cJSON_GetArrayItem(children, index);
	cJSON* data2 = cJSON_GetObjectItem(firstThingFromChildrenFuckYouJason, "data");
	cJSON* answer_json = cJSON_GetObjectItem(data2, "selftext");
	cJSON* time_json = cJSON_GetObjectItem(data2, "created_utc");
	cJSON* title_json = cJSON_GetObjectItem(data2, "title");

	free(data1);free(children);free(firstThingFromChildrenFuckYouJason);free(data2);

	struct post_info post;
	
	post.timestamp = (long long int) time_json->valuedouble;
	post.body = answer_json->valuestring; //idk if this will work lol
	
	if(strstr(title_json->valuestring, "alendar")!=NULL){ //It checks against "alendar" because I can't be assed to do case insensitive stuff
		post.isCalendar = true;
	} else {
		post.isCalendar = false;
	}
	//I don't check for anything else because besides the calendar, the only irregular posts will still fit the code
	return post;
}

int print_status(){
	cJSON* post_json;
	struct post_info post;
	int i = 0;
	do{
		post_json = fetch_posts(i+1);
		post = extract_data(post_json, i);
		i++;
	} while(post.isCalendar);
	
	char* time_string = calloc(sizeof(char), TIME_BUFFER);
	struct tm* time_struct;
       	time_struct = localtime(&post.timestamp);
	strftime(time_string, TIME_BUFFER, "%x", time_struct);

	printf("Is Eric wearing shorts (as of %s):\n%s\n", 
			time_string,
			post.body); //TODO: Make it prettier. Needs some kind of logic
	
	return EXIT_SUCCESS;
}

#ifdef PAM_SUPPORT
#include <security/pam_modules.h>
int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv){
	print_status();
	return PAM_SUCCESS;
}
#else
int main(){
	return print_status();
}
#endif
